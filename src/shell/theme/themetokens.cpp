// SPDX-License-Identifier: GPL-3.0-or-later
#include "themetokens.h"

#include <QCoreApplication>
#include <QFile>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaProperty>
#include <QQmlEngine>
#include <QRegularExpression>

#include <algorithm>
#include <cmath>
#include <utility>

using namespace Qt::StringLiterals;

namespace {

// Theme packs can come from anyone, on a USB stick.
constexpr qint64 largestThemeFile = 65536;

struct Range {
    const char *token;
    double min;
    double max;
};

// 24 px is the smallest text at 1080p (docs/research/A4-tv-text-size.md).
constexpr Range ranges[] = {
    { "textSmall", 24, 48 },
    { "textBody", 24, 64 },
    { "textTitle", 32, 96 },
    { "safeVertical", 54, 120 },
    { "safeSide", 80, 200 },
    { "gap", 8, 64 },
    { "radius", 0, 48 },
    { "focusRing", 4, 12 },
    { "outline", 0, 8 },
    { "duration", 0, 500 },
    { "focusScale", 1.0, 1.15 },
};

QString themePath(const QString &name)
{
    return u":/hearth/themes/%1.json"_s.arg(name);
}

qreal luminance(const QColor &color)
{
    const auto channel
        = [](qreal s) { return s <= 0.04045 ? s / 12.92 : std::pow((s + 0.055) / 1.055, 2.4); };
    return 0.2126 * channel(color.redF()) + 0.7152 * channel(color.greenF())
        + 0.0722 * channel(color.blueF());
}

// Reads one token from JSON. Returns an invalid QVariant if it isn't usable.
QVariant readToken(const QString &name, const QJsonValue &value, int type, QStringList &problems)
{
    if (type == QMetaType::QColor) {
        // Solid "#rrggbb" only. See-through colours would make the contrast check meaningless.
        static const QRegularExpression hex(u"\\A#[0-9a-fA-F]{6}\\z"_s);
        if (!value.isString() || !hex.match(value.toString()).hasMatch()) {
            problems << u"%1: not a #rrggbb colour"_s.arg(name);
            return { };
        }
        return QColor(value.toString());
    }
    const auto *range = std::find_if(std::begin(ranges), std::end(ranges),
        [&name](const Range &r) { return name == QLatin1StringView(r.token); });
    if (range == std::end(ranges) || !value.isDouble()) {
        problems << u"%1: not a number this token takes"_s.arg(name);
        return { };
    }
    const double asked = value.toDouble();
    const double used = std::clamp(asked, range->min, range->max);
    if (asked != used) {
        problems << u"%1: %2 changed to %3"_s.arg(name).arg(asked).arg(used);
    }
    return type == QMetaType::Int ? QVariant(qRound(used)) : QVariant(used);
}

template <typename Group>
void readGroup(const QJsonValue &json, Group &group, QStringList &problems)
{
    if (json.isUndefined()) {
        return;
    }
    if (!json.isObject()) {
        problems << u"a group of tokens isn't a JSON object"_s;
        return;
    }
    const QMetaObject &meta = Group::staticMetaObject;
    const QJsonObject tokens = json.toObject();
    for (auto it = tokens.begin(); it != tokens.end(); ++it) {
        const int index = meta.indexOfProperty(it.key().toUtf8().constData());
        if (index < 0) {
            problems << u"%1: no such token"_s.arg(it.key());
            continue;
        }
        const QMetaProperty property = meta.property(index);
        const QVariant value = readToken(it.key(), it.value(), property.typeId(), problems);
        if (value.isValid()) {
            property.writeOnGadget(&group, value);
        }
    }
}

} // namespace

ThemeTokens::ThemeTokens(QObject *parent)
    : QObject(parent)
{
    if (!loadBuiltin(u"dark"_s)) {
        qFatal("The built-in dark theme doesn't load: %s", qPrintable(m_problems.join(u"; "_s)));
    }
}

ThemeTokens *ThemeTokens::live()
{
    static ThemeTokens *tokens = [] {
        auto *created = new ThemeTokens(QCoreApplication::instance());
        QQmlEngine::setObjectOwnership(created, QQmlEngine::CppOwnership);
        return created;
    }();
    return tokens;
}

bool ThemeTokens::loadBuiltin(const QString &name)
{
    if (!builtinThemes().contains(name)) {
        m_problems = { u"%1: no such built-in theme"_s.arg(name) };
        return false;
    }
    return loadFile(themePath(name));
}

bool ThemeTokens::loadFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        m_problems = { u"can't read %1: %2"_s.arg(path, file.errorString()) };
        return false;
    }
    const QByteArray json = file.read(largestThemeFile + 1);
    if (json.size() > largestThemeFile) {
        m_problems = { u"%1 is too big for a theme file"_s.arg(path) };
        return false;
    }
    return loadJson(json);
}

bool ThemeTokens::loadJson(const QByteArray &json)
{
    m_problems.clear();
    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(json, &error);
    if (!document.isObject()) {
        m_problems << u"not a theme file: %1"_s.arg(error.errorString());
        return false;
    }

    ThemeColors colors;
    ThemeSizes sizes;
    ThemeMotion motion;
    QString fontFamily;
    QFile dark(themePath(u"dark"_s));
    if (dark.open(QIODevice::ReadOnly)) {
        readInto(
            QJsonDocument::fromJson(dark.readAll()).object(), colors, sizes, motion, fontFamily);
    }
    m_problems.clear();
    readInto(document.object(), colors, sizes, motion, fontFamily);
    if (!readable(colors, sizes)) {
        return false;
    }

    m_colors = colors;
    m_sizes = sizes;
    m_motion = std::move(motion);
    m_fontFamily = std::move(fontFamily);
    Q_EMIT changed();
    return true;
}

void ThemeTokens::readInto(const QJsonObject &theme, ThemeColors &colors, ThemeSizes &sizes,
    ThemeMotion &motion, QString &fontFamily)
{
    readGroup(theme.value("colors"_L1), colors, m_problems);
    readGroup(theme.value("sizes"_L1), sizes, m_problems);
    readGroup(theme.value("motion"_L1), motion, m_problems);
    const QString font = theme.value("font"_L1).toString();
    if (font.isEmpty()) {
        return;
    }
    // Qt swaps a missing font for another and says nothing.
    if (QFontDatabase::hasFamily(font)) {
        fontFamily = font;
    } else {
        m_problems << u"font: %1 isn't installed"_s.arg(font);
    }
}

bool ThemeTokens::readable(const ThemeColors &colors, const ThemeSizes &sizes)
{
    const struct {
        const char *what;
        QColor front;
        QColor back;
        qreal least;
    } pairs[] = {
        { "text on background", colors.text, colors.background, 4.5 },
        { "text on surface", colors.text, colors.surface, 4.5 },
        { "textDim on background", colors.textDim, colors.background, 4.5 },
        { "textDim on surface", colors.textDim, colors.surface, 4.5 },
        { "textOnAccent on accent", colors.textOnAccent, colors.accent, 4.5 },
        { "focusRing on background", colors.focusRing, colors.background, 3.0 },
        { "focusRing on surface", colors.focusRing, colors.surface, 3.0 },
    };
    bool good = true;
    for (const auto &pair : pairs) {
        const qreal ratio = contrast(pair.front, pair.back);
        if (ratio < pair.least) {
            m_problems << u"%1: contrast %2, needs %3"_s.arg(QLatin1StringView(pair.what))
                              .arg(ratio, 0, 'f', 2)
                              .arg(pair.least);
            good = false;
        }
    }
    // A theme that draws outlines does it so controls stand out from the background.
    if (sizes.outline > 0 && contrast(colors.outline, colors.background) < 3.0) {
        m_problems << u"outline on background: contrast below 3"_s;
        good = false;
    }
    return good;
}

ThemeSizes ThemeTokens::sizes() const
{
    const qreal screen = m_screenHeight / 1080.0;
    const auto scaled = [screen](int size) { return qRound(size * screen); };
    // Never scaled away: a ring or an outline the theme asks for is at least 1 px.
    const auto line = [&scaled](int size) { return size > 0 ? std::max(1, scaled(size)) : 0; };
    const auto text = [this, screen](int size) { return qRound(size * screen * m_textScale); };
    ThemeSizes sizes;
    sizes.textSmall = text(m_sizes.textSmall);
    sizes.textBody = text(m_sizes.textBody);
    sizes.textTitle = text(m_sizes.textTitle);
    sizes.safeVertical = scaled(m_sizes.safeVertical);
    sizes.safeSide = scaled(m_sizes.safeSide);
    sizes.gap = scaled(m_sizes.gap);
    sizes.radius = scaled(m_sizes.radius);
    sizes.focusRing = line(m_sizes.focusRing);
    sizes.outline = line(m_sizes.outline);
    return sizes;
}

ThemeMotion ThemeTokens::motion() const
{
    ThemeMotion motion = m_motion;
    if (m_reducedMotion) {
        motion.focusScale = 1.0;
    }
    return motion;
}

void ThemeTokens::setTextScale(qreal scale)
{
    const qreal used = std::clamp(scale, 1.0, 1.5);
    if (used != m_textScale) {
        m_textScale = used;
        Q_EMIT changed();
    }
}

void ThemeTokens::setReducedMotion(bool reduced)
{
    if (reduced != m_reducedMotion) {
        m_reducedMotion = reduced;
        Q_EMIT changed();
    }
}

void ThemeTokens::setScreenHeight(int height)
{
    const int used = std::max(height, 1);
    if (used != m_screenHeight) {
        m_screenHeight = used;
        Q_EMIT changed();
    }
}

QStringList ThemeTokens::builtinThemes()
{
    return { u"dark"_s, u"high-contrast"_s };
}

qreal ThemeTokens::contrast(const QColor &a, const QColor &b)
{
    const auto [darker, lighter] = std::minmax({ luminance(a), luminance(b) });
    return (lighter + 0.05) / (darker + 0.05);
}

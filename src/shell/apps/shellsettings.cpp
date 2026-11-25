// SPDX-License-Identifier: GPL-3.0-or-later
#include "shellsettings.h"

#include <QDir>
#include <QStandardPaths>

#include <algorithm>
#include <utility>

using namespace Qt::StringLiterals;

namespace {

// From one folder, so a photo collection doesn't make an endless list.
constexpr qsizetype mostPictures = 50;

} // namespace

ShellSettings::ShellSettings(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, u"hearth"_s, u"shell"_s)
{
}

int ShellSettings::columns() const
{
    return std::clamp(m_settings.value(u"home/columns"_s, 5).toInt(), 3, 7);
}

void ShellSettings::setColumns(int columns)
{
    save(u"home/columns"_s, std::clamp(columns, 3, 7));
}

int ShellSettings::rows() const
{
    return std::clamp(m_settings.value(u"home/rows"_s, 1).toInt(), 1, 2);
}

void ShellSettings::setRows(int rows)
{
    save(u"home/rows"_s, std::clamp(rows, 1, 2));
}

QUrl ShellSettings::background() const
{
    const QString path = m_settings.value(u"home/background"_s).toString();
    return path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path);
}

void ShellSettings::setBackground(const QUrl &background)
{
    save(u"home/background"_s, background.toLocalFile());
}

QString ShellSettings::theme() const
{
    return m_settings.value(u"look/theme"_s, u"dark"_s).toString();
}

void ShellSettings::setTheme(const QString &theme)
{
    save(u"look/theme"_s, theme);
}

int ShellSettings::textPercent() const
{
    return std::clamp(m_settings.value(u"look/textPercent"_s, 100).toInt(), 100, 150);
}

void ShellSettings::setTextPercent(int percent)
{
    save(u"look/textPercent"_s, std::clamp(percent, 100, 150));
}

bool ShellSettings::reducedMotion() const
{
    return m_settings.value(u"look/reducedMotion"_s, false).toBool();
}

void ShellSettings::setReducedMotion(bool reduced)
{
    save(u"look/reducedMotion"_s, reduced);
}

int ShellSettings::screensaverMinutes() const
{
    return std::clamp(m_settings.value(u"screensaver/minutes"_s, 2).toInt(), 0, 60);
}

void ShellSettings::setScreensaverMinutes(int minutes)
{
    save(u"screensaver/minutes"_s, std::clamp(minutes, 0, 60));
}

QVariantMap ShellSettings::extraKeys() const
{
    QVariantMap keys;
    for (const QString &action : { u"back"_s, u"home"_s, u"menu"_s }) {
        keys.insert(action, m_settings.value(u"keys/"_s + action).toStringList());
    }
    return keys;
}

void ShellSettings::setExtraKeys(const QVariantMap &keys)
{
    for (const QString &action : { u"back"_s, u"home"_s, u"menu"_s }) {
        save(u"keys/"_s + action, keys.value(action).toStringList());
    }
}

ShellSettings::RepeatSpeed ShellSettings::repeatSpeed() const
{
    return RepeatSpeed(
        std::clamp(m_settings.value(u"keys/repeat"_s, int(Normal)).toInt(), int(Slow), int(Fast)));
}

void ShellSettings::setRepeatSpeed(RepeatSpeed speed)
{
    save(u"keys/repeat"_s, int(speed));
}

bool ShellSettings::ignoreDoublePresses() const
{
    return m_settings.value(u"keys/ignoreDoublePresses"_s, false).toBool();
}

void ShellSettings::setIgnoreDoublePresses(bool ignore)
{
    save(u"keys/ignoreDoublePresses"_s, ignore);
}

QStringList ShellSettings::backgroundChoices() const
{
    QStringList folders = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation,
        u"hearth/backgrounds"_s, QStandardPaths::LocateDirectory);
    folders += QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QStringList pictures;
    for (const QString &folder : std::as_const(folders)) {
        const QFileInfoList files = QDir(folder).entryInfoList(
            { u"*.png"_s, u"*.jpg"_s, u"*.jpeg"_s, u"*.webp"_s }, QDir::Files, QDir::Name);
        for (const QFileInfo &file : files.first(std::min<qsizetype>(files.size(), mostPictures))) {
            pictures << file.filePath();
        }
    }
    return pictures;
}

QStringList ShellSettings::appOrder() const
{
    return m_settings.value(u"home/order"_s).toStringList();
}

void ShellSettings::setAppOrder(const QStringList &order)
{
    save(u"home/order"_s, order);
}

void ShellSettings::save(const QString &key, const QVariant &value)
{
    if (m_settings.value(key) != value) {
        m_settings.setValue(key, value);
        Q_EMIT changed();
    }
}

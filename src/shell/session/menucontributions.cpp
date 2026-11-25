// SPDX-License-Identifier: GPL-3.0-or-later
#include "menucontributions.h"

#include "applauncher.h"

#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDateTime>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSet>

using namespace Qt::StringLiterals;

namespace {

constexpr qsizetype longestMenu = 8192;
constexpr qsizetype mostItems = 12;
constexpr qsizetype mostOptions = 20;
constexpr qsizetype longestLabel = 40;
constexpr qsizetype longestId = 40;
constexpr int mostUpdates = 10;
constexpr qint64 updateWindowMs = 10'000;

// One line of plain text, of a bearable length.
QString label(const QJsonValue &value)
{
    return value.toString().simplified().left(longestLabel);
}

std::optional<QVariantMap> item(const QJsonObject &from)
{
    const QString id = from.value(u"id"_s).toString();
    const QString type = from.value(u"type"_s).toString();
    QVariantMap item { { u"id"_s, id }, { u"type"_s, type },
        { u"label"_s, label(from.value(u"label"_s)) },
        { u"enabled"_s, from.value(u"enabled"_s).toBool(true) } };
    if (id.isEmpty() || id.size() > longestId || item.value(u"label"_s).toString().isEmpty()) {
        return std::nullopt;
    }
    if (type == "action"_L1) {
        return item;
    }
    if (type == "toggle"_L1 && from.value(u"on"_s).isBool()) {
        item.insert(u"on"_s, from.value(u"on"_s).toBool());
        return item;
    }
    if (type == "choice"_L1) {
        const QJsonArray options = from.value(u"options"_s).toArray();
        const int current = from.value(u"current"_s).toInt(-1);
        if (options.isEmpty() || options.size() > mostOptions || current < 0
            || current >= options.size()) {
            return std::nullopt;
        }
        QStringList labels;
        for (const QJsonValue &option : options) {
            labels << label(option);
        }
        item.insert(u"options"_s, labels);
        item.insert(u"current"_s, current);
        return item;
    }
    return std::nullopt;
}

} // namespace

MenuContributions::MenuContributions(QObject *parent)
    : QObject(parent)
{
    new MenuAdaptor(this);
}

void MenuContributions::componentComplete()
{
    // In a test session the bus is the host desktop's.
    if (qEnvironmentVariable("HEARTH_SESSION_DEV") != "0"_L1
        || QFile::exists(u"/run/.containerenv"_s)) {
        return;
    }
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()
        || !bus.registerObject(u"/org/hearth/Menu"_s, this, QDBusConnection::ExportAdaptors)
        || !bus.registerService(u"org.hearth.Shell"_s)) {
        qWarning("apps can't add to the menu: no message bus");
        return;
    }
    // An app that ends takes its items with it.
    connect(bus.interface(), &QDBusConnectionInterface::serviceUnregistered, this,
        &MenuContributions::clear);
}

std::optional<QVariantList> MenuContributions::parse(const QByteArray &json)
{
    if (json.size() > longestMenu) {
        return std::nullopt;
    }
    const QJsonObject menu = QJsonDocument::fromJson(json).object();
    const QJsonArray from = menu.value(u"items"_s).toArray();
    if (menu.value(u"version"_s).toInt() != 1 || !menu.value(u"items"_s).isArray()
        || from.size() > mostItems) {
        return std::nullopt;
    }
    QVariantList items;
    QSet<QString> ids;
    for (const QJsonValue &value : from) {
        // One bad item, or one from a later version, doesn't lose the others.
        const auto read = item(value.toObject());
        if (!read) {
            continue;
        }
        const QString id = read->value(u"id"_s).toString();
        if (!ids.contains(id)) {
            ids << id;
            items << *read;
        }
    }
    return items;
}

MenuContributions::Result MenuContributions::set(
    const QString &appId, const QString &sender, const QByteArray &json)
{
    if (appId.isEmpty()) {
        return Refused;
    }
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    m_updates.removeIf([now](qint64 time) { return now - time > updateWindowMs; });
    if (m_updates.size() >= mostUpdates) {
        return TooOften;
    }
    m_updates << now;
    const auto items = parse(json);
    if (!items) {
        return Refused;
    }
    m_appId = appId;
    m_sender = sender;
    m_items = *items;
    Q_EMIT changed();
    return Accepted;
}

void MenuContributions::clear(const QString &sender)
{
    if (sender == m_sender && !m_appId.isEmpty()) {
        m_appId.clear();
        m_sender.clear();
        m_items.clear();
        Q_EMIT changed();
    }
}

void MenuContributions::choose(const QString &id, const QVariant &value)
{
    if (!m_sender.isEmpty()) {
        Q_EMIT chosen(m_sender, id, value);
    }
}

QString MenuContributions::appOf(const QString &sender) const
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!m_launcher || !bus.isConnected()) {
        return { };
    }
    const QDBusReply<uint> pid = bus.interface()->servicePid(sender);
    return pid.isValid() ? m_launcher->appOfProcess(pid.value()) : QString();
}

MenuAdaptor::MenuAdaptor(MenuContributions *menu)
    : QDBusAbstractAdaptor(menu)
    , m_menu(menu)
{
    connect(menu, &MenuContributions::chosen, this,
        [](const QString &sender, const QString &id, const QVariant &value) {
            QDBusMessage signal = QDBusMessage::createTargetedSignal(
                sender, u"/org/hearth/Menu"_s, u"org.hearth.Menu1"_s, u"ItemChosen"_s);
            signal << id << QVariant::fromValue(QDBusVariant(value));
            QDBusConnection::sessionBus().send(signal);
        });
}

bool MenuAdaptor::SetMenu(const QString &json, const QDBusMessage &message)
{
    const QString sender = message.service();
    return m_menu->set(m_menu->appOf(sender), sender, json.toUtf8()) == MenuContributions::Accepted;
}

void MenuAdaptor::ClearMenu(const QDBusMessage &message)
{
    m_menu->clear(message.service());
}

// SPDX-License-Identifier: GPL-3.0-or-later
#include "notifications.h"

#include "buses.h"

#include <QDBusError>
#include <QDBusMessage>
#include <QTextDocumentFragment>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace {

constexpr qsizetype mostKept = 50;
constexpr qsizetype longestName = 60;
constexpr qsizetype longestSummary = 120;
constexpr qsizetype longestBody = 400;
// From one app.
constexpr int mostPerWindow = 10;
constexpr qint64 windowMs = 10'000;

// Why a notification closed, in the specification's numbers.
constexpr uint dismissedByPerson = 2;
constexpr uint closedByApp = 3;
constexpr uint undefinedReason = 4;

// Apps may send markup whether or not it's supported.
QString plain(const QString &text, qsizetype longest)
{
    // Cut first: turning a huge text into plain text takes a while.
    QString result = text.left(longest * 4);
    if (result.contains(u'<') || result.contains(u'&')) {
        result = QTextDocumentFragment::fromHtml(result).toPlainText();
    }
    result.remove(QChar::ObjectReplacementCharacter);
    result = result.simplified();
    if (result.size() > longest) {
        result = result.left(longest - 1).trimmed() + u'…';
    }
    return result;
}

} // namespace

Notifications::Notifications(QObject *parent)
    : QAbstractListModel(parent)
{
    new NotificationsAdaptor(this);
}

void Notifications::componentComplete()
{
    QDBusConnection bus = Buses::session();
    if (!bus.isConnected()) {
        return;
    }
    if (!attach(bus) || !bus.registerService(u"org.freedesktop.Notifications"_s)) {
        qWarning("another program already takes notifications");
    }
}

bool Notifications::attach(QDBusConnection bus)
{
    return bus.registerObject(
        u"/org/freedesktop/Notifications"_s, this, QDBusConnection::ExportAdaptors);
}

bool Notifications::tooMany(const QString &sender)
{
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    // Apps that have gone quiet needn't be remembered.
    for (auto it = m_recent.begin(); it != m_recent.end();) {
        it->removeIf([now](qint64 time) { return now - time > windowMs; });
        it = it->isEmpty() ? m_recent.erase(it) : std::next(it);
    }
    QList<qint64> &times = m_recent[sender];
    if (times.size() >= mostPerWindow) {
        return true;
    }
    times << now;
    return false;
}

uint Notifications::add(const QString &sender, const QString &app, uint replaces,
    const QString &summary, const QString &body, bool transient)
{
    if (tooMany(sender)) {
        return 0;
    }

    Entry entry;
    entry.sender = sender;
    entry.app = plain(app, longestName);
    entry.summary = plain(summary, longestSummary);
    entry.body = plain(body, longestBody);
    entry.time = QDateTime::currentDateTime();

    // An app can only replace its own.
    const auto old = std::find_if(m_list.begin(), m_list.end(), [replaces, &sender](const Entry &e) {
        return replaces != 0 && e.id == replaces && e.sender == sender;
    });
    if (old != m_list.end()) {
        entry.id = old->id;
        const auto row = int(old - m_list.begin());
        *old = entry;
        Q_EMIT dataChanged(index(row), index(row));
    } else {
        entry.id = ++m_lastId;
        // "transient": of no use later, such as "track changed".
        if (!transient) {
            if (m_list.size() >= mostKept) {
                remove(m_list.size() - 1, undefinedReason);
            }
            beginInsertRows({ }, 0, 0);
            m_list.prepend(entry);
            endInsertRows();
            Q_EMIT countChanged();
            setUnseen(m_unseen + 1);
        }
    }
    Q_EMIT arrived(entry.app, entry.summary, entry.body);
    return entry.id;
}

void Notifications::close(const QString &sender, uint id)
{
    for (qsizetype row = 0; row < m_list.size(); ++row) {
        if (m_list.at(row).id == id && m_list.at(row).sender == sender) {
            remove(row, closedByApp);
            return;
        }
    }
}

void Notifications::remove(qsizetype row, uint reason)
{
    const uint id = m_list.at(row).id;
    beginRemoveRows({ }, int(row), int(row));
    m_list.removeAt(row);
    endRemoveRows();
    Q_EMIT countChanged();
    setUnseen(std::min(m_unseen, count()));
    Q_EMIT closed(id, reason);
}

void Notifications::dismiss(int row)
{
    if (row >= 0 && row < count()) {
        remove(row, dismissedByPerson);
    }
}

void Notifications::clear()
{
    while (!m_list.isEmpty()) {
        remove(m_list.size() - 1, dismissedByPerson);
    }
}

void Notifications::markSeen()
{
    setUnseen(0);
}

void Notifications::setUnseen(int unseen)
{
    if (unseen != m_unseen) {
        m_unseen = unseen;
        Q_EMIT unseenChanged();
    }
}

int Notifications::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : count();
}

QVariant Notifications::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid)) {
        return { };
    }
    const Entry &entry = m_list.at(index.row());
    switch (role) {
    case IdRole:
        return entry.id;
    case AppRole:
        return entry.app;
    case SummaryRole:
        return entry.summary;
    case BodyRole:
        return entry.body;
    case TimeRole:
        return entry.time;
    default:
        return { };
    }
}

QHash<int, QByteArray> Notifications::roleNames() const
{
    return { { IdRole, "notificationId" }, { AppRole, "app" }, { SummaryRole, "summary" },
        { BodyRole, "body" }, { TimeRole, "time" } };
}

NotificationsAdaptor::NotificationsAdaptor(Notifications *notifications)
    : QDBusAbstractAdaptor(notifications)
    , m_notifications(notifications)
{
    connect(notifications, &Notifications::closed, this, &NotificationsAdaptor::NotificationClosed);
}

QStringList NotificationsAdaptor::GetCapabilities()
{
    return { u"body"_s, u"persistence"_s };
}

QString NotificationsAdaptor::GetServerInformation(
    QString &vendor, QString &version, QString &specVersion)
{
    vendor = u"Hearth"_s;
    version = QStringLiteral(HEARTH_VERSION);
    specVersion = u"1.2"_s;
    return u"Hearth"_s;
}

uint NotificationsAdaptor::Notify(const QString &appName, uint replaces, const QString &,
    const QString &summary, const QString &body, const QStringList &, const QVariantMap &hints,
    int, const QDBusMessage &message)
{
    return m_notifications->add(message.service(), appName, replaces, summary, body,
        hints.value(u"transient"_s).toBool());
}

void NotificationsAdaptor::CloseNotification(uint id, const QDBusMessage &message)
{
    m_notifications->close(message.service(), id);
}

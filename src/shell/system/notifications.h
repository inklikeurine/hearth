// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QAbstractListModel>
#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDateTime>
#include <QHash>
#include <QQmlParserStatus>
#include <QStringList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

// Hearth's notification service: what apps' notifications arrive at
// (https://specifications.freedesktop.org/notification-spec/latest/), and the
// list behind the bell, newest first. Text only: apps' pictures and markup are
// left out. Nothing is written to disk.
class Notifications : public QAbstractListModel, public QQmlParserStatus {
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    // Not seen yet.
    Q_PROPERTY(int unseen READ unseen NOTIFY unseenChanged)

public:
    enum Role { IdRole = Qt::UserRole + 1, AppRole, SummaryRole, BodyRole, TimeRole };

    explicit Notifications(QObject *parent = nullptr);

    void classBegin() override { }
    void componentComplete() override;
    // Answers apps on this connection.
    bool attach(QDBusConnection bus);

    [[nodiscard]] int count() const
    {
        return int(m_list.size());
    }
    [[nodiscard]] int unseen() const
    {
        return m_unseen;
    }
    [[nodiscard]] int rowCount(const QModelIndex &parent = { }) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    // Adds a notification from SENDER, an app's name on the bus. Returns its
    // number, or 0 when the app has sent too many in a short time.
    uint add(const QString &sender, const QString &app, uint replaces, const QString &summary,
        const QString &body, bool transient);
    // An app can only close its own.
    void close(const QString &sender, uint id);

    Q_INVOKABLE void dismiss(int row);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void markSeen();

Q_SIGNALS:
    void countChanged();
    void unseenChanged();
    // For a pop-up.
    void arrived(const QString &app, const QString &summary, const QString &body);
    void closed(uint id, uint reason);

private:
    struct Entry {
        uint id = 0;
        QString sender;
        QString app;
        QString summary;
        QString body;
        QDateTime time;
    };

    bool tooMany(const QString &sender);
    void remove(qsizetype row, uint reason);
    void setUnseen(int unseen);

    QList<Entry> m_list;
    QHash<QString, QList<qint64>> m_recent;
    uint m_lastId = 0;
    int m_unseen = 0;
};

// How apps reach it. The names are the specification's.
class NotificationsAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.Notifications")

public:
    explicit NotificationsAdaptor(Notifications *notifications);

Q_SIGNALS:
    void NotificationClosed(uint id, uint reason);
    void ActionInvoked(uint id, const QString &action);

public Q_SLOTS:
    QStringList GetCapabilities();
    QString GetServerInformation(QString &vendor, QString &version, QString &specVersion);
    uint Notify(const QString &appName, uint replaces, const QString &icon, const QString &summary,
        const QString &body, const QStringList &actions, const QVariantMap &hints, int timeout,
        const QDBusMessage &message);
    void CloseNotification(uint id, const QDBusMessage &message);

private:
    Notifications *m_notifications;
};

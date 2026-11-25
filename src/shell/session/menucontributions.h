// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QObject>
#include <QPointer>
#include <QQmlParserStatus>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include <optional>

class AppLauncher;

// What apps add to the system menu. docs/menu-api.md is the specification, and
// has the limits. Apps send data; Hearth draws it. An app's own name and icon
// never come from here.
class MenuContributions : public QObject, public QQmlParserStatus {
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(AppLauncher *launcher MEMBER m_launcher NOTIFY launcherChanged)
    // The app the items below belong to.
    Q_PROPERTY(QString appId READ appId NOTIFY changed)
    Q_PROPERTY(QVariantList items READ items NOTIFY changed)

public:
    enum Result { Accepted, Refused, TooOften };

    explicit MenuContributions(QObject *parent = nullptr);

    void classBegin() override { }
    void componentComplete() override;

    // Nothing if the whole menu is refused.
    static std::optional<QVariantList> parse(const QByteArray &json);

    Result set(const QString &appId, const QString &sender, const QByteArray &json);
    void clear(const QString &sender);

    [[nodiscard]] QString appId() const
    {
        return m_appId;
    }
    [[nodiscard]] QVariantList items() const
    {
        return m_items;
    }

    // Passes on what the person chose. Only the app's next menu changes anything.
    Q_INVOKABLE void choose(const QString &id, const QVariant &value);

Q_SIGNALS:
    void launcherChanged();
    void changed();
    // For the app at SENDER only.
    void chosen(const QString &sender, const QString &id, const QVariant &value);

private:
    friend class MenuAdaptor;
    [[nodiscard]] QString appOf(const QString &sender) const;

    QPointer<AppLauncher> m_launcher;
    QString m_appId;
    QString m_sender;
    QVariantList m_items;
    QList<qint64> m_updates;
};

// How native apps reach it: org.hearth.Menu1.
class MenuAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.hearth.Menu1")

public:
    explicit MenuAdaptor(MenuContributions *menu);

Q_SIGNALS:
    void ItemChosen(const QString &id, const QDBusVariant &value);

public Q_SLOTS:
    // Whether it was accepted.
    bool SetMenu(const QString &json, const QDBusMessage &message);
    void ClearMenu(const QDBusMessage &message);

private:
    MenuContributions *m_menu;
};

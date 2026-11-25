// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "desktopentry.h"
#include "shellsettings.h"

#include <QAbstractListModel>
#include <QFileSystemWatcher>
#include <QPointer>
#include <QQmlParserStatus>
#include <QTimer>

// The apps on the home screen: everything installed that has a .desktop file
// meant for people, Flatpaks included, in this profile's order. New apps go at
// the end. It notices apps being installed and removed.
class AppList : public QAbstractListModel, public QQmlParserStatus {
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QStringList folders READ folders WRITE setFolders NOTIFY foldersChanged)
    Q_PROPERTY(ShellSettings *settings READ settings WRITE setSettings NOTIFY settingsChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum Role { IdRole = Qt::UserRole + 1, NameRole, PictureRole };
    Q_ENUM(Role)

    explicit AppList(QObject *parent = nullptr);

    void classBegin() override { }
    void componentComplete() override;

    [[nodiscard]] QStringList folders() const
    {
        return m_folders;
    }
    void setFolders(const QStringList &folders);
    [[nodiscard]] ShellSettings *settings() const
    {
        return m_settings;
    }
    void setSettings(ShellSettings *settings);
    [[nodiscard]] int count() const
    {
        return int(m_apps.size());
    }

    [[nodiscard]] int rowCount(const QModelIndex &parent = { }) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    [[nodiscard]] const QList<DesktopEntry> &apps() const
    {
        return m_apps;
    }
    // Moves an app to another place, and remembers it.
    Q_INVOKABLE void moveApp(int from, int to);

Q_SIGNALS:
    void foldersChanged();
    void settingsChanged();
    void countChanged();

private:
    void load();
    void watch();
    [[nodiscard]] QList<DesktopEntry> installed() const;
    void saveOrder();

    QStringList m_folders;
    QPointer<ShellSettings> m_settings;
    QList<DesktopEntry> m_apps;
    QFileSystemWatcher m_watcher;
    QTimer m_reload;
    bool m_complete = false;
};

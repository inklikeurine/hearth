// SPDX-License-Identifier: GPL-3.0-or-later
#include "applist.h"

#include <QDir>
#include <QDirIterator>
#include <QIcon>
#include <QLocale>
#include <QSet>
#include <QStandardPaths>
#include <QUrl>

#include <algorithm>
#include <utility>

using namespace Qt::StringLiterals;

namespace {

// Installing an app changes several files at once; wait for them to settle.
constexpr int settleMs = 500;

bool sameApps(const QList<DesktopEntry> &a, const QList<DesktopEntry> &b)
{
    return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](const auto &x, const auto &y) {
        return x.id == y.id && x.name == y.name && x.icon == y.icon && x.command == y.command
            && x.windowClass == y.windowClass;
    });
}

QUrl pictureFor(const QString &icon)
{
    const bool found = QDir::isAbsolutePath(icon) ? QFile::exists(icon)
                                                  : !icon.isEmpty() && QIcon::hasThemeIcon(icon);
    if (!found) {
        return { };
    }
    return QUrl(u"image://icon/"_s + QString::fromLatin1(QUrl::toPercentEncoding(icon)));
}

} // namespace

AppList::AppList(QObject *parent)
    : QAbstractListModel(parent)
    , m_folders(QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation))
{
    m_reload.setSingleShot(true);
    m_reload.setInterval(settleMs);
    connect(&m_reload, &QTimer::timeout, this, &AppList::load);
    connect(
        &m_watcher, &QFileSystemWatcher::directoryChanged, &m_reload, qOverload<>(&QTimer::start));
}

void AppList::componentComplete()
{
    m_complete = true;
    load();
}

void AppList::setFolders(const QStringList &folders)
{
    if (folders == m_folders) {
        return;
    }
    m_folders = folders;
    Q_EMIT foldersChanged();
    if (m_complete) {
        load();
    }
}

void AppList::setSettings(ShellSettings *settings)
{
    if (settings == m_settings) {
        return;
    }
    m_settings = settings;
    Q_EMIT settingsChanged();
    if (m_complete) {
        load();
    }
}

QList<DesktopEntry> AppList::installed() const
{
    const QString language = QLocale().name();
    // A file in an earlier folder hides one with the same ID in a later folder,
    // even when the earlier one isn't shown: that's how an app gets hidden.
    QSet<QString> seen;
    QList<DesktopEntry> apps;
    for (const QString &folder : m_folders) {
        QDirIterator files(folder, { u"*.desktop"_s }, QDir::Files, QDirIterator::Subdirectories);
        while (files.hasNext()) {
            const QString path = files.next();
            QString id = QDir(folder).relativeFilePath(path);
            id.chop(8);
            id.replace(u'/', u'-');
            if (seen.contains(id)) {
                continue;
            }
            seen.insert(id);
            if (auto entry = DesktopEntry::read(path, id, language)) {
                apps << std::move(*entry);
            }
        }
    }
    return apps;
}

void AppList::load()
{
    QList<DesktopEntry> apps = installed();
    const QStringList order = m_settings ? m_settings->appOrder() : QStringList();
    const auto place = [&order](const DesktopEntry &app) {
        const qsizetype index = order.indexOf(app.id);
        return index < 0 ? order.size() : index;
    };
    std::sort(apps.begin(), apps.end(), [&place](const auto &a, const auto &b) {
        const auto placeA = place(a);
        const auto placeB = place(b);
        if (placeA != placeB) {
            return placeA < placeB;
        }
        const int byName = QString::localeAwareCompare(a.name, b.name);
        return byName != 0 ? byName < 0 : a.id < b.id;
    });
    watch();
    if (sameApps(apps, m_apps)) {
        return;
    }
    const bool countDiffers = apps.size() != m_apps.size();
    beginResetModel();
    m_apps = std::move(apps);
    endResetModel();
    if (countDiffers) {
        Q_EMIT countChanged();
    }
    saveOrder();
}

void AppList::watch()
{
    for (const QString &folder : std::as_const(m_folders)) {
        if (QDir(folder).exists() && !m_watcher.directories().contains(folder)) {
            m_watcher.addPath(folder);
        }
    }
}

// Remembers where every app is, so the next new one goes after all of them. Apps
// that are gone keep their place: one that's reinstalled comes back where it was.
void AppList::saveOrder()
{
    if (!m_settings) {
        return;
    }
    QStringList order;
    for (const DesktopEntry &app : std::as_const(m_apps)) {
        order << app.id;
    }
    const QStringList before = m_settings->appOrder();
    for (qsizetype i = 0; i < before.size(); ++i) {
        if (!order.contains(before.at(i))) {
            order.insert(std::min(i, order.size()), before.at(i));
        }
    }
    m_settings->setAppOrder(order);
}

int AppList::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : count();
}

QVariant AppList::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid)) {
        return { };
    }
    const DesktopEntry &app = m_apps.at(index.row());
    switch (role) {
    case IdRole:
        return app.id;
    case NameRole:
        return app.name;
    case PictureRole:
        return pictureFor(app.icon);
    default:
        return { };
    }
}

QHash<int, QByteArray> AppList::roleNames() const
{
    return { { IdRole, "appId" }, { NameRole, "name" }, { PictureRole, "picture" } };
}

void AppList::moveApp(int from, int to)
{
    if (from == to || from < 0 || to < 0 || from >= count() || to >= count()) {
        return;
    }
    beginMoveRows({ }, from, from, { }, to > from ? to + 1 : to);
    m_apps.move(from, to);
    endMoveRows();
    saveOrder();
}

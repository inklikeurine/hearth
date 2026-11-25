// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QString>
#include <QStringList>

#include <optional>

// One installed app, read from its .desktop file
// (https://specifications.freedesktop.org/desktop-entry-spec/latest/).
struct DesktopEntry {
    QString id;
    QString name;
    QString icon;
    // The program and its arguments, ready to start with no file or address.
    QStringList command;
    // The window's app ID, when it isn't the same as the .desktop file's name.
    QString windowClass;

    // Nothing if the file isn't an app Hearth should show.
    static std::optional<DesktopEntry> read(
        const QString &path, const QString &id, const QString &language);
    // Splits an Exec line by the spec's quoting rules. Nothing if it breaks them.
    static std::optional<QStringList> splitExec(const QString &exec);
};

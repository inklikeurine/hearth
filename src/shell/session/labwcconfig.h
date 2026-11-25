// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "shellsettings.h"

#include <QObject>
#include <QPointer>
#include <QtQml/qqmlregistration.h>

// labwc's settings for this profile: the ones Hearth ships, with the profile's own
// buttons and key-repeat speed put in.
class LabwcConfig : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(ShellSettings *settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    using QObject::QObject;

    [[nodiscard]] ShellSettings *settings() const
    {
        return m_settings;
    }
    void setSettings(ShellSettings *settings);

    static QByteArray make(const ShellSettings &settings);
    // Writes rc.xml in FOLDER. labwc reads it from there.
    static bool write(const ShellSettings &settings, const QString &folder);
    // Whether a button may be given a meaning: not one that types, moves or
    // selects, or the remote could lock its owner out.
    static bool mayBeUsed(const QString &key);

    // Writes the running session's copy again and tells labwc.
    Q_INVOKABLE void apply();

Q_SIGNALS:
    void settingsChanged();

private:
    QPointer<ShellSettings> m_settings;
};

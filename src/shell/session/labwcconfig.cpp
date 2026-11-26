// SPDX-License-Identifier: GPL-3.0-or-later
#include "labwcconfig.h"

#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSaveFile>

#include <csignal>
#include <xkbcommon/xkbcommon.h>

using namespace Qt::StringLiterals;

namespace {

constexpr auto place
    = "    <!-- The profile's own buttons go here; the shell writes them. -->\n"_L1;
constexpr int mostKeys = 8;

struct Repeat {
    int perSecond;
    int delayMs;
};

Repeat repeatFor(ShellSettings::RepeatSpeed speed)
{
    switch (speed) {
    case ShellSettings::Slow:
        return { 8, 1200 };
    case ShellSettings::Fast:
        return { 40, 400 };
    case ShellSettings::Normal:
        break;
    }
    return { 25, 600 };
}

} // namespace

void LabwcConfig::setSettings(ShellSettings *settings)
{
    if (settings != m_settings) {
        m_settings = settings;
        Q_EMIT settingsChanged();
    }
}

bool LabwcConfig::mayBeUsed(const QString &key)
{
    static const QRegularExpression name(u"^[A-Za-z0-9_]{1,40}$"_s);
    static const QStringList never { u"Up"_s, u"Down"_s, u"Left"_s, u"Right"_s, u"Return"_s,
        u"KP_Enter"_s, u"Select"_s, u"Tab"_s, u"BackSpace"_s, u"Escape"_s };
    if (!name.match(key).hasMatch() || never.contains(key)) {
        return false;
    }
    const xkb_keysym_t symbol
        = xkb_keysym_from_name(key.toLatin1().constData(), XKB_KEYSYM_NO_FLAGS);
    // Letters, digits and signs type something; Shift and the like change others.
    const bool types = xkb_keysym_to_utf32(symbol) >= 0x20;
    const bool modifier = symbol >= XKB_KEY_Shift_L && symbol <= XKB_KEY_Hyper_R;
    return symbol != XKB_KEY_NoSymbol && !types && !modifier;
}

QByteArray LabwcConfig::make(const ShellSettings &settings)
{
    QFile shipped(u":/hearth/labwc/rc.xml"_s);
    if (!shipped.open(QIODevice::ReadOnly)) {
        return { };
    }
    QString config = QString::fromUtf8(shipped.readAll());

    const Repeat repeat = repeatFor(settings.repeatSpeed());
    config.replace(u"<repeatRate>25<"_s, u"<repeatRate>%1<"_s.arg(repeat.perSecond));
    config.replace(u"<repeatDelay>600<"_s, u"<repeatDelay>%1<"_s.arg(repeat.delayMs));

    QString extra;
    const QVariantMap keys = settings.extraKeys();
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        int count = 0;
        for (const QString &key : it.value().toStringList()) {
            // The file may have been changed by hand.
            if (!mayBeUsed(key) || ++count > mostKeys) {
                continue;
            }
            extra += u"    <keybind key=\"%1\" onRelease=\"yes\"><action name=\"Execute\" "
                     "command=\"hearth-send %2\" /></keybind>\n"_s.arg(key, it.key());
        }
    }
    // Before the usual ones' end: labwc uses the last binding of a key.
    config.replace(place, extra);
    return config.toUtf8();
}

bool LabwcConfig::write(const ShellSettings &settings, const QString &folder)
{
    const QByteArray config = make(settings);
    QSaveFile file(folder + u"/rc.xml"_s);
    return !config.isEmpty() && QDir().mkpath(folder) && file.open(QIODevice::WriteOnly)
        && file.write(config) == config.size() && file.commit();
}

void LabwcConfig::apply()
{
    // hearth-session says where; labwc gives its programs its own number.
    const QString folder = qEnvironmentVariable("HEARTH_LABWC_CONFIG");
    const int labwc = qEnvironmentVariableIntValue("LABWC_PID");
    if (!m_settings || folder.isEmpty() || labwc <= 1) {
        return;
    }
    if (write(*m_settings, folder)) {
        ::kill(labwc, SIGHUP);
    } else {
        qWarning("can't write labwc's settings in %s", qPrintable(folder));
    }
}

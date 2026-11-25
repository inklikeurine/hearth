// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Controls

// The time, the way the region writes it.
Label {
    id: clock

    property date now: new Date()

    function tick() {
        clock.now = new Date();
        // At the next minute, not a minute after some odd second.
        timer.interval = 60000 - clock.now.getSeconds() * 1000 - clock.now.getMilliseconds() + 50;
        timer.restart();
    }

    Accessible.name: KI18n.i18n("Time: %1", clock.text)
    Accessible.role: Accessible.StaticText
    elide: Text.ElideNone
    text: clock.now.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)

    Component.onCompleted: clock.tick()

    Timer {
        id: timer

        onTriggered: clock.tick()
    }
}

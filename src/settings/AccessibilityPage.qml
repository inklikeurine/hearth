// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Controls

SettingsPage {
    id: page

    Slider {
        from: 100
        objectName: "text"
        step: 10
        text: KI18n.i18n("Text size")
        to: 150
        value: page.settings.textPercent
        valueText: KI18n.i18n("%1%", value)
        width: parent.width

        onMoved: value => page.settings.textPercent = value
    }

    Toggle {
        objectName: "contrast"
        on: page.settings.theme === "high-contrast"
        text: KI18n.i18n("High contrast")
        width: parent.width

        onSwitched: on => page.settings.theme = on ? "high-contrast" : "dark"
    }

    Toggle {
        objectName: "motion"
        on: page.settings.reducedMotion
        text: KI18n.i18n("Less movement on screen")
        width: parent.width

        onSwitched: on => page.settings.reducedMotion = on
    }

    Toggle {
        detail: KI18n.i18n("Three apps in a row, one row at a time")
        objectName: "tiles"
        on: page.settings.columns === 3 && page.settings.rows === 1
        text: KI18n.i18n("Bigger tiles")
        width: parent.width

        onSwitched: on => {
            page.settings.columns = on ? 3 : 5;
            page.settings.rows = 1;
        }
    }

    Choice {
        current: page.settings.repeatSpeed
        detail: KI18n.i18n("How fast a held button repeats")
        objectName: "repeat"
        options: [KI18n.i18n("Slow"), KI18n.i18n("Normal"), KI18n.i18n("Fast")]
        text: KI18n.i18n("Held buttons")
        width: parent.width

        onChosen: index => page.settings.repeatSpeed = index as ShellSettings.RepeatSpeed
    }

    Toggle {
        detail: KI18n.i18n("On Hearth's own screens. Apps still get every press")
        objectName: "double"
        on: page.settings.ignoreDoublePresses
        text: KI18n.i18n("Ignore double presses")
        width: parent.width

        onSwitched: on => page.settings.ignoreDoublePresses = on
    }
}

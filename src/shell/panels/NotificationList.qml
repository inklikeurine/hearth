// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.System

// What's behind the bell. Select removes one.
SidePanel {
    id: list

    required property Notifications notifications

    title: KI18n.i18n("Notifications")

    Label {
        objectName: "nothing"
        text: KI18n.i18n("Nothing new.")
        visible: list.notifications.count === 0
        width: parent.width
    }

    ListRow {
        iconName: "close"
        objectName: "clear"
        text: list.notifications.count > 0 ? KI18n.i18n("Clear all") : KI18n.i18n("Close")
        width: parent.width

        onClicked: {
            if (list.notifications.count > 0) {
                list.notifications.clear();
            }
            list.dismissed();
        }
    }

    Repeater {
        model: list.notifications

        ListRow {
            id: row

            required property string app
            required property string body
            required property int index
            required property string summary
            required property date time

            detail: [row.app, row.time.toLocaleTimeString(Qt.locale(), Locale.ShortFormat), row.body].filter(part => part.length > 0).join(" · ")
            text: row.summary
            width: parent?.width ?? 0

            onClicked: list.notifications.dismiss(row.index)
        }
    }
}

// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.System
import Hearth.Theme

// The profile, the network, the time, the bell and power.
FocusScope {
    id: bar

    // NetworkState's value, so a test can show every one.
    property int network: NetworkState.Unknown
    property string profileName
    property bool resting: false
    property int unseen: 0

    signal bellPressed
    signal powerPressed

    implicitHeight: row.implicitHeight
    implicitWidth: row.implicitWidth

    BurnInGuard {
        anchors.fill: parent
        resting: bar.resting

        Row {
            id: row

            spacing: Theme.tokens.sizes.gap

            Icon {
                anchors.verticalCenter: parent.verticalCenter
                name: "profile"
                visible: bar.profileName.length > 0
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter
                text: bar.profileName
                visible: text.length > 0
                width: Math.min(implicitWidth, Theme.tokens.sizes.textBody * 10)
            }

            Icon {
                Accessible.ignored: false
                Accessible.name: {
                    switch (bar.network) {
                    case NetworkState.Wired:
                        return KI18n.i18n("Connected by cable");
                    case NetworkState.Wifi:
                        return KI18n.i18n("Connected by Wi-Fi");
                    default:
                        return KI18n.i18n("Not connected");
                    }
                }
                Accessible.role: Accessible.Graphic
                anchors.verticalCenter: parent.verticalCenter
                name: {
                    switch (bar.network) {
                    case NetworkState.Wired:
                        return "wired";
                    case NetworkState.Wifi:
                        return "wifi";
                    default:
                        return "none";
                    }
                }
                objectName: "network"
                visible: bar.network !== NetworkState.Unknown
            }

            Clock {
                anchors.verticalCenter: parent.verticalCenter
            }

            IconButton {
                id: bell

                KeyNavigation.right: power
                badge: bar.unseen
                focus: true
                iconName: "bell"
                objectName: "bell"
                text: KI18n.i18n("Notifications")

                onClicked: bar.bellPressed()
            }

            IconButton {
                id: power

                iconName: "power"
                objectName: "power"
                text: KI18n.i18n("Power")

                onClicked: bar.powerPressed()
            }
        }
    }
}

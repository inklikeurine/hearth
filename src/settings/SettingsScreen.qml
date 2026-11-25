// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Apps
import Hearth.Controls
import Hearth.Theme

// The list of settings pages, and the one that's open.
FocusScope {
    id: screen

    readonly property var pages: [
        {
            name: KI18n.i18n("Appearance"),
            file: "AppearancePage"
        },
        {
            name: KI18n.i18n("Accessibility"),
            file: "AccessibilityPage"
        },
        {
            name: KI18n.i18n("Remotes"),
            file: "RemotesPage"
        }
    ]
    // The page that's open, by its place in the list; -1 is the list itself.
    property int open: -1
    required property ShellSettings settings

    signal closed

    function show() {
        screen.open = -1;
        list.current = null;
        list.start();
    }

    function openPage(index: int) {
        screen.open = index;
        loader.forceActiveFocus();
        const page = loader.item as SettingsPage;
        if (page) {
            page.start();
        }
    }

    function goBack() {
        const page = loader.item as SettingsPage;
        if (screen.open < 0) {
            screen.closed();
        } else if (!page || !page.goBack()) {
            screen.open = -1;
            list.forceActiveFocus();
        }
    }

    Accessible.name: heading.text
    Accessible.role: Accessible.Pane

    Rectangle {
        Accessible.ignored: true
        anchors.fill: parent
        color: Theme.tokens.colors.background
    }

    Label {
        id: heading

        anchors.left: parent.left
        anchors.leftMargin: Theme.tokens.sizes.safeSide
        anchors.top: parent.top
        anchors.topMargin: Theme.tokens.sizes.safeVertical
        font.pixelSize: Theme.tokens.sizes.textTitle
        objectName: "heading"
        text: screen.open < 0 ? KI18n.i18n("Settings") : screen.pages[screen.open].name
        width: parent.width - 2 * Theme.tokens.sizes.safeSide
    }

    Item {
        id: middle

        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.tokens.sizes.safeVertical
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: heading.bottom
        anchors.topMargin: Theme.tokens.sizes.gap
        width: Math.min(parent.width - 2 * Theme.tokens.sizes.safeSide, parent.height * 1.2)

        FocusColumn {
            id: list

            anchors.fill: parent
            focus: screen.open < 0
            objectName: "list"
            visible: screen.open < 0

            Repeater {
                model: screen.pages

                ListRow {
                    id: row

                    required property int index
                    required property var modelData

                    objectName: row.modelData.file
                    text: row.modelData.name
                    width: parent?.width ?? 0

                    onClicked: screen.openPage(row.index)

                    Icon {
                        anchors.verticalCenter: parent.verticalCenter
                        name: "right"
                    }
                }
            }
        }

        Loader {
            id: loader

            active: screen.open >= 0
            anchors.fill: parent
            focus: screen.open >= 0
            sourceComponent: [appearance, accessibility, remotes][screen.open] ?? null
        }
    }

    Component {
        id: appearance

        AppearancePage {
            focus: true
            settings: screen.settings
        }
    }

    Component {
        id: accessibility

        AccessibilityPage {
            focus: true
            settings: screen.settings
        }
    }

    Component {
        id: remotes

        RemotesPage {
            focus: true
            settings: screen.settings
        }
    }
}

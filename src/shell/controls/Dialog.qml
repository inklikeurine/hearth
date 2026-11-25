// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import Hearth.Theme

// A question over what's behind it, with one button for each answer, the first
// one focused. Fills its parent, which it dims.
FocusScope {
    id: dialog

    property list<string> answers: []
    property string text
    property string title

    signal answered(int index)

    function show() {
        const first = buttons.itemAt(0);
        if (first) {
            first.forceActiveFocus();
        }
    }

    Accessible.description: dialog.text
    Accessible.name: dialog.title
    Accessible.role: Accessible.Dialog
    anchors.fill: parent

    Rectangle {
        Accessible.ignored: true
        anchors.fill: parent
        color: Qt.alpha(dialog.Theme.tokens.colors.background, 0.7)
    }

    // Clicks stay out of what's behind.
    MouseArea {
        anchors.fill: parent
    }

    Rectangle {
        anchors.centerIn: parent
        border.color: Theme.tokens.colors.outline
        border.width: Math.max(Theme.tokens.sizes.outline, 2)
        color: Theme.tokens.colors.background
        height: content.height + 4 * Theme.tokens.sizes.gap
        radius: Theme.tokens.sizes.radius
        width: Math.min(dialog.width - 2 * Theme.tokens.sizes.safeSide, dialog.height * 0.9)

        Column {
            id: content

            anchors.centerIn: parent
            spacing: Theme.tokens.sizes.gap
            width: parent.width - 4 * Theme.tokens.sizes.gap

            Label {
                elide: Text.ElideNone
                font.pixelSize: Theme.tokens.sizes.textTitle
                text: dialog.title
                visible: text.length > 0
                width: parent.width
                wrapMode: Text.Wrap
            }

            Label {
                elide: Text.ElideNone
                text: dialog.text
                visible: text.length > 0
                width: parent.width
                wrapMode: Text.Wrap
            }

            Repeater {
                id: buttons

                model: dialog.answers

                Button {
                    id: button

                    required property int index
                    required property string modelData

                    KeyNavigation.down: button.index + 1 < buttons.count ? buttons.itemAt(button.index + 1) : null
                    KeyNavigation.up: button.index > 0 ? buttons.itemAt(button.index - 1) : null
                    focus: button.index === 0
                    primary: button.index === 0
                    text: button.modelData
                    width: content.width

                    onClicked: dialog.answered(button.index)
                }
            }
        }
    }
}

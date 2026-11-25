// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.Failures
import Hearth.Theme

// What every failure looks like: one sentence, what to do next, the error code
// and a QR code to its help page.
FocusScope {
    id: screen

    // Buttons after "OK", such as "Try again".
    property list<string> actions: []
    required property failureText failure

    signal actionChosen(int index)
    signal dismissed

    function show() {
        ok.forceActiveFocus();
    }

    Accessible.description: screen.failure.next
    Accessible.name: screen.failure.what
    Accessible.role: Accessible.AlertMessage

    Rectangle {
        Accessible.ignored: true
        anchors.fill: parent
        color: Theme.tokens.colors.background
    }

    Column {
        id: words

        anchors.left: parent.left
        anchors.leftMargin: Theme.tokens.sizes.safeSide
        anchors.right: qr.left
        anchors.rightMargin: Theme.tokens.sizes.safeSide
        anchors.verticalCenter: parent.verticalCenter
        spacing: Theme.tokens.sizes.gap

        Label {
            elide: Text.ElideNone
            font.pixelSize: Theme.tokens.sizes.textTitle
            objectName: "what"
            text: screen.failure.what
            width: parent.width
            wrapMode: Text.Wrap
        }

        Label {
            elide: Text.ElideNone
            objectName: "next"
            text: screen.failure.next
            width: parent.width
            wrapMode: Text.Wrap
        }

        Label {
            color: Theme.tokens.colors.textDim
            objectName: "code"
            text: KI18n.i18n("Error code: %1", screen.failure.code)
            width: parent.width
        }

        Row {
            spacing: Theme.tokens.sizes.gap
            topPadding: Theme.tokens.sizes.gap

            Button {
                id: ok

                KeyNavigation.right: others.count > 0 ? others.itemAt(0) : null
                primary: true
                text: KI18n.i18n("OK")

                onClicked: screen.dismissed()
            }

            Repeater {
                id: others

                model: screen.actions

                Button {
                    id: other

                    required property int index
                    required property string modelData

                    KeyNavigation.left: other.index > 0 ? others.itemAt(other.index - 1) : ok
                    KeyNavigation.right: other.index + 1 < others.count ? others.itemAt(other.index + 1) : null
                    text: other.modelData

                    onClicked: screen.actionChosen(other.index)
                }
            }
        }
    }

    QrCode {
        id: qr

        readonly property int side: Math.round(screen.height * 0.4)

        Accessible.name: KI18n.i18n("QR code to the help page for %1", screen.failure.code)
        Accessible.role: Accessible.Graphic
        anchors.right: parent.right
        anchors.rightMargin: Theme.tokens.sizes.safeSide
        anchors.verticalCenter: parent.verticalCenter
        height: qr.side
        text: screen.failure.help.toString()
        width: qr.side
    }

    Label {
        anchors.horizontalCenter: qr.horizontalCenter
        anchors.top: qr.bottom
        anchors.topMargin: Theme.tokens.sizes.gap / 2
        color: Theme.tokens.colors.textDim
        font.pixelSize: Theme.tokens.sizes.textSmall
        horizontalAlignment: Text.AlignHCenter
        text: KI18n.i18n("Scan for help")
        width: qr.width
    }
}

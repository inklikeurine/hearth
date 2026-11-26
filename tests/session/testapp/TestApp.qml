// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Controls

Window {
    id: window

    readonly property bool fixed: options.includes("fixed")
    required property var log
    required property string name
    required property list<string> options

    function note(line: string) {
        window.log.write(line);
    }

    color: "darkslateblue"
    height: 480
    maximumHeight: window.fixed ? 480 : 16777215
    maximumWidth: window.fixed ? 640 : 16777215
    minimumHeight: window.fixed ? 480 : 0
    minimumWidth: window.fixed ? 640 : 0
    title: window.name
    visibility: window.options.includes("fullscreen") ? Window.FullScreen : Window.Windowed
    width: 640

    Component.onCompleted: window.note("started")
    onClosing: close => {
        window.note("asked to close");
        if (window.options.includes("stubborn")) {
            close.accepted = false;
            question.visible = true;
        }
    }

    Window {
        id: question

        color: "darkred"
        height: 200
        modality: Qt.WindowModal
        title: "Save your work?"
        transientParent: window
        width: 300
    }

    TextInput {
        id: box

        anchors.centerIn: parent
        color: "white"
        focus: window.options.includes("text")
        font.pixelSize: 40
        visible: window.options.includes("text") || second.visible
        width: 400

        KeyNavigation.down: second
        onAccepted: {
            window.note("accepted");
            if (second.visible) {
                second.forceActiveFocus();
            }
        }
        onTextChanged: window.note("text " + box.text)
    }

    // Hearth's own, as its screens have it.
    TextBox {
        id: second

        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: box.bottom
        inputMethodHints: Qt.ImhDigitsOnly
        label: "A number"
        visible: window.options.includes("boxes")

        onAccepted: rest.forceActiveFocus()
        onTextChanged: window.note("second " + second.text)
    }

    Item {
        id: rest

        anchors.fill: parent
        focus: !window.options.includes("text")

        KeyNavigation.down: second.visible ? box : null

        Keys.onPressed: event => window.note("key " + event.key.toString(16))
    }
}

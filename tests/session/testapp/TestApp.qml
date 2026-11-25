// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick

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
        visible: window.options.includes("text")
        width: 400

        onAccepted: window.note("accepted")
        onTextChanged: window.note("text " + box.text)
    }

    Item {
        anchors.fill: parent
        focus: !box.visible

        Keys.onPressed: event => window.note("key " + event.key.toString(16))
    }
}

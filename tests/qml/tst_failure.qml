// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTest
import Hearth.Controls
import Hearth.Failures
import Hearth.Theme

Item {
    id: root

    height: 1080
    width: 1920

    ThemeTokens {
        id: large

        textScale: 1.5
    }

    FailureScreen {
        id: screen

        actions: ["Try again", "Go home"]
        anchors.fill: parent
        failure: Failures.describe("HEARTH-APP-001", "A very long app name that goes on and on")
    }

    FailureScreen {
        id: largeScreen

        Theme.tokens: large
        anchors.fill: parent
        failure: Failures.describe("HEARTH-APP-001", "A very long app name that goes on and on")
        opacity: 0
    }

    Dialog {
        id: dialog

        answers: ["First", "Second", "Third"]
        text: "The text"
        title: "The question"
        visible: false
    }

    SignalSpy {
        id: dismissed

        signalName: "dismissed"
        target: screen
    }

    SignalSpy {
        id: chosen

        signalName: "actionChosen"
        target: screen
    }

    SignalSpy {
        id: answered

        signalName: "answered"
        target: dialog
    }

    TestCase {
        function focusedText(): string {
            const button = root.Window.activeFocusItem as Button;
            return button ? button.text : "nothing";
        }

        function init() {
            dismissed.clear();
            chosen.clear();
            answered.clear();
            dialog.visible = false;
            screen.show();
        }

        function test_startsOnOk() {
            compare(focusedText(), "OK");
            keyClick(Qt.Key_Return);
            compare(dismissed.count, 1);
        }

        function test_otherButtons() {
            keyClick(Qt.Key_Right);
            compare(focusedText(), "Try again");
            keyClick(Qt.Key_Return);
            compare(chosen.signalArguments[0][0], 0);
            keyClick(Qt.Key_Right);
            compare(focusedText(), "Go home");
            keyClick(Qt.Key_Right);
            compare(focusedText(), "Go home");
            keyClick(Qt.Key_Return);
            compare(chosen.signalArguments[1][0], 1);
            keyClick(Qt.Key_Left);
            keyClick(Qt.Key_Left);
            compare(focusedText(), "OK");
        }

        function test_showsTheCode() {
            compare(findChild(screen, "code").text, "Error code: HEARTH-APP-001");
            verify(findChild(screen, "what").text.endsWith("couldn't start."));
        }

        // At the largest text size too, nothing is cut off or off the screen.
        function test_everythingFits() {
            for (const name of ["what", "next", "code"]) {
                const label = findChild(largeScreen, name);
                verify(!label.truncated, name);
                const place = label.mapToItem(root, 0, 0);
                verify(place.y >= 0 && place.y + label.height <= root.height, name);
            }
        }

        function test_dialogAnswers() {
            dialog.visible = true;
            dialog.show();
            compare(focusedText(), "First");
            keyClick(Qt.Key_Up);
            compare(focusedText(), "First");
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Down);
            keyClick(Qt.Key_Down);
            compare(focusedText(), "Third");
            keyClick(Qt.Key_Return);
            compare(answered.count, 1);
            compare(answered.signalArguments[0][0], 2);
        }

        function test_dialogKeepsClicksToItself() {
            const ok = root.Window.activeFocusItem;
            const place = ok.mapToItem(root, ok.width / 2, ok.height / 2);
            dialog.visible = true;
            mouseClick(root, place.x, place.y);
            compare(dismissed.count, 0);
            compare(chosen.count, 0);
        }

        name: "Failure"
        when: windowShown
    }
}

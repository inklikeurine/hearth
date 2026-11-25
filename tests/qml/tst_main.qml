// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTest
import Hearth.Shell

TestCase {
    id: testCase

    function test_windowOpens() {
        const window = createTemporaryObject(mainWindow, testCase);
        verify(window !== null);
        compare(window.visible, true);
    }

    name: "Main"

    Component {
        id: mainWindow

        Main {}
    }
}

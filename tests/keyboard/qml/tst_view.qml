// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtTest
import Hearth.Controls
import Hearth.Keyboard
import Hearth.KeyboardTest
import Hearth.Theme

Item {
    id: root

    // Linux's numbers for some keys.
    readonly property int codeDown: 108
    readonly property int codeEnter: 28
    readonly property int codeLeft: 105
    readonly property int codeRight: 106
    readonly property list<string> layouts: ["qwerty", "alphabet", "row", "wheel"]
    // text-input v3's content purposes.
    readonly property int purposeEmail: 6
    readonly property int purposeNormal: 0
    readonly property int purposePassword: 8
    readonly property int purposePhone: 4

    height: 1080
    width: 1920

    KeyboardView {
        id: view

        anchors.bottom: parent.bottom
        height: implicitHeight
        keys: Harness.keyboard.keys
        width: parent.width
    }

    Component {
        id: keyboardWindow

        KeyboardWindow {
            keyboard: Harness.keyboard
        }
    }

    TestCase {
        function allItems(item) {
            return Array.from(item.children).reduce((all, child) => all.concat(allItems(child)), [item]);
        }

        function cap(text) {
            return caps().find(key => key.text === text);
        }

        function caps() {
            return allItems(view).filter(item => item instanceof KeyCap);
        }

        function current() {
            const found = caps().filter(key => key.current);
            compare(found.length, 1);
            return found[0];
        }

        function init() {
            // The window's own test sets it to this machine's.
            view.Theme.tokens.screenHeight = 1080;
            view.Theme.tokens.textScale = 1.0;
            open("qwerty", root.purposeNormal);
        }

        // A text box takes focus.
        function open(layout, purpose) {
            Harness.useLayout(layout);
            tryVerify(() => Harness.usesLayout(layout));
            Harness.leaveBox();
            tryVerify(() => !Harness.keyboard.shown);
            Harness.focusBox(purpose);
            tryVerify(() => Harness.keyboard.shown);
            Harness.forget();
        }

        function placeOf(item) {
            return item.mapToItem(view, 0, 0, item.width, item.height);
        }

        function ringOf(key) {
            return allItems(key).find(item => item instanceof FocusRing);
        }

        function test_arrowsMoveTheHighlight() {
            compare(current().text, "q");
            Harness.tap(root.codeRight);
            compare(current().text, "w");
            Harness.tap(root.codeDown);
            compare(current().text, "s");
            Harness.tap(root.codeEnter);
            compare(Harness.asked, ["type s"]);
        }

        function test_clickPressesAndTakesTheHighlight() {
            mouseClick(cap("t"));
            compare(Harness.asked, ["type t"]);
            compare(current().text, "t");
            // A page of keys changes under the pointer.
            mouseClick(cap("?123"));
            verify(cap("{"));
            mouseClick(cap("ABC"));
            verify(cap("a"));
        }

        function test_everyKeyFitsAndIsReadable_data() {
            const rows = [];
            for (const layout of root.layouts) {
                for (const purpose of [root.purposeNormal, root.purposeEmail, root.purposePhone]) {
                    for (const symbols of [false, true]) {
                        rows.push({
                            tag: layout + " " + purpose + (symbols ? " symbols" : ""),
                            layout: layout,
                            purpose: purpose,
                            symbols: symbols
                        });
                    }
                }
            }
            return rows;
        }

        function test_everyKeyFitsAndIsReadable(data) {
            open(data.layout, data.purpose);
            if (data.symbols) {
                const abc = cap("ABC");
                if (abc) {
                    mouseClick(abc);
                }
                // In a row it can be off the screen, where a click can't go.
                view.keys.pressKey(caps().findIndex(key => key.text === "?123"));
                verify(cap("{"));
            }
            for (const scale of [1.0, 1.5]) {
                view.Theme.tokens.textScale = scale;
                const sizes = view.Theme.tokens.sizes;
                verify(view.implicitHeight <= root.height * 0.4 * scale, view.implicitHeight);
                for (const key of caps()) {
                    const place = placeOf(key);
                    verify(place.height >= 2 * sizes.textSmall, key.text);
                    verify(place.y + place.height <= view.height - sizes.safeVertical + 1, key.text);
                    if (data.layout !== "row") {
                        verify(place.x >= sizes.safeSide - 1, key.text);
                        verify(place.x + place.width <= view.width - sizes.safeSide + 1, key.text);
                    }
                    const label = key.contentItem;
                    verify(label.font.pixelSize >= 24);
                    // The wheel is a stand-in: its words get cut at the largest text size.
                    if (data.layout !== "wheel" || scale === 1.0) {
                        verify(!label.truncated, key.text);
                    }
                }
            }
        }

        function test_everyKeyHasANameAndRole_data() {
            return root.layouts.map(layout => ({
                        tag: layout,
                        layout: layout
                    }));
        }

        function test_everyKeyHasANameAndRole(data) {
            open(data.layout, root.purposeNormal);
            verify(caps().length > 30);
            for (const key of caps()) {
                compare(key.Accessible.role, Accessible.Button);
                verify(key.Accessible.name !== "");
            }
            verify(cap("Shift").Accessible.checkable);
            verify(!cap("a").Accessible.checkable);
        }

        function test_focusNeverFollowsThePointer() {
            mouseMove(cap("t"), 2, 2);
            mouseMove(cap("t"), 20, 20);
            compare(current().text, "q");
        }

        // Never by colour alone: the ring is only around the highlighted key.
        function test_highlightIsARingToo() {
            for (const key of caps()) {
                compare(ringOf(key).visible, key.current);
            }
            Harness.tap(root.codeRight);
            for (const key of caps()) {
                compare(ringOf(key).visible, key.current);
            }
            verify(current().z > cap("e").z);
        }

        function test_keysNeverTakeTheFocus() {
            mouseClick(cap("t"));
            for (const key of caps()) {
                verify(!key.activeFocus);
                verify(!key.activeFocusOnTab);
            }
        }

        function test_passwordBoxSaysSo() {
            const window = createTemporaryObject(keyboardWindow, root);
            const notes = () => allItems(window.contentItem).filter(item => item.text && item.text.startsWith("Password:") && item.visible);
            compare(notes().length, 0);
            const height = window.height;
            open("qwerty", root.purposePassword);
            tryVerify(() => notes().length === 1);
            tryVerify(() => window.height > height);
            open("qwerty", root.purposeNormal);
            tryVerify(() => notes().length === 0);
            tryCompare(window, "height", height);
        }

        function test_rowKeepsTheHighlightOnScreen() {
            open("row", root.purposeNormal);
            view.Theme.tokens.reducedMotion = true;
            const side = view.Theme.tokens.sizes.safeSide;
            for (let i = 0; i < view.keys.rowCount(); ++i) {
                const place = placeOf(current());
                verify(place.x >= side - 1, current().text);
                verify(place.x + place.width <= view.width - side + 1, current().text);
                Harness.tap(root.codeRight);
            }
            view.Theme.tokens.reducedMotion = false;
        }

        function test_shiftShowsInTheLetters() {
            mouseClick(cap("Shift"));
            verify(cap("Q"));
            verify(!cap("q"));
            verify(cap("Shift").Accessible.checked);
            mouseClick(cap("Shift"));
            verify(cap("Caps").Accessible.checked);
            mouseClick(cap("Caps"));
            verify(cap("q"));
            verify(!cap("Shift").Accessible.checked);
        }

        function test_themes() {
            const tokens = view.Theme.tokens;
            const fill = key => (key.background as Rectangle).color;
            for (const theme of ["dark", "high-contrast"]) {
                verify(tokens.loadBuiltin(theme));
                const other = cap("e");
                verify(Qt.colorEqual(fill(current()), tokens.colors.accent));
                verify(Qt.colorEqual(current().contentItem.color, tokens.colors.textOnAccent));
                verify(Qt.colorEqual(fill(other), tokens.colors.surface));
                verify(Qt.colorEqual(other.contentItem.color, tokens.colors.text));
                compare((other.background as Rectangle).border.width, tokens.sizes.outline);
                // The ring stays clear of the next key.
                verify(-ringOf(current()).anchors.margins <= view.spacing);
            }
            compare(tokens.sizes.outline, 3);
            verify(tokens.loadBuiltin("dark"));
        }

        function test_windowLooksAsTheShellSays() {
            const window = createTemporaryObject(keyboardWindow, root);
            const tokens = window.Theme.tokens;
            Harness.useLook("high-contrast", 150, true);
            tryCompare(tokens, "textScale", 1.5);
            verify(tokens.reducedMotion);
            compare(tokens.colors.background, "#000000");
            Harness.useLook("no such theme", 100, false);
            tryCompare(tokens, "textScale", 1.0);
            verify(!tokens.reducedMotion);
            compare(tokens.colors.background, "#101217");
        }

        function test_windowFollowsTheKeyboard() {
            const window = createTemporaryObject(keyboardWindow, root);
            verify(window.visible);
            compare(window.width, Screen.width);
            Harness.leaveBox();
            tryCompare(window, "visible", false);
            Harness.focusBox(root.purposeNormal);
            tryCompare(window, "visible", true);
            // Slid in, or faded in with reduced motion.
            const strip = allItems(window.contentItem).find(item => item instanceof KeyboardView);
            tryCompare(strip, "y", 0);
            tryCompare(strip, "opacity", 1);
        }

        name: "KeyboardView"
        when: windowShown
    }
}

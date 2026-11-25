// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// Controls one above another, moved through with up and down. Scrolls to keep
// the focused one in view.
FocusScope {
    id: column

    default property alias content: inner.data
    // Room for the ring around the controls.
    readonly property int inset: Theme.tokens.sizes.focusRing * 2
    property alias spacing: inner.spacing
    property Item current

    function stops(): var {
        return Array.from(inner.children).filter(child => child.visible && child.enabled && child.activeFocusOnTab);
    }

    // Focuses the first control.
    function start() {
        const all = column.stops();
        if (all.length > 0) {
            all[0].forceActiveFocus();
        }
    }

    function go(by: int): bool {
        const all = column.stops();
        const next = all[all.findIndex(child => child.activeFocus) + by];
        if (!next) {
            return false;
        }
        next.forceActiveFocus();
        return true;
    }

    function keepInView(item: Item) {
        const top = item.y;
        const bottom = top + item.height + 2 * column.inset;
        const all = column.stops();
        if (item === all[0]) {
            // Whatever is above the first one, such as a heading, comes along.
            view.contentY = 0;
        } else if (top < view.contentY) {
            view.contentY = top;
        } else if (bottom > view.contentY + view.height) {
            view.contentY = Math.max(0, bottom - view.height);
        }
    }

    implicitHeight: inner.height + 2 * inset
    implicitWidth: inner.implicitWidth + 2 * inset

    Keys.onDownPressed: event => event.accepted = column.go(1)
    Keys.onUpPressed: event => event.accepted = column.go(-1)
    onActiveFocusChanged: {
        // Focused as a whole: go where it was, or to the first.
        if (activeFocus && !column.stops().some(child => child.activeFocus)) {
            if (column.current && column.stops().includes(column.current)) {
                column.current.forceActiveFocus();
            } else {
                column.start();
            }
        }
    }

    Connections {
        function onActiveFocusItemChanged() {
            const focused = column.stops().find(child => child.activeFocus);
            if (focused) {
                column.current = focused;
                column.keepInView(focused);
            }
        }

        target: column.Window.window
    }

    Flickable {
        id: view

        anchors.fill: parent
        clip: true
        contentHeight: inner.height + 2 * column.inset
        contentWidth: width
        interactive: false

        Behavior on contentY {
            enabled: !column.Theme.tokens.reducedMotion

            NumberAnimation {
                duration: column.Theme.tokens.motion.duration
            }
        }

        Column {
            id: inner

            spacing: column.Theme.tokens.sizes.gap
            width: view.width - 2 * column.inset
            x: column.inset
            y: column.inset
        }
    }
}

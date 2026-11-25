// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.Session

// Buttons that mean Back, Home and Menu besides the usual ones.
SettingsPage {
    id: page

    objectName: "remotes"

    readonly property var names: ({
            back: KI18n.i18n("Back"),
            home: KI18n.i18n("Home"),
            menu: KI18n.i18n("Menu")
        })
    // What the next button pressed will mean.
    property string waitingFor
    property string message

    function keysFor(action: string): var {
        return Array.from(page.settings.extraKeys[action] ?? []);
    }

    function use(action: string, key: string) {
        const keys = {};
        for (const name of ["back", "home", "menu"]) {
            // A button means one thing.
            keys[name] = page.keysFor(name).filter(other => other !== key);
        }
        keys[action].push(key);
        page.settings.extraKeys = keys;
    }

    function goBack(): bool {
        if (!catcher.catching) {
            return false;
        }
        catcher.catching = false;
        page.message = "";
        return true;
    }

    KeyCatcher {
        id: catcher

        onCaught: key => {
            page.use(page.waitingFor, key);
            page.message = KI18n.i18n("Done. That button now means %1.", page.names[page.waitingFor]);
        }
        onRefused: page.message = KI18n.i18n("That button is needed for typing or moving around. Try another.")
    }

    Label {
        elide: Text.ElideNone
        objectName: "message"
        text: catcher.catching ? KI18n.i18n("Press the button you want for %1 now. Back cancels.", page.names[page.waitingFor]) : page.message
        visible: text.length > 0
        width: parent.width
        wrapMode: Text.Wrap
    }

    Repeater {
        model: ["back", "home", "menu"]

        ListRow {
            id: row

            required property string modelData

            detail: {
                const keys = page.keysFor(row.modelData);
                return keys.length > 0 ? KI18n.i18n("Also: %1", keys.join(", ")) : KI18n.i18n("Only its usual button");
            }
            objectName: row.modelData
            text: KI18n.i18n("Choose a button for %1", page.names[row.modelData])
            width: parent?.width ?? 0

            onClicked: {
                page.waitingFor = row.modelData;
                catcher.catching = true;
            }
        }
    }

    ListRow {
        detail: KI18n.i18n("The usual buttons always work")
        objectName: "reset"
        text: KI18n.i18n("Forget the buttons added here")
        width: parent.width

        onClicked: {
            page.settings.extraKeys = {};
            page.message = KI18n.i18n("Only the usual buttons are used now.");
        }
    }
}

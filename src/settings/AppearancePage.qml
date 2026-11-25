// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import org.kde.ki18n
import Hearth.Controls
import Hearth.Theme

// The theme shows in a preview first, and is used once it's chosen.
SettingsPage {
    id: page

    readonly property var pictures: [""].concat(page.settings.backgroundChoices())
    readonly property list<string> themes: page.Theme.tokens.builtinThemes()
    property string tried: page.settings.theme

    function themeName(theme: string): string {
        switch (theme) {
        case "dark":
            return KI18n.i18n("Dark");
        case "high-contrast":
            return KI18n.i18n("High contrast");
        default:
            return theme;
        }
    }

    Choice {
        current: Math.max(0, page.themes.indexOf(page.tried))
        detail: page.tried === page.settings.theme ? KI18n.i18n("In use") : KI18n.i18n("Press Select on “Use this theme” to use it")
        objectName: "theme"
        options: page.themes.map(theme => page.themeName(theme))
        text: KI18n.i18n("Theme")
        width: parent.width

        onChosen: index => page.tried = page.themes[index]
    }

    Rectangle {
        id: preview

        objectName: "preview"
        Accessible.name: KI18n.i18n("Preview")
        Accessible.role: Accessible.Graphic
        Theme.tokens: previewTokens
        border.color: preview.Theme.tokens.colors.textDim
        border.width: 2
        color: preview.Theme.tokens.colors.background
        height: previewRow.height + 2 * preview.Theme.tokens.sizes.gap
        radius: preview.Theme.tokens.sizes.radius
        width: parent.width

        ThemeTokens {
            id: previewTokens

            screenHeight: page.Theme.tokens.screenHeight
            textScale: page.settings.textPercent / 100

            Component.onCompleted: loadBuiltin(page.tried)
        }

        Connections {
            function onTriedChanged() {
                previewTokens.loadBuiltin(page.tried);
            }

            target: page
        }

        Row {
            id: previewRow

            anchors.centerIn: parent
            spacing: preview.Theme.tokens.sizes.gap * 2

            Tile {
                activeFocusOnTab: false
                enabled: false
                text: KI18n.i18n("An app")
                width: implicitWidth * 0.8
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: preview.Theme.tokens.sizes.gap

                Label {
                    text: KI18n.i18n("Words look like this")
                }

                Label {
                    color: preview.Theme.tokens.colors.textDim
                    font.pixelSize: preview.Theme.tokens.sizes.textSmall
                    text: KI18n.i18n("and smaller ones like this")
                }

                Button {
                    activeFocusOnTab: false
                    enabled: false
                    primary: true
                    text: KI18n.i18n("A button")
                }
            }
        }
    }

    ListRow {
        enabled: page.tried !== page.settings.theme
        objectName: "use"
        opacity: enabled ? 1 : 0.5
        text: KI18n.i18n("Use this theme")
        width: parent.width

        onClicked: {
            page.settings.theme = page.tried;
            // It can't be chosen twice, so the focus goes back to the choice.
            page.start();
        }
    }

    Choice {
        current: Math.max(0, page.pictures.indexOf(page.settings.background.toString().replace("file://", "")))
        objectName: "background"
        options: page.pictures.map(picture => picture.length > 0 ? picture.split("/").pop() : KI18n.i18n("None"))
        text: KI18n.i18n("Background picture")
        width: parent.width

        onChosen: index => page.settings.background = page.pictures[index].length > 0 ? "file://" + page.pictures[index] : ""
    }

    Slider {
        from: 3
        objectName: "columns"
        text: KI18n.i18n("Apps in a row")
        to: 7
        value: page.settings.columns
        width: parent.width

        onMoved: value => page.settings.columns = value
    }

    Slider {
        from: 1
        objectName: "rows"
        text: KI18n.i18n("Rows that show")
        to: 2
        value: page.settings.rows
        width: parent.width

        onMoved: value => page.settings.rows = value
    }

    Choice {
        readonly property list<int> minutes: [0, 1, 2, 5, 10, 30]

        current: Math.max(0, minutes.indexOf(page.settings.screensaverMinutes))
        detail: KI18n.i18n("On Hearth's own screens, when nobody presses anything")
        objectName: "screensaver"
        options: minutes.map(count => count === 0 ? KI18n.i18n("Off") : KI18n.i18np("After 1 minute", "After %1 minutes", count))
        text: KI18n.i18n("Screensaver")
        width: parent.width

        onChosen: index => page.settings.screensaverMinutes = minutes[index]
    }
}

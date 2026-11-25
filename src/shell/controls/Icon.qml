// SPDX-License-Identifier: GPL-3.0-or-later
pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Shapes
import Hearth.Theme

// A line drawing in the theme's colours, on a 24 by 24 grid.
Item {
    id: icon

    property color color: Theme.tokens.colors.text
    required property string name
    readonly property var paths: ({
            "bell": "M 12 3 C 8 3 6 6 6 9 L 6 14 L 4 17 L 20 17 L 18 14 L 18 9 C 18 6 16 3 12 3 M 10 20 L 14 20",
            "close": "M 6 6 L 18 18 M 18 6 L 6 18",
            "home": "M 4 11 L 12 4 L 20 11 L 20 20 L 4 20 Z",
            "left": "M 15 5 L 8 12 L 15 19",
            "move": "M 12 3 L 12 21 M 3 12 L 21 12 M 9 6 L 12 3 L 15 6 M 9 18 L 12 21 L 15 18 M 6 9 L 3 12 L 6 15 M 18 9 L 21 12 L 18 15",
            "muted": "M 4 9 L 8 9 L 13 5 L 13 19 L 8 15 L 4 15 Z M 16 9 L 22 15 M 22 9 L 16 15",
            "none": "M 3 9 C 8 4 16 4 21 9 M 7 13 C 10 10 14 10 17 13 M 12 18 L 12 18.5 M 4 4 L 20 20",
            "pause": "M 8 5 L 8 19 M 16 5 L 16 19",
            "play": "M 7 5 L 19 12 L 7 19 Z",
            "power": "M 12 3 L 12 11 M 7 6 C 2 10 4 20 12 20 C 20 20 22 10 17 6",
            "profile": "M 12 4 C 17 4 17 12 12 12 C 7 12 7 4 12 4 M 4 21 C 4 13 20 13 20 21",
            "right": "M 9 5 L 16 12 L 9 19",
            "settings": "M 4 7 L 20 7 M 4 17 L 20 17 M 9 4 L 9 10 M 15 14 L 15 20",
            "volume": "M 4 9 L 8 9 L 13 5 L 13 19 L 8 15 L 4 15 Z M 16 9 C 18 11 18 13 16 15 M 18.5 6.5 C 22 10 22 14 18.5 17.5",
            "wifi": "M 3 9 C 8 4 16 4 21 9 M 7 13 C 10 10 14 10 17 13 M 12 18 L 12 18.5",
            "wired": "M 4 5 L 20 5 L 20 15 L 4 15 Z M 12 15 L 12 19 M 8 19 L 16 19"
        })

    Accessible.ignored: true
    implicitHeight: Theme.tokens.sizes.textBody
    implicitWidth: implicitHeight

    Shape {
        height: 24
        scale: icon.height / 24
        transformOrigin: Item.TopLeft
        width: 24

        ShapePath {
            capStyle: ShapePath.RoundCap
            fillColor: "transparent"
            joinStyle: ShapePath.RoundJoin
            strokeColor: icon.color
            strokeWidth: 2

            PathSvg {
                path: icon.paths[icon.name] ?? ""
            }
        }
    }
}

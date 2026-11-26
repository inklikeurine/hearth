// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import QtQuick.Effects
import Hearth.Controls
import Hearth.Theme

// What a panel is drawn on: frosted glass where something can blur what's behind
// it, a solid tint where nothing can. Always solid with "Reduce transparency", and
// with a theme whose glass is solid (high contrast).
//  - With "source": Qt blurs that item, drawn earlier in the same window. That's
//    the menu over Hearth's own screens. Not with Qt's software renderer (no
//    graphics driver), which can't.
//  - Without: the compositor is asked to blur the apps behind the window. labwc
//    can't, so there it's the tint.
Item {
    id: background

    property color color: Theme.tokens.colors.background
    readonly property bool frosted: background.glassy && (background.source !== null && background.qtBlurs || compositor.available)
    readonly property bool glassy: background.Theme.tokens.glass.opacity < 1
    // Qt's own software renderer can't blur: what's behind would show through sharp.
    readonly property bool qtBlurs: background.GraphicsInfo.api !== GraphicsInfo.Software
    // An item in the same window, under this one, to blur.
    property Item source: null

    CompositorBlur {
        id: compositor

        anchors.fill: parent
        wanted: background.glassy && background.source === null
    }

    ShaderEffectSource {
        id: behind

        height: background.height
        sourceItem: background.frosted ? background.source : null
        // The part of the source behind this panel. The panel's own place and its
        // parent's say when to look again, as it slides in.
        // Hidden: the blur draws it.
        visible: false
        sourceRect: {
            const moved = [background.x, background.y, background.width, background.height, background.parent?.x, background.parent?.y];
            return background.source && moved ? background.mapToItem(background.source, 0, 0, background.width, background.height) : Qt.rect(0, 0, 0, 0);
        }
        width: background.width
        wrapMode: ShaderEffectSource.ClampToEdge
    }

    MultiEffect {
        anchors.fill: parent
        autoPaddingEnabled: false
        blur: 1
        blurEnabled: true
        blurMax: Math.max(2, background.Theme.tokens.glass.blur)
        source: behind
        visible: behind.sourceItem !== null
    }

    Rectangle {
        anchors.fill: parent
        color: background.frosted ? Qt.alpha(background.color, background.Theme.tokens.glass.opacity) : background.color
    }
}

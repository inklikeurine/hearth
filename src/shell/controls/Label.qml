// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Theme

// Text in the theme's font. Give it a width: text that doesn't fit ends in "…".
Text {
    color: Theme.tokens.colors.text
    elide: Text.ElideRight
    font.family: Theme.tokens.fontFamily
    font.pixelSize: Theme.tokens.sizes.textBody
    textFormat: Text.PlainText
}

# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hearth-controls STATIC)
qt_add_qml_module(hearth-controls
    URI Hearth.Controls
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Controls
    QML_FILES
        Button.qml
        FocusRing.qml
        Label.qml
        Tile.qml
        TileGrid.qml
        TvButton.qml
    DEPENDENCIES
        TARGET hearth-theme
)
target_link_libraries(hearth-controls PUBLIC
    hearth-theme
    hearth-themeplugin
    Qt6::Quick
)

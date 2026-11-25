# SPDX-License-Identifier: GPL-3.0-or-later

find_package(LayerShellQt 6.8 REQUIRED)

qt_add_library(hearth-overlays STATIC)
qt_add_qml_module(hearth-overlays
    URI Hearth.Overlays
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Overlays
    QML_FILES
        Overlay.qml
    SOURCES
        layersurface.cpp
        layersurface.h
    DEPENDENCIES
        TARGET hearth-controls
        TARGET hearth-theme
)
target_include_directories(hearth-overlays PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-overlays
    PUBLIC
        hearth-controls
        hearth-controlsplugin
        hearth-theme
        hearth-themeplugin
        Qt6::Quick
    PRIVATE LayerShellQt::Interface
)

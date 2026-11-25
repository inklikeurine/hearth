# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hearth-theme STATIC)
qt_add_qml_module(hearth-theme
    URI Hearth.Theme
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Theme
    SOURCES
        theme.cpp
        theme.h
        themetokens.cpp
        themetokens.h
)
qt_add_resources(hearth-theme "themes"
    PREFIX "/hearth"
    FILES
        themes/dark.json
        themes/high-contrast.json
)
target_include_directories(hearth-theme PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-theme PUBLIC
    Qt6::Quick
    Qt6::QuickControls2
)

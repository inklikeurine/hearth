# SPDX-License-Identifier: GPL-3.0-or-later

# A window showing every control in a theme. For developers; never shipped.
qt_add_executable(hearth-gallery main.cpp)
qt_add_qml_module(hearth-gallery
    URI Hearth.Gallery
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Gallery
    QML_FILES
        Gallery.qml
    DEPENDENCIES
        TARGET hearth-controls
)
target_link_libraries(hearth-gallery PRIVATE
    hearth-controls
    hearth-controlsplugin
    Qt6::Quick
)

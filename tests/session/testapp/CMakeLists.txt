# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(hearth-testapp main.cpp)
qt_add_qml_module(hearth-testapp
    URI Hearth.TestApp
    QML_FILES TestApp.qml
    DEPENDENCIES
        TARGET hearth-controls
)
target_link_libraries(hearth-testapp PRIVATE
    hearth-controls
    hearth-controlsplugin
    Qt6::Quick
)

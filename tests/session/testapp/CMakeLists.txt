# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(hearth-testapp main.cpp)
qt_add_qml_module(hearth-testapp
    URI Hearth.TestApp
    QML_FILES TestApp.qml
)
target_link_libraries(hearth-testapp PRIVATE Qt6::Quick)

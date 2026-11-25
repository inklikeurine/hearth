# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hearth-settings STATIC)
qt_add_qml_module(hearth-settings
    URI Hearth.Settings
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Settings
    QML_FILES
        AccessibilityPage.qml
        AppearancePage.qml
        RemotesPage.qml
        SettingsPage.qml
        SettingsScreen.qml
    DEPENDENCIES
        TARGET hearth-apps
        TARGET hearth-controls
        TARGET hearth-session-lib
        TARGET hearth-theme
)
target_link_libraries(hearth-settings PUBLIC
    hearth-apps
    hearth-appsplugin
    hearth-controls
    hearth-controlsplugin
    hearth-session-lib
    hearth-session-libplugin
    hearth-theme
    hearth-themeplugin
    Qt6::Quick
)

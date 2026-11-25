# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hearth-panels STATIC)
qt_add_qml_module(hearth-panels
    URI Hearth.Panels
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Panels
    QML_FILES
        BurnInGuard.qml
        Clock.qml
        NotificationList.qml
        NotificationPopup.qml
        Screensaver.qml
        SidePanel.qml
        StatusBar.qml
        SystemMenu.qml
        VolumeBar.qml
    DEPENDENCIES
        TARGET hearth-controls
        TARGET hearth-system
        TARGET hearth-theme
)
target_link_libraries(hearth-panels PUBLIC
    hearth-controls
    hearth-controlsplugin
    hearth-system
    hearth-systemplugin
    hearth-theme
    hearth-themeplugin
    Qt6::Quick
)

# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_library(hearth-apps STATIC)
qt_add_qml_module(hearth-apps
    URI Hearth.Apps
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Apps
    SOURCES
        applauncher.cpp
        applauncher.h
        applist.cpp
        applist.h
        desktopentry.cpp
        desktopentry.h
        iconprovider.cpp
        iconprovider.h
        shellsettings.cpp
        shellsettings.h
)
target_include_directories(hearth-apps PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-apps PUBLIC Qt6::Quick)

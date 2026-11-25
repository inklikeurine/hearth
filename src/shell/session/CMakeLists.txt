# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt6 6.11 REQUIRED COMPONENTS Network WaylandClient)

set(foreign_toplevel
    /usr/share/wlr-protocols/unstable/wlr-foreign-toplevel-management-unstable-v1.xml)

# The Wayland code is a library of its own. Fedora 45's qt6-qtwayland-devel lacks
# a file that a QML module linking Qt6::WaylandClient itself asks for; linked
# privately through this one, it isn't asked for.
qt_add_library(hearth-wayland STATIC
    toplevels.cpp
    toplevels.h
    waylandwindows.cpp
    waylandwindows.h
    windows.h
)
qt_generate_wayland_protocol_client_sources(hearth-wayland FILES ${foreign_toplevel})
target_include_directories(hearth-wayland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-wayland
    PUBLIC Qt6::Quick
    PRIVATE Qt6::WaylandClient
)

qt_add_library(hearth-session-lib STATIC)
qt_add_qml_module(hearth-session-lib
    URI Hearth.Session
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Session
    SOURCES
        appswitcher.cpp
        appswitcher.h
        commandserver.cpp
        commandserver.h
        keyrouter.cpp
        keyrouter.h
        sessionwindows.h
    DEPENDENCIES
        TARGET hearth-apps
)
target_include_directories(hearth-session-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-session-lib PUBLIC
    hearth-apps
    hearth-appsplugin
    hearth-wayland
    Qt6::Network
    Qt6::Quick
)

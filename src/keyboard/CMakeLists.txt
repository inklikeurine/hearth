# SPDX-License-Identifier: GPL-3.0-or-later

find_package(LayerShellQt 6.8 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Qt6 6.11 REQUIRED COMPONENTS Network WaylandClient)
pkg_check_modules(xkbcommon REQUIRED IMPORTED_TARGET xkbcommon)

# The two protocols' definitions, which no Fedora 45 package here has. Copied
# unchanged from Stevia, Phosh's on-screen keyboard: tag v0.57.0 (commit
# d45a7e0d156d885721dfba6ef390be77e165c766), folder protocol/,
# https://gitlab.gnome.org/World/Phosh/stevia. Their licence is the MIT-style
# notice inside each file. The checksums are the ones the lead approved.
function(borrowed_protocol file sha256)
    file(SHA256 ${CMAKE_CURRENT_SOURCE_DIR}/protocol/${file} actual)
    if(NOT actual STREQUAL sha256)
        message(FATAL_ERROR "protocol/${file} doesn't match its approved checksum")
    endif()
endfunction()
borrowed_protocol(input-method-unstable-v2.xml
    33ac1a0325dcfefd566773928917b0251f54078d0f530852cbfe82aa0270a0d9)
borrowed_protocol(virtual-keyboard-unstable-v1.xml
    8c0dbb0ca3697ddb0dec70829da425b6a7b29a020a7a70d3d292e50d76e0c26b)

# The Wayland code is a library of its own for the reason src/shell/session has.
qt_add_library(hearth-keyboard-wayland STATIC
    compositorlink.h
    keymap.cpp
    keymap.h
    waylandlink.cpp
    waylandlink.h
)
qt_generate_wayland_protocol_client_sources(hearth-keyboard-wayland FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/input-method-unstable-v2.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol/virtual-keyboard-unstable-v1.xml
)
target_include_directories(hearth-keyboard-wayland PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-keyboard-wayland
    PUBLIC Qt6::Quick
    PRIVATE LayerShellQt::Interface PkgConfig::xkbcommon Qt6::WaylandClient
)

qt_add_library(hearth-keyboard-lib STATIC)
qt_add_qml_module(hearth-keyboard-lib
    URI Hearth.Keyboard
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Keyboard
    QML_FILES
        KeyCap.qml
        KeyboardView.qml
        KeyboardWindow.qml
    DEPENDENCIES
        TARGET hearth-controls
        TARGET hearth-theme
    SOURCES
        fieldkind.cpp
        fieldkind.h
        keyboard.cpp
        keyboard.h
        keyboardmodel.cpp
        keyboardmodel.h
        keyboardsettings.cpp
        keyboardsettings.h
        keyrules.cpp
        keyrules.h
        layout.cpp
        layout.h
        shelllink.cpp
        shelllink.h
        textbox.cpp
        textbox.h
)
target_include_directories(hearth-keyboard-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-keyboard-lib PUBLIC
    hearth-controls
    hearth-controlsplugin
    hearth-keyboard-wayland
    hearth-theme
    hearth-themeplugin
    PkgConfig::xkbcommon
    Qt6::Network
    Qt6::Quick
)

qt_add_executable(hearth-keyboard main.cpp)
target_link_libraries(hearth-keyboard PRIVATE
    hearth-keyboard-lib
    hearth-keyboard-libplugin
    KF6::I18n
    KF6::I18nQml
    Qt6::Quick
)
install(TARGETS hearth-keyboard)

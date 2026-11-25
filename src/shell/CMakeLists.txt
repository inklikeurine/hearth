# SPDX-License-Identifier: GPL-3.0-or-later

# The shell's parts are a library, so the tests can use them without the program.
qt_add_library(hearth-shell-lib STATIC)
qt_add_qml_module(hearth-shell-lib
    URI Hearth.Shell
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Shell
    QML_FILES
        Main.qml
    DEPENDENCIES
        TARGET hearth-theme
    SOURCES
        version.cpp
        version.h
)
target_compile_definitions(hearth-shell-lib PRIVATE HEARTH_VERSION="${PROJECT_VERSION}")
target_include_directories(hearth-shell-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-shell-lib PUBLIC
    hearth-theme
    hearth-themeplugin
    Qt6::Quick
)

qt_add_executable(hearth-shell main.cpp)
target_link_libraries(hearth-shell PRIVATE
    hearth-shell-lib
    hearth-shell-libplugin
    Qt6::Quick
)

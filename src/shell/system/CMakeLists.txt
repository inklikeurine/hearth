# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt6 6.11 REQUIRED COMPONENTS DBus)

qt_add_library(hearth-system STATIC)
qt_add_qml_module(hearth-system
    URI Hearth.System
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/System
    SOURCES
        buses.cpp
        buses.h
        idlewatcher.cpp
        idlewatcher.h
        mediaplayers.cpp
        mediaplayers.h
        networkstate.cpp
        networkstate.h
        notifications.cpp
        notifications.h
        power.cpp
        power.h
        volume.cpp
        volume.h
)
target_compile_definitions(hearth-system PRIVATE HEARTH_VERSION="${PROJECT_VERSION}")
target_include_directories(hearth-system PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-system PUBLIC Qt6::DBus Qt6::Quick)

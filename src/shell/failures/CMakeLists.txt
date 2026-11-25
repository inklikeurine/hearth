# SPDX-License-Identifier: GPL-3.0-or-later

find_package(PkgConfig REQUIRED)
pkg_check_modules(qrencode REQUIRED IMPORTED_TARGET libqrencode)

qt_add_library(hearth-failures STATIC)
qt_add_qml_module(hearth-failures
    URI Hearth.Failures
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/Failures
    QML_FILES
        FailureScreen.qml
    SOURCES
        failures.cpp
        failures.h
        qrcode.cpp
        qrcode.h
    DEPENDENCIES
        TARGET hearth-controls
        TARGET hearth-theme
)
target_compile_definitions(hearth-failures PRIVATE HEARTH_VERSION="${PROJECT_VERSION}")
target_include_directories(hearth-failures PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hearth-failures
    PUBLIC
        hearth-controls
        hearth-controlsplugin
        hearth-theme
        hearth-themeplugin
        Qt6::Quick
    PRIVATE
        KF6::I18n
        PkgConfig::qrencode
)

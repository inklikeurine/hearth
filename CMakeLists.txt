# SPDX-License-Identifier: GPL-3.0-or-later
#
# Hearth. Build inside the dev box (docs/dev-environment.md), or run
# tools/dev/check to build and run every check.

cmake_minimum_required(VERSION 3.30)
project(hearth VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# clang-tidy reads this list of compile commands.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(KF6I18n 6.32 REQUIRED)
find_package(Qt6 6.11 REQUIRED COMPONENTS Quick QuickControls2 QuickTest Test)
qt_standard_project_setup(REQUIRES 6.11)
add_compile_definitions(TRANSLATION_DOMAIN="hearth")

add_compile_options(-Wall -Wextra)

enable_testing()
add_subdirectory(src/shell/apps)
add_subdirectory(src/shell/theme)
add_subdirectory(src/shell/controls)
add_subdirectory(src/shell)
add_subdirectory(tests)
add_subdirectory(tests/gallery)

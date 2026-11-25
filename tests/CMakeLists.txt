# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(tst_version cpp/tst_version.cpp)
target_compile_definitions(tst_version PRIVATE EXPECTED_VERSION="${PROJECT_VERSION}")
target_link_libraries(tst_version PRIVATE hearth-shell-lib Qt6::Test)
add_test(NAME version COMMAND tst_version)

qt_add_executable(tst_theme cpp/tst_theme.cpp)
target_link_libraries(tst_theme PRIVATE hearth-theme Qt6::Test)
add_test(NAME theme COMMAND tst_theme)

qt_add_executable(tst_heldkeys cpp/tst_heldkeys.cpp)
target_link_libraries(tst_heldkeys PRIVATE
    hearth-controls
    hearth-controlsplugin
    Qt6::Quick
    Qt6::Test
)
add_test(NAME heldkeys COMMAND tst_heldkeys)

qt_add_executable(tst_qml qml/main.cpp)
target_compile_definitions(tst_qml PRIVATE QUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml")
target_link_libraries(tst_qml PRIVATE
    hearth-controls
    hearth-controlsplugin
    hearth-shell-lib
    hearth-shell-libplugin
    Qt6::Quick
    Qt6::QuickTest
)
add_test(NAME qml COMMAND tst_qml)

# No screen needed.
set_tests_properties(version theme heldkeys qml PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

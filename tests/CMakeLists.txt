# SPDX-License-Identifier: GPL-3.0-or-later

qt_add_executable(tst_version cpp/tst_version.cpp)
target_compile_definitions(tst_version PRIVATE EXPECTED_VERSION="${PROJECT_VERSION}")
target_link_libraries(tst_version PRIVATE hearth-shell-lib Qt6::Test)
add_test(NAME version COMMAND tst_version)

qt_add_executable(tst_theme cpp/tst_theme.cpp)
target_link_libraries(tst_theme PRIVATE hearth-theme Qt6::Test)
add_test(NAME theme COMMAND tst_theme)

qt_add_executable(tst_apps cpp/tst_apps.cpp)
target_link_libraries(tst_apps PRIVATE hearth-apps Qt6::Test)
add_test(NAME apps COMMAND tst_apps)

qt_add_executable(tst_launcher cpp/tst_launcher.cpp)
target_link_libraries(tst_launcher PRIVATE hearth-apps Qt6::Test)
add_test(NAME launcher COMMAND tst_launcher)

qt_add_executable(tst_switcher cpp/tst_switcher.cpp)
target_link_libraries(tst_switcher PRIVATE hearth-session-lib Qt6::Test)
add_test(NAME switcher COMMAND tst_switcher)

qt_add_executable(tst_keys cpp/tst_keys.cpp)
target_link_libraries(tst_keys PRIVATE hearth-session-lib Qt6::Test)
add_test(NAME keys COMMAND tst_keys)

qt_add_executable(tst_commands cpp/tst_commands.cpp)
target_compile_definitions(tst_commands PRIVATE HEARTH_SEND="$<TARGET_FILE:hearth-send>")
target_link_libraries(tst_commands PRIVATE hearth-session-lib Qt6::Test)
add_dependencies(tst_commands hearth-send)
add_test(NAME commands COMMAND tst_commands)

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
    KF6::I18n
    KF6::I18nQml
    hearth-controls
    hearth-controlsplugin
    hearth-shell-lib
    hearth-shell-libplugin
    Qt6::Quick
    Qt6::QuickTest
)
add_test(NAME qml COMMAND tst_qml)

# No screen needed, and never the developer's own settings or apps.
set(test_environment
    QT_QPA_PLATFORM=offscreen
    XDG_CONFIG_HOME=${CMAKE_CURRENT_BINARY_DIR}/config
    XDG_DATA_HOME=${CMAKE_CURRENT_BINARY_DIR}/data
    XDG_DATA_DIRS=${CMAKE_CURRENT_SOURCE_DIR}/data/share
)
set_tests_properties(version theme apps launcher switcher keys commands heldkeys qml PROPERTIES ENVIRONMENT "${test_environment}")

add_subdirectory(session)

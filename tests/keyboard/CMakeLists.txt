# SPDX-License-Identifier: GPL-3.0-or-later

foreach(name keyboard keyrules layouts settings shelllink textbox)
    qt_add_executable(tst_keyboard_${name} tst_${name}.cpp fakelink.h)
    target_include_directories(tst_keyboard_${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(tst_keyboard_${name} PRIVATE
        hearth-keyboard-lib
        hearth-session-lib
        Qt6::Test
    )
    add_test(NAME keyboard_${name} COMMAND tst_keyboard_${name})
    list(APPEND keyboard_tests keyboard_${name})
endforeach()

qt_add_executable(tst_keyboard_qml qml/main.cpp)
qt_add_qml_module(tst_keyboard_qml
    URI Hearth.KeyboardTest
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/Hearth/KeyboardTest
    SOURCES
        fakelink.h
        qml/harness.h
    DEPENDENCIES
        TARGET hearth-keyboard-lib
)
target_include_directories(tst_keyboard_qml PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/qml
)
target_compile_definitions(tst_keyboard_qml PRIVATE
    QUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/qml"
)
target_link_libraries(tst_keyboard_qml PRIVATE
    KF6::I18n
    KF6::I18nQml
    hearth-keyboard-lib
    hearth-keyboard-libplugin
    Qt6::Quick
    Qt6::QuickTest
)
add_test(NAME keyboard_qml COMMAND tst_keyboard_qml)

set_tests_properties(${keyboard_tests} keyboard_qml PROPERTIES ENVIRONMENT "${test_environment}")

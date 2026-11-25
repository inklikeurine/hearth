# SPDX-License-Identifier: GPL-3.0-or-later

add_subdirectory(testapp)

# The whole session, in a labwc with no screen.
add_test(NAME session
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run.sh ${CMAKE_BINARY_DIR}
)
set_tests_properties(session PROPERTIES RUN_SERIAL ON TIMEOUT 300)

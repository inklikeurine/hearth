# SPDX-License-Identifier: GPL-3.0-or-later

# No Qt: labwc starts it for every key, and a held key repeats 25 times a second.
add_executable(hearth-send send.cpp)
set_target_properties(hearth-send PROPERTIES AUTOMOC OFF)
install(TARGETS hearth-send)

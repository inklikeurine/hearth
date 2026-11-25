# SPDX-License-Identifier: GPL-3.0-or-later
# The system menu and the other overlays, over home and over an app.

key Menu
check "Menu opens the menu over home" state_is menu True
picture 20-menu-home
key XF86Back
check "  Back closes it" state_is menu False
key Menu
key Menu
check "  and so does Menu" state_is menu False

tile 0
key Return
check "an app is open" window app_id:a-plain state:active
before="$(grep -c "key 1000015" "$log")"
key F2
check "Menu opens over it" state_is menu True
picture 20-menu-app
key Down
key Up
check "  the app behind gets none of its keys" test "$(grep -c "key 1000015" "$log")" = "$before"
check "  and stays full screen" window app_id:a-plain state:fullscreen
key Return
check "its first item goes home" state_is home True
check "  and the menu closes" state_is menu False

key Return
check "back to the app" window app_id:a-plain state:active
key Menu
key Down
key Return
check "the menu closes the app" no_window app_id:a-plain
check "  and home comes back" state_is home True

key Menu
key Down
key Return
check "Notifications opens from the menu" state_is notifications True
picture 20-notifications
key Home
check "  Home closes it" state_is notifications False

tile 0
key Up
key Return
check "the bell opens them too" state_is notifications True
key XF86Back
check "  Back closes them" state_is notifications False
key Right
key Return
check "the power button asks first" state_is power True
picture 20-power
key Return
check "  in a test session, Sleep says it can't" state_is failure HEARTH-POWER-001
key XF86Back
check "  Back closes that" state_is failure ""
key Home

# Moving an app.
tile 0
key Menu
key Return
check "Move starts from the menu" state_is moving True
key Right
picture 20-moving
key Return
check "  Select ends it" state_is moving False
order_is() { [[ "$(sed -n 's/^order=//p' "$XDG_CONFIG_HOME/hearth/shell.ini")" == "$1"* ]]; }
check "  the order is kept" order_is "b-fixed, a-plain"
key Menu
key Return
key Left
key Return
check "  and back again" order_is "a-plain, b-fixed"

# The volume: wpctl is a pretend one that says what it was asked.
key XF86AudioRaiseVolume
check "the volume shows" state_is volume True
picture 20-volume
check "  and wpctl was asked for more" grep -q "5%+" "$work/wpctl.log"
check "  it goes away by itself" state_is volume False

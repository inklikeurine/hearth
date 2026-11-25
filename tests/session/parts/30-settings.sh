# SPDX-License-Identifier: GPL-3.0-or-later
# Settings, and what its pages change in the running session.

tile 0
key Return
check "an app is open" window app_id:a-plain state:active
key Menu
key Down
key Down
key Down
key Return
check "Settings opens from the menu, and the app goes to the back" state_is settings True
check "  home is in front" state_is home True
picture 30-settings
key Return
picture 30-appearance
key Right
key Down
key Return
check "a theme can be used" state_is theme high-contrast
picture 30-contrast
key XF86Back
key Down
key Return
key Right
picture 30-accessibility
key XF86Back
key Down
key Return
picture 30-remotes
key Return
key F7
saved() { grep -qx "back=F7" "$XDG_CONFIG_HOME/hearth/shell.ini"; }
check "a button can be given a meaning" saved
bound() { grep -q 'key="F7"' "$XDG_RUNTIME_DIR"/hearth-labwc-*/rc.xml; }
check "  labwc's settings have it" bound
# One Back leaves the page, the next the settings.
key F7
key F7
check "  and it works at once" state_is settings False

key Home
key Return
check "the app is still there" window app_id:a-plain state:active
key Menu
key Down
key Return
check "  and closes" no_window app_id:a-plain

# SPDX-License-Identifier: GPL-3.0-or-later
# The home screen, opening apps and switching between them.

check "the home screen is full screen and in front" window app_id:hearth-shell state:fullscreen state:active
check "  and the shell agrees" state_is home True
picture 10-home

tile 0
key Return
check "Select opens the first app" window app_id:a-plain state:active
check "  full screen" window app_id:a-plain state:fullscreen
check "  the shell knows which" state_is running a-plain
check "  and that home isn't in front" state_is home False
picture 10-app
key Down
check "the app gets the arrow keys" logged "key 1000015"

key XF86HomePage
check "the remote's Home goes home" window app_id:hearth-shell state:active
check "  the app is kept, hidden" window app_id:a-plain state:minimized
key Return
check "Select on its tile brings it back" window app_id:a-plain state:active
check "  without starting it again" logged_times "started" 1
key Home
check "a keyboard's Home goes home too" state_is home True

tile 1
key Return
check "opening another app closes the first" no_window app_id:a-plain
check "  it was asked to close" logged "asked to close"
check "  the other one opens" window app_id:b-fixed state:active
check "  full screen, though its size is fixed" window app_id:b-fixed state:fullscreen

key XF86Back
key Escape
key BackSpace
check "Back, Esc and Backspace never reach an app" test "$(grep -cE '^key 100000[013]$' "$log")" = 0
wtype -P Escape -s 800 -p Escape
check "  not even held" test "$(grep -cE '^key 100000[013]$' "$log")" = 0
key Home

tile 4
key Return
check "an app that starts full screen" window app_id:e-fullscreen state:active
check "  stays full screen" window app_id:e-fullscreen state:fullscreen
key Home

tile 3
key Return
check "an app whose program is missing: a failure screen" state_is failure HEARTH-APP-001
picture 10-failure
key Right
key Return
check "  Try again fails again" state_is failure HEARTH-APP-001
key XF86Back
check "  Back closes it" state_is failure ""
check "  and home is in front" window app_id:hearth-shell state:active
key Return
check "  its tile is still the focused one" state_is failure HEARTH-APP-001
key Home
check "  Home closes it too" state_is failure ""

tile 2
key Return
check "the app with unsaved work opens" window app_id:c-stubborn state:active
key Home
tile 0
key Return
check "asked to close, it shows its question" window "title:Save your work?"
check "after a while the shell asks what to do" state_is question True
check "  its dialog isn't stretched over the screen" no_window "title:Save your work?" state:fullscreen
check "  the next app isn't opened over it" no_window app_id:a-plain
picture 10-question
key XF86Back
check "Back on the question goes back to the app" window app_id:c-stubborn
check "  and the question goes" state_is question False
picture 10-unsaved
key Home
tile 0
key Return
check "asked again, it asks again" state_is question True
key Down
key Return
check "'Close it anyway' ends it" no_window app_id:c-stubborn
check "  and the next app opens" window app_id:a-plain state:active
key Home

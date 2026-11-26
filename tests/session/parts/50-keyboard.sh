# SPDX-License-Identifier: GPL-3.0-or-later
# The on-screen keyboard. It starts on "q", in the QWERTY layout.

tile 0
key Return
check "an app with no text box is in front" window app_id:a-plain state:active
wtype z
check "  its keys go to it" logged "key 5a"
check "  and there's no keyboard" state_is keyboard False
key Home

tile 5
key Return
check "an app with a text box opens" window app_id:f-text state:active
check "  the keyboard appears" state_is keyboard True
check "  the shell knows of the text box" state_is textBox True
check "  the app keeps the focus" window app_id:f-text state:active
picture 50-qwerty

key Return
key Right
key Return
check "arrows and Select type" logged "text qw"
wtype -P Right -s 1500 -p Right
key Return
check "a held arrow repeats, to the end of the row" logged "text qw-"
key Up
key Return
check "the Delete key deletes" logged_times "text qw" 2
key Down
key Down
key Return
check "the Enter key is Enter" logged_times accepted 1
key Left
wtype -P Return -s 900 -p Return
check "a held Select" logged "text qw'"
check "  presses once" logged_times "text qw''" 0

wtype cats
check "a real keyboard's letters go to the text box" logged "text qw'cats"
key BackSpace
check "  its Backspace deletes" logged "text qw'cat"
key Return
check "  its Enter after typing is Enter" logged_times accepted 2
key Left
key Return
check "  after an arrow, it presses the highlighted key" logged "text qw'catl"
wtype -P BackSpace -s 1500 -p BackSpace
check "a held Backspace deletes it all" logged "text "
wtype abé
key BackSpace
key BackSpace
check "Backspace takes whole characters" logged_times "text a" 2
wtype -M ctrl -k a -m ctrl
wtype z
check "shortcuts reach the app" logged "text z"

# To the "t", from wherever it is.
key Left
wlrctl pointer move -4000 -4000
wlrctl pointer move 800 780
key Return
check "the pointer doesn't move the highlight" logged "text zk"
# wlrctl's pointer is plugged in only for a moment: the click can come before the
# keyboard has seen it.
click_t() { wlrctl pointer click; sleep 0.2; logged "text zkt"; }
check "a click on a key presses it" click_t
check "  the app keeps the focus" window app_id:f-text state:active
wlrctl pointer move -4000 -4000

mkdir -p "$XDG_CONFIG_HOME/hearth"
echo "layout=wheel" >"$XDG_CONFIG_HOME/hearth/keyboard.ini"
# The wheel starts on "a". Until it's there, Select presses something else.
selects_a() { wtype -k Return; sleep 0.2; grep -qx "text zkt.*a" "$log"; }
check "another layout at once, when the settings change" selects_a
picture 50-wheel
echo "layout=qwerty" >"$XDG_CONFIG_HOME/hearth/keyboard.ini"

key XF86Back
check "Back closes the keyboard" state_is keyboard False
check "  the text box is still there" state_is textBox True
check "  and the app still in front" window app_id:f-text state:active
wtype o
check "  keys go to the app" grep -qx "text zkt.*o" "$log"
check "  and the keyboard stays closed" state_is keyboard False

key Home
check "going home" state_is home True
check "  there's no text box there" state_is textBox False
key Return
check "back in the app, the keyboard is back" state_is keyboard True
key Menu
check "the menu opens over the keyboard" state_is menu True
check "  which goes while it's open" state_is keyboard False
key XF86Back
check "  and comes back when it closes" state_is keyboard True
key Home
check "Home with the keyboard open goes home" state_is home True
check "  and the keyboard goes" state_is keyboard False

tile 6
key Return
check "an app that starts beside its text boxes" window app_id:g-boxes state:active
wtype y
check "  its keys go to it" logged "key 59"
check "  and there's no keyboard" state_is keyboard False
key Down
check "the keyboard comes with the text box" state_is keyboard True
wtype b
key Return
check "focus moves to the next box" logged_times accepted 3
wtype 7
check "  one of Hearth's own, which the keyboard types into" logged "second 7"
check "  still shown" state_is keyboard True
key Return
check "it goes when focus leaves the boxes" state_is keyboard False
check "  and the shell knows" state_is textBox False
key Down
check "back in a text box, it's back" state_is keyboard True
key Escape
check "Esc closes it" state_is keyboard False
key Up
check "  focus leaves the box" state_is textBox False
key Down
check "  the next box brings the keyboard" state_is keyboard True

second_keyboard() {
    local said
    said="$(timeout 10 hearth-keyboard 2>&1)" && [[ "$said" == *"another keyboard program is running"* ]]
}
check "a second keyboard leaves by itself" second_keyboard
check "  the first stays" state_is keyboard True

for pid in $(pgrep -x hearth-keyboard); do
    grep -qzx "XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR" "/proc/$pid/environ" && kill -KILL "$pid"
done
check "a crashed keyboard takes nothing with it" state_is textBox False
check "  it starts again, for the same box" state_is keyboard True
wtype s
check "  and types" logged "text bs"
check "what was typed isn't in the session's log" test "$(grep -c 'cats' "$work/session.log")" = 0
key Home

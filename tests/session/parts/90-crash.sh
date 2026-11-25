# SPDX-License-Identifier: GPL-3.0-or-later
# The shell crashes. Last, so nothing after it depends on this.

check "until here the shell never crashed" test "$(grep -c 'hearth-shell stopped' "$work/session.log")" = 0

tile 0
key Return
check "an app is open" window app_id:a-plain state:active
starts="$(grep -cx started "$log")"
# This session's shell only. KILL leaves no core dump behind.
for pid in $(pgrep -x hearth-shell); do
    grep -qzx "XDG_RUNTIME_DIR=$XDG_RUNTIME_DIR" "/proc/$pid/environ" && kill -KILL "$pid"
done
check "the shell starts again after a crash" window app_id:hearth-shell
check "  the app is still open" window app_id:a-plain
check "  and the shell knows it" state_is running a-plain
key Home
check "  Home still works" state_is home True
check "  the app wasn't started again" logged_times started "$starts"

#!/usr/bin/env bash
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Tests the whole session with no screen: labwc runs headless, wtype presses keys
# as a remote would, and wlrctl says which windows labwc has. Nothing in it can
# reach the desktop it runs on: it has its own runtime folder, settings, apps and
# an address for the message bus where nothing answers.
#   tests/session/run.sh BUILD_FOLDER [PICTURES_FOLDER]
# With a folder for pictures, saves the screen at each step.

set -uo pipefail

build="$(readlink -f "$1")"
repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
pictures="${2:-}"
[[ -n "$pictures" ]] && mkdir -p "$pictures"

work="$(mktemp -d)"
mkdir -p "$work/runtime" "$work/config" "$work/data/applications" "$work/cache" "$work/state"
chmod 700 "$work/runtime"
export XDG_RUNTIME_DIR="$work/runtime" XDG_CONFIG_HOME="$work/config" XDG_CACHE_HOME="$work/cache"
export XDG_DATA_HOME="$work/data" XDG_DATA_DIRS="$work/data" XDG_STATE_HOME="$work/state"
export DBUS_SESSION_BUS_ADDRESS="unix:path=$work/no-bus" DBUS_SYSTEM_BUS_ADDRESS="unix:path=$work/no-bus"
export WLR_BACKENDS=headless WLR_RENDERER=pixman WLR_LIBINPUT_NO_DEVICES=1
# Nothing here needs a graphics card.
export QT_QUICK_BACKEND=software QT_FORCE_STDERR_LOGGING=1
export PATH="$build/src/send:$build/src/shell:$build/src/keyboard:$PATH"
unset DISPLAY WAYLAND_DISPLAY

log="$work/app.log"
: >"$log"
app() {  # ID, options...: an app for the home screen, named "Test ID"
    local id="$1"
    shift
    printf '[Desktop Entry]\nType=Application\nName=Test %s\nExec=%s %s %s %s\n' \
        "$id" "$build/tests/session/testapp/hearth-testapp" "$id" "$log" "$*" \
        >"$work/data/applications/$id.desktop"
}
# In the order of their tiles.
app a-plain
app b-fixed fixed
app c-stubborn stubborn
printf '[Desktop Entry]\nType=Application\nName=Test d-missing\nExec=/nowhere/at/all\n' \
    >"$work/data/applications/d-missing.desktop"
app e-fullscreen fullscreen
app f-text text

"$repo/src/session/hearth-session" --dev >"$work/session.log" 2>&1 &
session=$!

finish() {
    kill "$session" 2>/dev/null
    wait "$session" 2>/dev/null
    rm -rf "$work"
}
trap finish EXIT

try() {  # SECONDS, command...: again and again until it succeeds
    local end=$((SECONDS + $1))
    shift
    while true; do
        "$@" >/dev/null 2>&1 && return 0
        ((SECONDS >= end)) && return 1
        sleep 0.1
    done
}

passed=0
failed=0
check() {  # description, command...
    local what="$1"
    shift
    if try 10 "$@"; then
        passed=$((passed + 1))
        echo "PASS  $what"
    else
        failed=$((failed + 1))
        echo "FAIL  $what"
        echo "      state: $(hearth-send state 2>&1)"
        picture "fail-$failed"
    fi
}
picture() {
    [[ -n "$pictures" ]] && grim "$pictures/$1.png" 2>/dev/null
    return 0
}

socket() { compgen -G "$XDG_RUNTIME_DIR/hearth-shell-wayland-*.sock"; }
if ! try 20 socket; then
    echo "FAIL  the shell didn't start"
    cat "$work/session.log"
    exit 1
fi
WAYLAND_DISPLAY="$(basename "$(socket)" .sock)"
export WAYLAND_DISPLAY="${WAYLAND_DISPLAY#hearth-shell-}"
wlr-randr --output HEADLESS-1 --custom-mode 1920x1080 >/dev/null 2>&1

state() { hearth-send state | python3 -I -c 'import json, sys; print(json.load(sys.stdin).get(sys.argv[1]))' "$1"; }
state_is() { [[ "$(state "$1")" == "$2" ]]; }
window() { wlrctl toplevel find "$@"; }
no_window() { ! wlrctl toplevel find "$@"; }
logged() { grep -qx "$1" "$log"; }
logged_times() { [[ "$(grep -cx "$1" "$log")" == "$2" ]]; }
key() { wtype -k "$1"; sleep 0.15; }
# Goes to a tile by its place: the first five are in one row.
tile() {
    key XF86Back
    for ((i = 0; i < $1; i++)); do key Right; done
}

for part in "$repo"/tests/session/parts/*.sh; do
    echo "== $(basename "$part" .sh)"
    # shellcheck source=/dev/null
    source "$part"
done

if ((failed)); then
    echo "-- the session's log:"
    tail -n 40 "$work/session.log"
fi
echo "$passed passed, $failed failed"
((failed == 0))

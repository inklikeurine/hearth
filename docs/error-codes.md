# Error codes

Every failure screen shows one of these codes, with a QR code to its help page on the project's wiki (`https://github.com/inklikeurine/hearth/wiki/CODE?version=VERSION`). The address holds the code and Hearth's version, nothing else, and the box sends nothing.

How it works:
- A code is `HEARTH-AREA-NUMBER`, such as `HEARTH-APP-001`. A number is never used again for something else.
- Each part keeps its own codes next to its code. The shell's are in [failures.cpp](../src/shell/failures/failures.cpp). A test fails when a code is missing here, or is here and nowhere else.
- The wiki pages don't exist yet. Each starts as its row here.

| Code | What happened | What the screen says to do | What to check when it keeps happening |
|---|---|---|---|
| HEARTH-APP-001 | The app's program couldn't be started at all | Try it again; if it still doesn't start, remove the app and install it again | Whether the program in the app's `.desktop` file exists |
| HEARTH-APP-002 | The app ended with an error before it showed a window | Try it again; if it keeps happening, restart the box | The app's messages in the session's log |
| HEARTH-APP-003 | The app crashed, or was ended by the system, while it was open | Open it again; if it keeps happening, restart the box | Free memory; the app's messages in the session's log |
| HEARTH-SHELL-000 | A failure whose code this version doesn't know | Try again; if it keeps happening, restart the box | The session's log |

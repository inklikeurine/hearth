# P5 — polkit rules and checks
 
**Sources:** polkit's current ("latest") docs on polkit.pages.freedesktop.org, plus Fedora 45 package metadata (your `fedora45-primary.xml.zst`). I did not confirm the docs match version 127 exactly. [C] = primary. Polkit is the system service that decides whether a program may do a privileged action.
 
## 1. Format, location, version
 
- **Format:** Yes, JavaScript `.rules` files. They must follow ECMA-262 edition 5 (the JavaScript standard) and talk to polkit through a global `polkit` object. `polkit.addRule()` makes allow/deny decisions; `polkit.addAdminRule()` picks which users count as administrators. [C](https://polkit.pages.freedesktop.org/polkit/polkit.8.html)
- **Where:** `/etc/polkit-1/rules.d` and `/usr/share/polkit-1/rules.d`. Files run in order of file name; on a name tie, `/etc` runs first. Any change reloads all rules. A rule function that returns nothing passes the decision to the next one. [C](https://polkit.pages.freedesktop.org/polkit/polkit.8.html)
- **Fedora 45:** `polkit` 127-5.fc45. It owns `/etc/polkit-1/rules.d`. [C] (package metadata, no public link)
## 2. Documented example: group, no password
 
The page's example lets members of group `admin` manage users without a password. [C](https://polkit.pages.freedesktop.org/polkit/polkit.8.html)
 
```js
polkit.addRule(function(action, subject) {
    if (action.id == "org.freedesktop.accounts.user-administration" &&
        subject.isInGroup("admin")) {
        return polkit.Result.YES;
    }
});
```
 
`action.id` names the action; `polkit.Result.YES` means "allowed, no password".
 
## 3. How a D-Bus service asks polkit
 
The service calls `CheckAuthorization` on the system bus. [C](https://polkit.pages.freedesktop.org/polkit/eggdbus-interface-org.freedesktop.PolicyKit1.Authority.html)
 
| Item | Value |
|---|---|
| Bus name / path / interface | `org.freedesktop.PolicyKit1` / `/org/freedesktop/PolicyKit1/Authority` / `org.freedesktop.PolicyKit1.Authority` |
| Arguments | `subject` (who), `action_id` (what), `details` (extra text for the dialog), `flags`, `cancellation_id` |
| Subject kinds | `unix-process`, `unix-session`, `system-bus-name` |
| Flag | `AllowUserInteraction` lets polkit show a password prompt; use it only for a direct user action |
| Returns | `is_authorized`, `is_challenge` (could be authorized with a password), `details` |
 
The man page itself only says a service can use the D-Bus API directly or run `pkcheck`. [C](https://polkit.pages.freedesktop.org/polkit/polkit.8.html)
 
## Not confirmed
 
- Whether the docs match version 127 exactly: NOT CONFIRMED.
- How a service should pick the subject in practice: NOT FOUND in these pages.
 

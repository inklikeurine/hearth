# P3 — PIN as password on Fedora 45
 
**Sources:** upstream man pages (read on man.archlinux.org), plus authselect and libpwquality files from their GitHub master branches, not the exact versions in Fedora 45. I read authselect's profile files, not authselect(8). [C] = primary doc or source. [S] = secondary.
 
## 1. Does Fedora 45 support PINs for user login?
 
Fedora 45 is out (3 Nov 2026). [C](https://fedoramagazine.org/announcing-fedora-linux-45/) I found no built-in PIN login in Fedora's docs: NOT FOUND. A feature request for one was closed NOTABUG. [S](https://bugzilla.redhat.com/show_bug.cgi?id=1271831) A PIN would be an ordinary short password.
 
## 2–4. Settings
 
| Setting | What it does | Fedora default | Link |
|---|---|---|---|
| `yescrypt` (pam_unix option) | Hashes new passwords with yescrypt; pam_unix reads hashes from `/etc/shadow` | On; `local` profile sets it explicitly (without it: `login.defs` `ENCRYPT_METHOD`, else SHA512) | [C](https://raw.githubusercontent.com/authselect/authselect/master/profiles/local/system-auth) [C](https://man.archlinux.org/man/pam_unix.8.en) |
| `with-faillock` (authselect feature) | Switches pam_faillock on | Optional; one Fedora example shows it off | [C](https://raw.githubusercontent.com/authselect/authselect/master/profiles/local/README) [S](https://fedoramagazine.org/how-to-use-authselect-to-configure-pam-in-fedora-linux/) |
| `deny=` | Failures in a row before the account locks | 3 | [C](https://man.archlinux.org/man/faillock.conf.5.en) |
| `unlock_time=` | Seconds until the lock lifts | 600 | [C](https://man.archlinux.org/man/faillock.conf.5.en) |
| `fail_interval=` | Window in which failures must happen to count | 900 seconds | [C](https://man.archlinux.org/man/faillock.conf.5.en) |
| `minlen` (pwquality) | Minimum length of a new password | 8; cannot be set below 6 | [C](https://man.archlinux.org/man/pwquality.conf.5.en) |
| `enforce_for_root` | Applies the checks to root too | Off: root gets a warning but can set it | [C](https://man.archlinux.org/man/pwquality.conf.5.en) |
 
- The hash starts with `$y$` in `/etc/shadow`. [C](https://fedoraproject.org/wiki/Changes/yescrypt_as_default_hashing_method_for_shadow)
- The faillock values are the man-page defaults. Whether Fedora's own `faillock.conf` changes them: NOT FOUND.
- `pam_pwquality` is in the password-change stack (`requisite`), not the login stack. [C](https://raw.githubusercontent.com/authselect/authselect/master/profiles/local/system-auth)
- Yes: `minlen` blocks a 4-digit PIN. A user changing their own password is refused, since 4 is below 8 and below the floor of 6. libpwquality's sample config leaves `minlen` commented out, so the default of 8 applies. [C](https://raw.githubusercontent.com/libpwquality/libpwquality/master/src/pwquality.conf)
- Recommendation: don't make the PIN the Unix password; no `minlen` setting allows 4 digits.
 

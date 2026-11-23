# M2: Shipping an extension in the OS image (Firefox on Linux)

*Research chat result, run by the lead on 2026-11-22 and copied here as returned. For `hearth-firefox-bridge` (design doc, sections 6.1 and 8.2).*

Checked 2026-11-22. Tags: [C] Mozilla's own page, [S] secondary, [M] memory. Quotes were relayed by a page-reading tool, so check wording before copying into a decision record.

## 1. policies.json force-install from a local file

- Locations: `/etc/firefox/policies` for system-wide, or `firefox/distribution` inside the install directory (varies by distribution). Docs give no precedence if both exist. [C] https://support.mozilla.org/en-US/kb/customizing-firefox-using-policiesjson
- Fedora's actual path: NOT FOUND in Mozilla's docs.
- Local files: "If installing from the local file system, use a file:/// URL." Firefox re-installs the extension when the XPI file changes. `force_installed`: "Automatically installs the extension and prevents it from being removed by the user." [C] https://mozilla.github.io/enterprise-admin-reference/reference/policies/extensionsettings/
- Syntax, assembled from the documented fields (not copied from a page, untested):

  ```json
  { "policies": { "ExtensionSettings": {
    "myext@example.org": {
      "installation_mode": "force_installed",
      "install_url": "file:///path/to/myext.xpi" } } } }
  ```

  Keys are extension IDs; UUID-style IDs need braces. [C] same page
- Every profile: the docs don't say. [M] policies are per install, not per profile.
- Signing: Mozilla says signing isn't optional in standard releases. [C] https://support.mozilla.org/kb/add-on-signing-in-firefox. Whether policy installs are exempt: NOT FOUND.

## 2. System-wide extensions folder

- Yes, still documented. For all users: `/usr/lib/mozilla/extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}/`, the `/usr/lib64/...` twin, or `/usr/share/mozilla/extensions/{ec8030f7-...}/`. [C] https://extensionworkshop.com/documentation/publish/distribute-sideloading/
- Not automatic: "The standard installation of Firefox disables the automatic installation of add-ons from these locations"; the user is asked to approve or must enable it in the add-on manager. [C] same page
- The page's Firefox 73 sentence ("no longer possible to have an extension be automatically installed") was cut off in the chat's read; its meaning is unconfirmed. [C]
- Signed only: the page doesn't say outright; it refers to "the signed add-on file you downloaded from AMO". [C] A 2024 bug report shows a signed XPI in `/usr/share/mozilla/extensions/` loading. [S] https://bugzilla.mozilla.org/show_bug.cgi?id=1884677
- The pref that controls the prompt, `extensions.autoDisableScopes`: NOT FOUND in the docs. Default 15 [M]; a 2011 bug mentions 15 [S] https://bugzilla.mozilla.org/show_bug.cgi?id=703577

## 3. Native messaging manifests (Linux)

- System-wide: `/usr/lib/mozilla/native-messaging-hosts/<name>.json` or `/usr/lib64/mozilla/native-messaging-hosts/<name>.json`. Per-user: `~/.mozilla/native-messaging-hosts/<name>.json`. [C] https://developer.mozilla.org/en-US/docs/Mozilla/Add-ons/WebExtensions/Native_manifests
- `/usr/share/mozilla/native-messaging-hosts`: NOT FOUND on that page.

## Needs testing on real Fedora

- Which policies.json path Fedora's build reads.
- Whether a policy-installed extension skips the approval prompt.
- Whether the `/usr/share` native-messaging path works.

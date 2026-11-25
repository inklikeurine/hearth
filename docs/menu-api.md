# Menu Contribution API, version 1

*Claude's draft, 2026-11-24, from the lead's design notes ([App Menu Contribution API](research/2026-11-20-app-menu-contribution-api.md)). The notes are agreed; the format and the limits below are Claude's and wait for the lead.*

An app adds items to the menu Hearth opens with the Menu button. The app sends data and Hearth draws it: an app can't put pictures, markup or code in the menu. The app's name and icon come from its installed files, never from here.

## Over D-Bus, for native apps

Name `org.hearth.Shell`, object `/org/hearth/Menu`, interface `org.hearth.Menu1`, on the session's bus.

| Member | What it does |
|---|---|
| `SetMenu(s json) → b accepted` | Replaces the app's items |
| `ClearMenu()` | Removes them |
| `ItemChosen(s id, v value)` (signal, sent to that app only) | The person chose an item |

Only a program Hearth started, or one that program started, can add items, and they show only while that app is in front. Anyone else is refused. The items go when the program ends.

## The menu

```json
{
  "version": 1,
  "items": [
    { "id": "next", "type": "action", "label": "Next episode" },
    { "id": "subtitles", "type": "toggle", "label": "Subtitles", "on": true },
    { "id": "audio", "type": "choice", "label": "Audio", "options": ["English", "French"], "current": 0, "enabled": false }
  ]
}
```

| Type | Extra fields | `ItemChosen` value |
|---|---|---|
| `action` | none | true |
| `toggle` | `on` | what it's asked to become, true or false |
| `choice` | `options`, `current` (the place in `options`, from 0) | the place it's asked to go to |

- `enabled` is optional; false shows the item but nothing can choose it.
- Choosing an item only asks. What the menu shows changes when the app sends its next menu.
- Submenus don't exist.

## Limits

| What | Limit | Over it |
|---|---|---|
| The whole menu | 8 KB | Refused |
| Items | 12 | Refused |
| A label or option | 40 characters, one line, plain text | Cut |
| An ID | 40 characters, each used once | That item is left out |
| Options in a choice | 20 | That item is left out |
| Menus sent | 10 in 10 seconds | Refused |
| An item of an unknown type, or with a missing field | — | That item is left out |

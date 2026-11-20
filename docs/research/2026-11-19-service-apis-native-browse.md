# Could Hearth draw its own browse screens for YouTube and Twitch? (2026-11-19)

Research task G2 (the "hybrid" route: Hearth draws rows of videos natively from a service's official API, and the web page only plays the video), checked in a design session against the services' own developer pages. Tags: [C] read on the provider's own page today; [M] inference, not confirmed. Jellyfin is left out: Hearth already uses its official API.

## YouTube: ruled out by its terms

- The Data API needs an API key or OAuth for its requests; the default allocation is 10,000 units a day per project (plus 100 `search.list` calls), "subject to change" [C, [getting started](https://developers.google.com/youtube/v3/getting-started)].
- The developer policies say you must not "embed your API Credentials in open source projects", and credentials belong to "exactly one (1) API Project for that API Client" [C, [developer policies](https://developers.google.com/youtube/terms/developer-policies), section III.D]. Hearth is open source, so it can't ship a key; every user would need their own Google Cloud project.
- They also say apps "must not mimic or replicate YouTube's core user experiences" without significant independent value, and must not "modify, interfere with, replace" YouTube functionality [C, same page, section I].
- So a Hearth-drawn YouTube browser is out. YouTube stays a Firefox web app running YouTube's own TV interface.

## Twitch: possible in principle, not a 1.0 goal

- The API uses OAuth 2.0. Public apps that can't keep a secret are supported: the device code flow does "not need to maintain a client secret" [C, [authentication](https://dev.twitch.tv/docs/authentication/)]. So a user could sign in on their phone and Hearth could list streams [M that every browse call we'd need works with that token].
- Playback in someone else's interface goes through Twitch's embed, which is for websites: it requires HTTPS and a `parent` domain, and "Embeds must utilize only Twitch-approved player elements" [C, [embed](https://dev.twitch.tv/docs/embed)]. Using it inside a Hearth-drawn screen means hosting it in a web page Firefox loads [M].
- Native app embedding: NOT FOUND on the pages read.

## Dropout

Not checked. No public API is known [M].

## What this means

The "hybrid" route doesn't fit YouTube at all and fits Twitch only partly. Making web apps feel native stays with the other two routes: the Hearth web-app API in our extension (consistent Back, focus and theme), and the compositor drawing Hearth's overlays and transitions around the page (series E, prompts E1 and E6).

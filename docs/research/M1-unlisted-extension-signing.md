# M1: Unlisted extension signing (Mozilla's rules)

*Research chat result, run by the lead on 2026-11-22 and copied here as returned. For `hearth-firefox-bridge` (design doc, sections 6.1 and 8.2).*

Checked 2026-11-22. Tags: [C] Mozilla's own page, [S] secondary, [M] memory (none used). Quotes were relayed by a page-reading tool, so check wording against the page before copying into a decision record.

## 1. What unlisted signing needs

- Signing is required: "Extensions and themes need to be signed by Mozilla before they can be installed in release and beta versions of Firefox." [C] https://extensionworkshop.com/documentation/publish/signing-and-distribution-overview/
- Ways to submit: AMO Developer Hub upload, web-ext sign, Add-on Create/Version Create API (V5), Signing API (V4). [C] same page
- API keys (only for web-ext sign and the APIs): `--api-key` is a JWT issuer, `--api-secret` a JWT secret, both from the AMO API. `--channel` is required; `unlisted` is the self-distribution value. [C] https://extensionworkshop.com/documentation/develop/web-ext-command-reference/
- Account: implied by the API keys; the overview page never says it outright. Unconfirmed.
- Review is automatic first, a person possibly later: "Regardless of distribution method, all add-ons undergo automated validation before they are signed." Also "subject to be manually reviewed at any time after submission." Reviews can reject current or previous versions or block the add-on. [C] overview page
- Time: "up to 24 hours", longer if picked for manual review. [C] overview page. web-ext sign waits 15 minutes by default (`--approval-timeout`). [C] web-ext page. Staff say manual review runs days to weeks. [S] https://discourse.mozilla.org/t/stuck-in-review/147896/9

## 2. Policies that apply

- Scope: "All add-ons are subject to these policies, regardless of how they are distributed." [C] https://extensionworkshop.com/documentation/publish/add-on-policies/
- Source code: "Code must be provided in a way that is reviewable." and "Mozilla needs to review a copy of the source code before any of these steps have been applied." Not providing it "results in rejection or blocking." [C] policy page §3.1
- Source code page: required when code comes from minifiers, bundlers, templating or custom build tools. Builds must be reproducible with open-source, non-web tools, plus a lockfile. "matching source code must be attached to every extension version." It does not say whether this applies to unlisted. [C] https://extensionworkshop.com/documentation/publish/source-code-submission/
- Minified: "Add-ons may contain transpiled, minified or otherwise machine-generated code". "Add-ons are not allowed to contain obfuscated code". Minifying to cut size is permitted. [C] policy page
- nativeMessaging: not named. Policies on user consent and control "apply to any data sent to the native application as well"; "Leaking local or user-specific information to websites or other applications (e.g. through native messaging) is prohibited." [C] policy page §6
- `<all_urls>`: not mentioned. Only: "Add-ons must only request those permissions that are necessary for them to function." [C] policy page §4
- A new version, effective Dec 1, 2026, repeats these rules as far as the chat read (first ~96% of the page). [C] https://extensionworkshop.com/documentation/publish/add-on-policies-preview-2026-12/

## 3. AI-generated or AI-assisted code

- NOT FOUND. No mention of AI, LLMs or generative tools in the current policies, the Dec 1 preview, or the source code page. "Machine-generated" there means build output.
- Not policy: Mozilla staff on the forum say manual reviews of AI-related add-ons take longer. [S] https://discourse.mozilla.org/t/question-regarding-review-time-for-gptflow-firefox-add-on/148372
- Not about add-ons: the Firefox AI Coding Policy covers patches to Firefox itself. [C] https://firefox-source-docs.mozilla.org/contributing/ai-coding.html

## Not confirmed / needs testing

- The first signing-page URL tried failed; the chat used the overview page.
- Whether an AMO account is stated as required.
- Whether source code is demanded for unlisted at signing time.
- Real turnaround for an extension with nativeMessaging and all-sites content scripts: needs a test submission.

## Ground-rule check (the chat's note, the lead's call)

The "follow upstream AI policy" rule is written for contributions. Signing your own extension isn't a contribution, so the chat read this as no conflict.

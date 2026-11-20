# B5 – Open-source font facts
 
Sources: project repo/README ("repo"); Google Fonts data files ("GF"), `github.com/google/fonts/tree/main/ofl/<family>/METADATA.pb`; packages.fedoraproject.org. GF scripts = what Google serves, which can be fewer than the font has. Pages were read through a summarising fetch tool. Legibility claims are the makers' own; none cite independent tests.
 
| Font | Licence | Scripts | Weights | Static files? | Fedora package | Legibility claims (who) | Link |
|---|---|---|---|---|---|---|---|
| Inter | OFL 1.1; reserved name "Inter" | GF: Latin, Cyrillic, Greek, Vietnamese. Repo: NOT FOUND | 9 (100–900) + italics | Yes: repo says also distributed as distinct files | rsms-inter-fonts | "Tall x-height" to aid mixed-case reading; "designed for computer screens" (Rasmus Andersson, repo) | https://github.com/rsms/inter |
| Noto Sans | OFL 1.1 | Repo: Latin, Greek, Cyrillic. GF adds Devanagari, Vietnamese | Repo: NOT FOUND. GF: 100–900, width 62.5–100 | NOT FOUND (GF: variable only) | google-noto-sans-fonts | None found | https://github.com/notofonts/latin-greek-cyrillic |
| Atkinson Hyperlegible Next | OFL 1.1 | GF: Latin only (+ext). Repo: NOT FOUND | 6, upright + italic (repo); GF axis 200–800 | NOT FOUND (GF: variable only) | atkinson-hyperlegible-next-fonts | "Developed specifically to increase legibility for readers with low vision" (repo, googlefonts org; named for Braille Institute; designers not named on page) | https://github.com/googlefonts/atkinson-hyperlegible-next |
| Source Sans 3 | OFL 1.1 | GF: Latin, Cyrillic, Greek, Vietnamese. Repo: NOT FOUND | Repo: NOT FOUND. GF: 200–900 | Repo lists OTF and TTF folders beside a VF folder; page doesn't describe them | adobe-source-sans-pro-fonts (v3.052; upstream adobe-fonts/source-sans) | "Designed to work well in UI environments" (Adobe; Paul D. Hunt). Not a legibility claim | https://github.com/adobe-fonts/source-sans |
| Lexend | OFL 1.1 | Repo: Latin, Arabic. GF: Latin only (+ext, Vietnamese) | Variable, Thin–Black (GF 100–900) | NOT FOUND | NOT FOUND (only texlive-lexend) | "Designed to aid in reading proficiency"; says studies show readers improve fluency, none cited (Bonnie Shaver-Troup, Thomas Jockin, repo) | https://github.com/googlefonts/lexend |
| Red Hat Text | OFL 1.1; reserved name "Red Hat" | Repo: NOT FOUND. GF: Latin (+ext) | Repo: "range of weights", not listed. GF: 300–700 | Repo lists OTF and TTF folders; doesn't say "static" | redhat-text-fonts | "Slightly smaller x-height and narrower width for better legibility"; thinned joins for small sizes (Red Hat) | https://github.com/RedHatOfficial/RedHatFont |
| IBM Plex Sans | OFL (links to 1.1) | Repo: Extended Latin, Arabic, Chinese (Trad.), Cyrillic, Devanagari, Greek, Hebrew, Japanese, Korean, Thai. GF: Latin, Cyrillic, Greek, Vietnamese | Repo: NOT FOUND. GF: 100–700, width 75–100 | NOT FOUND (repo: "multiple formats") | ibm-plex-sans-fonts | No legibility claim; "designed to work well in UI environments" (IBM) | https://github.com/IBM/plex |
| Fira Sans | OFL 1.1 (LICENSE file; Mozilla Foundation, Telefonica) | GF: Latin, Cyrillic, Greek, Vietnamese. Repo: NOT FOUND | GF: 9 weights 100–900, each with italic | Yes: GF lists 18 static TTFs | mozilla-fira-sans-fonts | None found. Repo archived; design info at Carrois Studio | https://github.com/mozilla/Fira |
 
## Added candidates (sans-serif only)
 
| Font | Licence | Scripts | Weights | Static files? | Fedora package | Legibility claims (who) | Link |
|---|---|---|---|---|---|---|---|
| Clear Sans (Intel) | Apache 2.0 | Latin, Cyrillic, Greek (repo) | Thin, Light, Regular, Medium; bold and italic styles | Repo lists TTF, WOFF, EOT, SVG folders; doesn't say "static" | intel-clear-sans-fonts | "Designed with on-screen legibility in mind" (Intel UX team; designer Daniel Ratighan, Monotype) | https://github.com/intel/clear-sans |
| Open Sans | OFL (GF) | GF: Latin, Cyrillic, Greek, Hebrew, Vietnamese | GF: 300–800, width 75–100 | NOT FOUND (GF: variable only) | open-sans-fonts | None found. Designer Steve Matteson (GF) | https://fonts.google.com/specimen/Open+Sans |
| Roboto | OFL 1.1 | GF: Latin, Cyrillic, Greek, Vietnamese. Repo: NOT FOUND | Repo: 6 weights, two widths. GF: 100–900 | NOT FOUND (repo describes variable build) | google-roboto-fonts | None found. Google Fonts-funded, Type Network-built | https://github.com/googlefonts/roboto-3-classic |
| Public Sans | OFL 1.1 | Latin only (repo) | Repo: NOT FOUND. GF: 100–900 | Yes: OTF and webfonts; variable experimental | uswds-public-sans-fonts | Narrower characters aid reading flow; even strokes "especially at small sizes" (USWDS) | https://github.com/uswds/public-sans |
 
Maintenance: Intel archived Clear Sans in January 2023 and stopped updates. Public Sans says it isn't actively maintained. Intel's other open font, One Mono, is monospace, so it's left out. Cantarell not added: its project page wasn't reachable.
 
Not confirmed: Google Fonts specimen pages returned only metadata (page script not run), so GF facts come from its data files. Not checked in repo file lists (GitHub access off). Fedora lookups were name searches; Lexend has no font package in those results.
 

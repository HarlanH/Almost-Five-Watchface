Fuzzy Text Watch Star
=====================

## Overview

This is a watch face for the [Pebble smartwatch](http://www.getpebble.com).
It is based on the [PebbleTextWatch](https://github.com/wearewip/PebbleTextWatch) by waerewip,
and [Fuzzy-Text-Watch-Plus](https://github.com/Sarastro72/Fuzzy-Text-Watch-Plus) by Sarastro72.

Compared to Fuzzy-Text-Watch-Plus, this fork adds day-of-month display, upcoming-meeting flags
and Bluetooth and battery indicators, additional languages, fixed backlight on gestures,
and other small changes.


## For users

### Features

- Fuzzy time in natural language with 5 minute precision.
- Current languages: Swedish, English (US, default), English (UK), Norwegian, Dutch, Italian, Spanish, German (east), German (west), French, Japanese (romaji)
- The large and easy to read fonts of the original Text Watch
- Buzz and message when bluetooth connection is lost
- Top status indicators: bluetooth disconnect and low battery (< 40%)
- Localized status strings for meeting alerts and low battery text
- Configurable colors
- Supports all pebble models up to and including Pebble Time Round
- Configurable time offset for when each fuzzy phrase changes
- Gesture-triggered backlight pulse
- Bottom localized day-of-month text (language-dependent format)
- Nice staggered animation
- Between one and four lines of text, depending on need.
- The text rows are always centered vertically
- Smaller words may share a single line (such as "ten to")
- Refactored code for better flexibility/rewriteability

### Getting the watchface

This watchface is available on the Pebble App store as (pending).

### Settings

Open this watch face’s settings in the Pebble / Rebble app to change appearance, language, time offset, backlight-on-gesture behavior, Bluetooth disconnect alerts, and Google Calendar iCal URLs.

The companion app uses an embedded `data:` URL for the configuration page, so no external hosted config site is required.

### Calendar and meeting status

- Add up to three Google Calendar iCal URLs in settings.
- Events with `TRANSP:TRANSPARENT` or `X-MICROSOFT-CDO-BUSYSTATUS:FREE` are ignored.
- Meeting status is shown in the top row (`meeting soon` / `meeting now`) and localized with the active watch language.
- Calendar polling is aligned to real 5-minute boundaries (`:00`, `:05`, `:10`, etc.) with minute boundary status checks to avoid delayed state transitions.

### Possible future features

- [ ] Japanese in kana/kanji: system fonts lack CJK glyphs; bundle a subset font and load it for the text layers that show time strings.
- [ ] More languages
- [ ] Configurable and improved text alignment
- [ ] Graphic icon variants for bluetooth and battery indicators


## For developers

### Prerequisites

- Rebble/Pebble SDK tooling with the `pebble` CLI available in your shell.
- Node.js 18+ if you want to run PKJS regression tests (`npm test`).

### Build and install (examples)

- `pebble build` — compile the app; the bundle is written under `build/` (for example `build/Fuzzy-Text-Watch-Plus.pbw`).
- `pebble install --phone 192.168.0.218` — install to a watch on the LAN; replace the IP with your phone’s address (Pebble app must allow developer connection).
- `pebble install --emulator flint` — install to a running emulator; replace `flint` with your target (for example `basalt`, `chalk`, `aplite`).

To remove build artifacts, run `pebble clean` or delete the `build/` directory.

### Troubleshooting

- If `pebble build` fails because `pebble` is missing, install or re-link your Pebble/Rebble SDK tooling.
- If configuration fails to open, inspect PKJS logs from the phone/emulator runtime.
- If settings save but do not apply, inspect phone logs for PKJS AppMessage payloads and ensure message keys match `package.json`.

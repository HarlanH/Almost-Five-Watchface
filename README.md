Fuzzy Text watch Plus
========================
This is a watch face for the [Pebble smartwatch](http://www.getpebble.com). It is based on the
[PebbleTextWatch](https://github.com/wearewip/PebbleTextWatch) by waerewip

I wanted to combine the elegant layout and animations of the TextWatch with the natural language of
the Fuzzy Time watchface and I wanted it in my native language, Swedish.

I have since then added more languages, colors, connection lost notifications and more...

Features
--------
 - Fuzzy time in natural language with 5 minute precision.
 - Several languages are supported.
 - The large and easy to read fonts of the original Text Watch
 - Buzz and message when bluetooth connection is lost
 - Top status indicators: bluetooth disconnect and low battery (< 40%)
- Localized status strings for meeting alerts and low battery text
 - Configurable colors
 - Supports all pebble models up to and including Pebble Time Round
 - Configurable time offset for when each fuzzy phrase changes (offset values are in seconds)
 - Gesture-triggered backlight pulse
- Bottom localized day-of-month text (language-dependent format)
 - Nice staggered animation
 - Between one and four lines of text, depending on need.
 - The text rows are always centered vertically
 - Smaller words may share a single line (such as "ten to")
 - Refactored code for better flexibility/rewriteability

Possible future features
----
 - [ ] More languages
 - [ ] Configurable text alignment
 - [ ] Graphic icon variants for bluetooth and battery indicators
 - [ ] Calendar-aware text/message integration
 - [x] ~~Configurable time offset (when the watch switches time)~~
 - [x] ~~Configurable languages in one app~~

Preview
-------
A demo video of an old left justified version is available here:
https://www.youtube.com/watch?v=ccubUg9Hp5Y


Download / Install
------------------
This watchface is available on the Pebble App store as [Fuzzy Text Plus](https://apps.getpebble.com/en_US/application/52de476d6094ff6bf0000046).

Modern Rebble development workflow
----------------------------------

Prerequisites
- Rebble/Pebble SDK tooling with `pebble` CLI available in your shell.
- Node.js 18+ for PKJS regression tests.

Build commands
- `npm run build` - configure and build a `.pbw` from a clean checkout.
- `npm run clean` - remove build artifacts.
- `npm test` - run PKJS regression tests.

Legacy make targets still work and delegate to npm scripts:
- `make c` - build
- `make l` - build + load first `build/*.pbw`
- `make d` - build + reinstall first `build/*.pbw`

Configuration page
------------------

The PKJS companion now uses an embedded `data:` URL configuration page, similar to `anylist-pebble`.
This removes dependency on an external hosted config URL and simplifies emulator/device setup.

Calendar meeting status
-----------------------

- Add up to three Google Calendar iCal URLs in settings.
- Events with `TRANSP:TRANSPARENT` or `X-MICROSOFT-CDO-BUSYSTATUS:FREE` are ignored.
- Meeting status is shown in the top row (`meeting soon` / `meeting now`) and localized with the active watch language.
- Calendar polling is aligned to real 5-minute boundaries (`:00`, `:05`, `:10`, etc.) with minute boundary status checks to avoid delayed state transitions.

Migration troubleshooting
-------------------------

- If `npm run build` fails with missing `pebble`, install or re-link your Pebble/Rebble SDK tooling.
- If configuration fails to open, inspect PKJS logs from the phone/emulator runtime.
- If settings save but do not apply, inspect phone logs for PKJS AppMessage payloads and ensure message keys match `package.json`.


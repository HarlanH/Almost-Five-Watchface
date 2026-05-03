# Top-row hybrid + gesture/weather follow-up plan

Branch: `cursor/top-row-hybrid-gesture-weather-baaa`  
Last feature commit: `c0a2113`  
Includes merged env setup from `main` at: `17a441a`

## Goal

Finish verification and polish for the top-row redesign:

- Hybrid top row (left/right compact indicators + center rotating phrase)
- Contextual gesture behavior
- 3-second backlight timeout
- `BAT!` non-numeric low battery indicator
- `SHH` quiet-time indicator
- Weather phrase from PKJS

## Current implementation status

Implemented in this branch:

- C app updates:
  - `src/TextWatch.c`
  - `src/TextWatch.h`
- PKJS updates:
  - `src/js/pebble-js-app.js`
- AppMessage key added:
  - `package.json` (`KEY_WEATHER_PHRASE: 11`)
- JS tests added:
  - `test/pkjs/pebble-js-app.test.js`

Behavior currently coded:

1. **Hybrid top row**
   - Left: `BT!` and/or `SHH`
   - Center: meeting phrase and weather phrase rotate every minute when both are present
   - Right: `BAT!` when battery < 40%
2. **Contextual gesture**
   - Backlight off: gesture turns on light
   - Backlight on: gesture advances top-row center phrase
   - Backlight timeout set to 3000ms
3. **Weather phrase**
   - PKJS fetches Open-Meteo (`weather_code`, `temperature_2m`)
   - Phrase normalized/truncated and sent via `KEY_WEATHER_PHRASE`

## Required next steps in fresh environment

Run these in order:

1. `npm test`
2. `pebble build`
3. Install to emulator and validate behavior:
   - `pebble install --emulator basalt`
   - `pebble install --emulator flint`

## Manual verification checklist

- [ ] Top row shows `SHH` when quiet mode is active.
- [ ] Low battery shows `BAT!` only (no numbers).
- [ ] With both meeting + weather available, center phrase rotates on minute changes.
- [ ] Meeting phrase is **not pinned** (weather still appears during rotation).
- [ ] First gesture turns light on.
- [ ] Additional gestures while lit advance center phrase.
- [ ] Backlight stays on about 3 seconds, then turns off.
- [ ] No text overflow/clipping on aplite/basalt/diorite/flint.

## Known follow-up opportunities (optional polish)

1. **Weather location configurability**  
   Current code uses fixed coordinates in PKJS. Add settings-backed location when desired.

2. **Phrase style tuning**  
   Adjust weather labels for tone/length as preferred (e.g., "cloudy", "light rain", etc.).

3. **Top-row width pressure**  
   If long meeting strings collide visually on specific platforms, consider:
   - Slightly tighter center phrase max length
   - Per-platform abbreviation map for meeting/weather

## If issues are found

- Keep behavior decisions from product direction:
  - contextual gesture model
  - `BAT!` style
  - no meeting pinning
- Make fixes in focused commits and push to this same branch.

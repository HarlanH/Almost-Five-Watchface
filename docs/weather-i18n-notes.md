# Weather i18n — Doc’s Note (future work)

On-watch formatting uses `strings/weather.yaml` → `npm run codegen:weather` → `src/generated/weather_i18n_gen.{h,c}`. PKJS only fetches Open-Meteo and sends `KEY_WEATHER_CODE` + `KEY_WEATHER_TEMP_F` (always Fahrenheit); non–US English locales convert to °C on the watch for band text.

## Implemented baseline

- Unknown WMO/Open-Meteo codes: sky segment is `"-"` from `sky_unknown` in YAML.
- Temperature bands: same low / mid / high within-decade behavior as the former JS (ported in `weather_format.c`).
- String length: no extra policy beyond existing top-row buffer and ellipsis.

## Deferred / nice-to-have

1. **Real translations** — All `locales` in `weather.yaml` currently share the English anchor. Split per locale when translators are available.
2. **Codegen in CI** — Today generated C is committed; optionally add a CI check that `npm run codegen:weather` produces a clean git diff.
3. **UK / US temperature display policy** — Today only `LANG_EN` (US) uses °F bands; `LANG_EN_GB` uses °C. Revisit if you want UK to stay in °F colloquially.
4. **Configurable location** — Open-Meteo URL still uses fixed coordinates in PKJS; move to settings when desired.
5. **Richer sky taxonomy** — More WMO codes or shorter labels per platform could be added to YAML without changing AppMessage shape.
6. **AppMessage ordering** — If you ever split code/temp across messages, teach the watch to buffer partial updates.
7. **Host-side tests for `weather_format.c`** — Logic is C-only; optional native unit tests or golden-file tests against codegen output.

# Unifying `lang-*.c` with YAML/codegen (future)

**Status:** Not planned for now. Weather uses `strings/weather.yaml` → `npm run codegen:weather`. **Date ordinals and `bat_low`** use `strings/ui.yaml` → `npm run codegen:ui`. Fuzzy-time phrases, hours, and exceptions still live in hand-maintained `src/lang-*.c` + `num2words.c`.

This note captures tradeoffs if we revisit merging everything into one data-driven pipeline.

## Why unify later

- **Single workflow** for translators: edit YAML (or one structured format) → codegen → C.
- **Consistency** with the weather pipeline; no “two kinds of i18n.”
- **Easier bulk edits** (terminology, length policy, adding a locale in one place).

## What makes it harder than weather-only codegen

- The **`Language` struct** is richer than weather: 12 hour words, 12 pentaminute phrases, 4 greetings, connection-lost, meeting strings, and a **variable-length `exceptions` array** per locale. (Day-of-month and battery-low copy moved to `strings/ui.yaml`.)
- **`SCREEN_WIDE` vs narrow** (e.g. English in `lang-english.c`): layout differs by platform. Codegen must emit **two phrase/greeting variants** per language, or encode layout-specific fields in YAML (e.g. `phrases_narrow` / `phrases_wide`).
- **Reviewability**: Small per-language `.c` files are easy to diff today; generated C can be noisy unless **YAML is the only reviewed artifact**.
- **Migration cost**: Touch `num2words.c`, all `lang-*` consumers, and verify **parity** (tests or golden outputs) so fuzzy time behavior does not drift.

## Options when the time comes

1. **Full merge** — One schema (or merged YAML files) generates the full `Language` tables + exceptions + wide/narrow splits.
2. **Middle ground** — Codegen only “simple” strings from YAML (greetings, meeting, connection lost) and keep hour/phrase/exception tables in C until the schema is ready.
3. **Stay split** — Keep `lang-*.c` if fuzzy-time copy is stable and only weather (or other satellite copy) churns; duplication stays bounded.

## When it’s worth doing

Unification pays off when **ongoing multi-language maintenance** of `lang-*.c` clearly exceeds the **one-time cost** of a generator that correctly models `Language`, exceptions, and `SCREEN_WIDE`.

## Related

- `docs/weather-i18n-notes.md` — weather-specific follow-ups.
- `AGENTS.md` — current codegen commands and overview.

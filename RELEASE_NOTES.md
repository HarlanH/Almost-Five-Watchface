# Almost Five release notes

## 1.1.0

### Appstore text (short)

- Added a YAML-based local emulator config workflow for development (`dev-config.yml` + `npm run emu:apply-config`).
- The emulator config helper sends AppMessage tuples directly, bypassing flaky config-webview callbacks.
- Updated project docs and development guidance for the local emulator config flow.

### Detailed

- Added direct YAML-driven emulator config injection tooling:
  - `scripts/emu-apply-config.js`
  - `dev-config.yml`
  - test coverage for config mapping and tuple payload generation
- Added `yaml` dependency and npm script integration for local config workflows.
- Updated README with:
  - local emulator config instructions
  - screenshot gallery updates
- Removed legacy, unused hosted-config assets under `config/`.

## 1.0.0

first release


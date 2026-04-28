# Rebble Migration Checklist

## Build and packaging
- [ ] `wscript` is tracked in git and builds all target platforms from `package.json`.
- [ ] `npm run build` succeeds from a clean checkout.
- [ ] Generated `.pbw` appears in `build/`.

## Config webview
- [ ] PKJS opens repo-hosted config URL.
- [ ] Config page assets load (`slate.css` and `slate.js`).
- [ ] Submitting config returns valid JSON to PKJS.

## AppMessage behavior
- [ ] Incoming settings update language, offset, message time, gesture, and BT notification.
- [ ] Color/inverse behavior is preserved on color and monochrome watches.
- [ ] Updated settings persist across restart.

## Platforms
- [ ] Aplite smoke test passes.
- [ ] Basalt smoke test passes.
- [ ] Chalk smoke test passes.

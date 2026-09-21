# Changelog

Format follows [Keep a Changelog](https://keepachangelog.com/1.1.0/).

## Public API

These are what the version number promises, so they are worth listing where
they can be diffed:

- the entry-point file paths under `packages/`
- every `id:` the package declares — see [docs/reference.md](docs/reference.md)
- every `bp_*` and `btn_*` substitution name, and its default
- the `button_plus::` C++ helpers
- slot semantics: 0–5 top-left to bottom-right, 4 is the navigation slot
- the contract: `bp_events`, `bp_render_page`, and optionally `bp_page_prev` /
  `bp_page_next`
- **the default `name:` strings**, because Home Assistant derives entity ids
  from them

### Versioning

| | |
|---|---|
| **major** | renaming or removing an id, substitution or C++ symbol; changing a default `name:`; changing slot semantics; adding a required contract item |
| **minor** | new ids, new substitutions with defaults, new optional entry points, new icons, `min_version` bumps |
| **patch** | comments, docs, and fixes that change no name and no id |

`min_version` bumps are minor on purpose. ESPHome has no LTS and deprecates on
roughly a six-month cycle; treating each of its releases as a major here would
spend the major number on someone else's schedule. The bump fails loudly and
early, and anyone pinned to a tag is unaffected until they move.

Tags are **immutable**. A broken release is fixed with a new version, never by
moving a tag — a moved tag is invisible to anyone using `refresh: never`.

## [1.0.0] — 2026-09-21

First release. Extracted from a private home-automation repository where it
drives a Button+ V2 on a living-room wall.

### Added

- Button+ V2 hardware: ESP32-S3, octal PSRAM, both I²C buses, the SPI bus, four
  MCP23008 expanders, seven displays with per-panel chip-select guards, two
  backlight rails, a 20-LED SK6805 chain, STS3x temperature and LTR-303 ambient
  light.
- An on-device page engine. Navigation never leaves the panel, so it keeps
  working with Home Assistant unreachable.
- Slot-based buttons: a press reports its position, and the page decides what
  that means — so Home Assistant hears `lights_on`, not "bar 1 left".
- A curated MDI icon catalogue with a text fallback for anything not compiled
  in, and 17 named colours.
- `packages/extras/lvgl-paging.yaml` to wire the display-module buttons to an
  LVGL instance, as an opt-in rather than a requirement.
- `example/demo.yaml`: a panel that works with no Home Assistant, no wifi and no
  secrets.
- [docs/hardware.md](docs/hardware.md), recording the panel's real geometry, the
  six-slot SPI budget, and the things that do not work.

**Requires ESPHome ≥ 2026.9.0.**

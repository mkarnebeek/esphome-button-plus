# esphome-button-plus

ESPHome support for the [Button+](https://button.plus/) V2 wall panel: the base
module, the display module and up to three bar modules, as one package you can
drop into your own config.

It gives you the hardware — seven displays, nine buttons, twenty LEDs, two
sensors, two backlights — plus a page engine that runs **on the device**, so
pressing a button changes the panel immediately whether or not Home Assistant is
reachable.

> Unaffiliated third-party project. Not made, endorsed or supported by Button+.

## About this project

This repository is mostly vibecoded with Claude Code, and mostly read by me. It is
verified to work, tested and performance-tweaked on real hardware. I have tested
every aspect of it, but I have not mounted it on a wall yet — I am still looking for
the right spot. I am convinced it is at daily-driver level for stability and
performance, and I intend to use it as-is. My own ESPHome setup references this
repository directly, so I will be pushing updates as I go.

Feel free to fork it and to send PRs. I will review them (with the help of AI) in my
own time, which can sometimes mean weeks.

At the time of writing I believe this works better than the vendor-provided
firmware: it seems more stable and more performant, and for Home Assistant users it
integrates far better.

What you get over the vendor firmware:

- Better stability, especially across reboots and reconfigurations.
- LVGL rendering, which supports far more on screen — gauges, music cover art and
  more.
- Tweaked for responsiveness. For example:
  - SPI writes to the displays have been heavily optimised.
  - Button presses fire on release, instead of waiting out the hold timeout.
  - Pressing play updates the cover art near-instantly.

I hope you have as much fun using this as I had setting it up. Enjoy!

## Status

**Hardware revision 2 only.** V1 and V2 are not interchangeable: V2 moved the
display bus from IO35/36/37 to IO5/6/7 and swapped the two I²C buses. V1 is out
of scope.

Requires ESPHome **2026.9.0** or newer.

## Quick start

```yaml
substitutions:
  bp_home_page: home

esphome:
  name: hall-panel

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
api:
ota:
  platform: esphome

packages:
  button_plus: github://mkarnebeek/esphome-button-plus/packages/button-plus-v2.yaml@v1.0.0

# Every event name your pages can fire.
event:
  - platform: template
    id: bp_events
    name: "Buttons"
    device_class: button
    event_types: [lights_on, lights_off]

# What each page puts in each of the six slots.
script:
  - id: bp_render_page
    mode: restart
    parameters:
      page: string
    then:
      - if:
          condition: {lambda: 'return page == "home";'}
          then:
            - script.execute: {id: bp_act, slot: 0, title: Lights, icon: lightbulb,     color: amber, event: lights_on}
            - script.execute: {id: bp_act, slot: 1, title: "Off",  icon: lightbulb-off, color: grey,  event: lights_off}
```

That is a working panel. For something you can flash and use with no Home
Assistant at all, see [`example/demo.yaml`](example/demo.yaml); for the panel
this package was written for, warts and all, see
[`example/living-room-reference.yaml`](example/living-room-reference.yaml) —
including how album art and a drift-free media progress bar are done.

This package configures **the panel, not the node** — you bring `esphome:`,
`wifi:`/`ethernet:`, `api:`/`mqtt:`, `ota:` and `logger:` yourself.

## The contract

Two things you must provide:

| | |
|---|---|
| `bp_events` | an `event` entity listing every event name your pages fire |
| `bp_render_page` | a script taking `page: string` that fills the slots for that page |

And one optional extension point: `bp_page_prev` / `bp_page_next`, the two
buttons on the display module. Loading `extras/lvgl-paging.yaml` (which the full
entry point does) wires them to an LVGL instance called `main_lvgl`. Extend them
yourself for anything else.

Full detail in [docs/contract.md](docs/contract.md).

## Slots

The six bar buttons are numbered by position, not identity:

```
      bar 1   [ slot 0 ] [ slot 1 ]
      bar 2   [ slot 2 ] [ slot 3 ]
      bar 3   [ slot 4 ] [ slot 5 ]
                  ^
                  back button, filled automatically
                  on every page except the home page
```

A press reports its **slot**, and the page engine looks up what the current page
put there. So Home Assistant never hears "bar 1 left" — it hears `lights_on`.
Rearranging a page cannot break an automation.

## Filling a slot

```yaml
- script.execute: {id: bp_nav, slot: 0, title: Scenes, icon: palette, color: amber, go: scenes}
- script.execute: {id: bp_act, slot: 1, title: Play,   icon: play,    color: green, event: media_play}
```

`bp_nav` goes to another page, `bp_act` fires an event. Use `bp_button` directly
for a hold action or a non-default title colour.

`icon` is an MDI name if the glyph is compiled in, and otherwise rendered as
plain text — which is how live readings work with no second mechanism:

```yaml
icon: !lambda 'char b[8]; snprintf(b, sizeof(b), "%.1fC", id(temp).state); return std::string(b);'
```

Colours are `red`, `green`, `blue`, `amber`, `cyan`, `purple`, `orange`, `grey`
and friends, or `#RRGGBB`.

## Composing

The full entry point is everything:

```yaml
packages:
  button_plus: github://mkarnebeek/esphome-button-plus/packages/button-plus-v2.yaml@v1.0.0
```

Or take the pieces — **either the full entry point or a set of these, never
both**, or every id is declared twice:

| File | What it brings |
|---|---|
| `packages/hardware.yaml` | board, PSRAM, buses, expanders, seven displays, backlights |
| `packages/page-engine.yaml` | bar LVGL, fonts, six buttons, the page scripts |
| `packages/leds.yaml` | the 20-LED chain as one light |
| `packages/led-partitions.yaml` | one light entity per LED |
| `packages/sensors.yaml` | on-board temperature and ambient light |
| `packages/diagnostics.yaml` | free-PSRAM sensor |
| `packages/extras/lvgl-paging.yaml` | display buttons → an LVGL instance |

```yaml
packages:
  button_plus:
    url: https://github.com/mkarnebeek/esphome-button-plus.git
    ref: v1.0.0
    refresh: never
    files:
      - packages/hardware.yaml
      - packages/page-engine.yaml
```

## Home Assistant

The panel sends **meanings**, not button identities. One automation maps them:

```yaml
triggers:
  - trigger: state
    entity_id: event.hall_panel_buttons
    not_from: unavailable
    not_to: [unavailable, unknown]
    variables:
      press: "{{ trigger.to_state.attributes.event_type }}"
```

The `not_from`/`not_to` filters matter: without them the event replays when the
device reconnects and your lights come on by themselves.

See [`example/home-assistant.yaml`](example/home-assistant.yaml).

## Substitutions

Every name is overridable, so you can run the panel in your own language without
touching the package. Full table in [docs/reference.md](docs/reference.md).

Home Assistant derives entity ids from `name:`, not from `id:` — so overriding
these is also how you keep entity ids stable across an upgrade.

## Hardware notes

[docs/hardware.md](docs/hardware.md) records what the panel actually does, as
opposed to what its datasheets imply: the six-slot SPI budget and why one panel
must register per transaction, the bar panels' real 160-pixel long axis, their
asymmetric column offsets, and why screen transition animations were abandoned.
Worth reading before changing anything in `packages/parts/`.

## Versioning

Tags are annotated and **immutable** — a broken release is fixed with a new
version, never by moving a tag. Pin one:

```yaml
ref: v1.0.0
refresh: never
```

The public API is the entry-point paths, every `id:`, every `bp_*` and `btn_*`
substitution, the `button_plus::` C++ helpers, slot semantics, the contract, and
the default `name:` strings (because those decide entity ids). See
[CHANGELOG.md](CHANGELOG.md).

## Credits

**[dixi83/ESPhome_ButtonPlus](https://github.com/dixi83/ESPhome_ButtonPlus)** got
there first. That project is the original proof that a Button+ can run ESPHome at
all, and it was the starting point for this one — the module topology it
documents (one MCP23008 per module, addressed by base slot, with the same GP
layout for chip selects and buttons throughout) is what made the V2 bring-up a
matter of hours rather than guesswork. MIT licensed.

It targets **V1**, and V1 and V2 are not pin-compatible, so none of its GPIO
assignments carry over — see [docs/hardware.md](docs/hardware.md). It also drives
the main panel as an ILI9341; on V2 that panel is an ST7789V, which matters for
gamma, power rails and mirroring. Those differences are where this repository
diverges, and neither is a criticism of a project that predates the V2
schematics entirely.

**[Button+](https://button.plus/)** publish the hardware documentation this is
built on:

- [V2 schematics and pinout](https://button.plus/support/docs/123) — the source
  for every GPIO in `packages/hardware.yaml`, including the IO5/6/7 display bus
  and the swapped I²C buses that distinguish V2 from V1
- [Support documentation](https://button.plus/support) — module layout and
  assembly
- [Firmware downloads](https://button.plus/support/firmware) — the stock
  firmware, worth keeping a copy of before flashing ESPHome. V2 builds are
  suffixed `-V2` and must not be crossed with V1 builds.

## Provenance

Extracted in September 2026 from a private home-automation repository, which
consumes this package to drive a Button+ V2 for the living room. It is developed
by being used.

## Licence

MIT — see [LICENSE](LICENSE). Third-party components in [NOTICE.md](NOTICE.md).

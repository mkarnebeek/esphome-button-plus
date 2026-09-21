# The contract

The page engine knows that pages exist. It does not know which ones — that is
yours. Two things must exist in your config for it to compile.

## 1. `bp_events`

An `event` entity naming every event your pages can fire. The engine fires into
it whenever a pressed slot carries an event name.

```yaml
event:
  - platform: template
    id: bp_events
    name: "Buttons"
    device_class: button
    event_types:
      - lights_on
      - lights_off
      - media_play
```

An event fired that is not in `event_types` is dropped at runtime, so this list
has to stay in step with your pages.

## 2. `bp_render_page`

A script taking `page: string`, which the engine calls on every navigation.
Fill the slots that page uses; everything is blanked first, so a page only
declares what it needs.

```yaml
script:
  - id: bp_render_page
    mode: restart
    parameters:
      page: string
    then:
      - if:
          condition: {lambda: 'return page == "home";'}
          then:
            - script.execute: {id: bp_nav, slot: 0, title: Scenes, icon: palette, color: amber, go: scenes}
```

Because it runs on every navigation, this is also the natural home for a page's
side effects — anything that should happen when a page is shown.

## 3. `bp_page_prev` / `bp_page_next` — optional

The two buttons on the display module. They are declared as empty scripts, so
out of the box they do nothing and the package still compiles.

`extras/lvgl-paging.yaml` — which the full entry point loads — extends them to
page an LVGL instance called `main_lvgl`. Override `bp_main_lvgl` if yours has a
different id:

```yaml
substitutions:
  bp_main_lvgl: my_display_lvgl
```

For anything that is not LVGL, leave that file out and extend the hooks:

```yaml
script:
  - id: !extend bp_page_prev
    then:
      - display.page.show_previous: main_display
```

## Filling slots

| Script | Parameters | Use |
|---|---|---|
| `bp_nav` | `slot, title, icon, color, go` | go to another page |
| `bp_act` | `slot, title, icon, color, event` | fire an event |
| `bp_button` | `slot, title, icon, color, title_color, event, hold, go` | everything else |

ESPHome scripts have no default parameter values, so `bp_button` needs all eight
arguments every time — which is why the two shorthands exist.

`icon` is resolved against the compiled MDI catalogue; anything that is not a
known glyph name is drawn as text instead. That fallback is deliberate and is
how live values are displayed.

`color` and `title_color` accept a named colour or `#RRGGBB`.

## Slots

```
      bar 1   [ slot 0 ] [ slot 1 ]
      bar 2   [ slot 2 ] [ slot 3 ]
      bar 3   [ slot 4 ] [ slot 5 ]
```

Slot 4 is the navigation slot. On any page other than `${bp_home_page}` it is
filled with a back button *before* `bp_render_page` runs — so pages do not
declare it, but may overwrite it if they want that position for something else.

## What you also bring

This package configures the panel, not the node. You supply `esphome:`,
`wifi:` or `ethernet:`, `api:` or `mqtt:`, `ota:` and `logger:` — and, if you
want anything on the main display, an LVGL instance or a display lambda of your
own. The package deliberately leaves `main_display` unattached.

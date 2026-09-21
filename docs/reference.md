# Reference

Everything in this file is public API — see [CHANGELOG.md](../CHANGELOG.md) for
what that means for versioning.

## Substitutions

All optional; the defaults are what you get if you say nothing. Set any of them
in your own `substitutions:` block, which outranks the package.

### Naming

Home Assistant derives entity ids from `name:`, not from `id:`, so these decide
your entity ids. Overriding them is how you run the panel in another language,
and how you keep entity ids stable across an upgrade that changes a default.

| Substitution | Default |
|---|---|
| `bp_main_backlight_name` | `Display backlight` |
| `bp_bar_backlight_name` | `Bar backlight` |
| `bp_config_button_name` | `Config button` |
| `bp_temperature_name` | `Temperature` |
| `bp_light_name` | `Ambient light` |
| `bp_psram_name` | `Free PSRAM` |
| `bp_leds_name` | `LEDs` |
| `bp_led_display_left_front_name` | `Display LED left front` |
| `bp_led_display_left_wall_bottom_name` | `Display LED left wall bottom` |
| `bp_led_display_left_wall_middle_name` | `Display LED left wall middle` |
| `bp_led_display_left_wall_top_name` | `Display LED left wall top` |
| `bp_led_display_right_front_name` | `Display LED right front` |
| `bp_led_display_right_wall_bottom_name` | `Display LED right wall bottom` |
| `bp_led_display_right_wall_middle_name` | `Display LED right wall middle` |
| `bp_led_display_right_wall_top_name` | `Display LED right wall top` |
| `bp_led_bar{1,2,3}_left_front_name` | `Bar N LED left front` |
| `bp_led_bar{1,2,3}_left_wall_name` | `Bar N LED left wall` |
| `bp_led_bar{1,2,3}_right_front_name` | `Bar N LED right front` |
| `bp_led_bar{1,2,3}_right_wall_name` | `Bar N LED right wall` |

### Pages

| Substitution | Default | |
|---|---|---|
| `bp_home_page` | `home` | the page with no back button |
| `bp_back_title` | `Back` | title of the automatic back button |
| `bp_back_icon` | `arrow-left` | its icon |
| `bp_back_color` | `grey` | its colour |
| `bp_title_color` | `grey` | default title colour for `bp_nav` / `bp_act` |

### Buttons

| Substitution | Default | |
|---|---|---|
| `btn_debounce` | `30ms` | shortest press that counts |
| `btn_click_max` | `499ms` | longest press still a click |
| `btn_hold_min` | `500ms` | shortest press that is a hold |

`btn_click_max` deliberately stops one below `btn_hold_min`. Keep them adjacent
if you change them, or a press can fire both or neither.

### Fonts and LVGL

| Substitution | Default | |
|---|---|---|
| `bp_mdi_font_file` | `../bpv2-assets/...ttf` | the icon font |
| `bp_text_font_file` | `gfonts://Inter@600` | bar title and message font |
| `bp_main_lvgl` | `main_lvgl` | which LVGL instance the display buttons page |

A relative path you set for either font resolves from **your** config directory,
while the default resolves from inside the package. Absolute paths and
`gfonts://` avoid the question.

## Ids

### Displays and LVGL

| | |
|---|---|
| `main_display` | the 320×240 ST7789V. Nothing is attached to it — that is yours |
| `bar1_left` … `bar3_right` | the six 160×80 bar panels |
| `bar1_left_lvgl` … `bar3_right_lvgl` | one LVGL instance per bar panel |

### Buses and expanders

`bus_modules`, `bus_sensors`, `module_j0` … `module_j3`

### Entities

| | |
|---|---|
| `main_backlight`, `bar_backlight` | the two backlight rails |
| `leds` | the 20-LED chain |
| `led_display_left_front` … `led_bar3_right_wall` | one light per LED |
| `panel_temperature` | STS3x, °C |
| `panel_light` / `panel_illuminance` | the LTR-303 component / its lux sensor |
| `config_button_events` | the recessed CONFIG switch: click, hold, release |

### Scripts

| | |
|---|---|
| `bp_show(page)` | show a page; what a back button and `go:` call |
| `bp_nav(slot, title, icon, color, go)` | fill a slot with a navigation button |
| `bp_act(slot, title, icon, color, event)` | fill a slot with an event button |
| `bp_button(slot, title, icon, color, title_color, event, hold, go)` | the full form |
| `bp_press(slot)`, `bp_hold(slot)` | what the physical buttons call |
| `bp_page_prev`, `bp_page_next` | the display-module buttons; extend these |

### C++ helpers

Available in any lambda:

```cpp
button_plus::current_page()                 // std::string&, the page showing now
button_plus::slots()[n]                     // .event .hold .go for slot n
button_plus::mdi_glyph("robot-vacuum")      // const char*, nullptr if unknown
button_plus::parse_color("amber", 0xFFFFFF) // uint32_t
```

`current_page()` is the useful one — it lets a sensor redraw a page only when
that page is actually showing:

```yaml
on_value:
  - if:
      condition: {lambda: 'return button_plus::current_page() == "scenes";'}
      then: [{script.execute: {id: bp_show, page: scenes}}]
```

## Named colours

`white`, `black`, `red`, `green`, `blue`, `yellow`, `orange`, `purple`, `pink`,
`cyan`, `magenta`, `lime`, `teal`, `amber`, `grey`/`gray`, `warmwhite`.

Anything else is parsed as `#RRGGBB`, `RRGGBB` or `0xRRGGBB`.

## Icons

The compiled catalogue is curated, not the whole MDI set (over 7000 glyphs) — each costs roughly
800 bytes of flash. The list is in `packages/parts/icons.yaml`, grouped by
navigation, lighting, window coverings, climate, media, household, rooms, scenes
and status.

Adding one means adding a line to **both** `parts/icons.yaml` and
`bpv2/mdi_icons.h`; they are a matched pair and the file comments say so.

An `icon:` that is not in the catalogue is drawn as text instead.

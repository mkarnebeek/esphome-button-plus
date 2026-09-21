# Examples

## `demo.yaml` — no Home Assistant needed

```bash
esphome run demo.yaml --device /dev/ttyACM0
```

No wifi, no secrets, no Home Assistant. Flash it and the panel is immediately
usable, which makes it the fastest way to prove your hardware works.

You need a V2 base, a display module and at least one bar module. With fewer
than three bars the missing panels simply never draw; the config still runs.

| Bar page | What it shows |
|---|---|
| **home** | navigation to the other pages, and one event button |
| **Colours** | lights the whole chain red / green / blue / amber / off |
| **LEDs** | lights one group at a time — a wiring check for the chain order |
| **Sensors** | live temperature, lux and uptime rendered as button text |
| **Icons** | a sample of the compiled icon catalogue |

On any page other than home, the bottom-left button is a back button that the
page engine fills in for you.

The two buttons on the display module page between the two main screens: a
status screen, and a diagram of which physical button is which slot.

To put it on your network, uncomment the `wifi:`, `api:` and `ota:` blocks at
the top.

## `home-assistant.yaml` — the real pattern

Placeholder entity ids at the top; replace them with your own. This shows:

- an `event` entity carrying **meanings** (`lights_on`), not button positions
- importing state back from Home Assistant, and redrawing a page **only when it
  is the one showing** — using `button_plus::current_page()`
- a slot whose icon, colour and event all depend on state
- a live sensor reading drawn on a button, via the icon-or-text fallback

### The Home Assistant side

One automation maps every event:

```yaml
automation:
  - alias: Living room panel
    triggers:
      - trigger: state
        entity_id: event.living_room_panel_buttons
        not_from: unavailable
        not_to: [unavailable, unknown]
        variables:
          press: "{{ trigger.to_state.attributes.event_type }}"
    actions:
      - choose:
          - conditions: "{{ press == 'lights_on' }}"
            sequence:
              - action: light.turn_on
                target: {entity_id: light.living_room}
          - conditions: "{{ press == 'media_play_pause' }}"
            sequence:
              - action: media_player.media_play_pause
                target: {entity_id: media_player.living_room}
```

The `not_from` / `not_to` filters are not optional. An `event` entity goes
`unavailable` when the device reboots and then re-reports its **last** event on
reconnect — without those filters your lights come on by themselves every time
the panel restarts.

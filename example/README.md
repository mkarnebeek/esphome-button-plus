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

## `living-room-reference.yaml` — the real thing

The panel this package was written for, published as reference. **It does not
run as-is** — it still refers to a house base layer, Home Assistant entities and
an image resizer that do not exist in this repository. Read it for the ideas,
not as a starting point.

Two parts of it are genuinely hard to work out from scratch:

### Album art

The panel cannot decode a full-size cover. ESPHome's JPEG decoder draws **one
pixel per callback**, so a 640×640 image is 409,600 calls — about **2.3 seconds
with the main loop stopped**. The expanders are polled from that same loop, so a
button press landing in that window is lost entirely. At 88×88 it is 7,744 calls,
roughly **43ms**, and the transfer drops from 80kB to under 2kB.

Home Assistant serves album art at 640×640 and offers no way to ask for a
smaller one, so the fix is a resizer on the LAN. Any will do; the reference uses
[willnorris/imageproxy](https://github.com/willnorris/imageproxy):

```yaml
services:
  imageproxy:
    image: ghcr.io/willnorris/imageproxy:latest
    command:
      - -addr=0.0.0.0:8080
      # Not optional. Without it this is an open relay that anyone on your LAN
      # can use to fetch arbitrary URLs.
      - -allowHosts=your-home-assistant,i.scdn.co
    ports: ["8088:8080"]
```

Serve it over **plain HTTP on its own port, not behind a TLS reverse proxy**. A
TLS handshake blocks the ESP32's main loop for hundreds of milliseconds — more
than the decoding this exists to avoid.

Two more traps, both in the reference file's comments:

- `lvgl.image.update` with `src:` on `on_download_finished`, not
  `lvgl.widget.update` (which rejects `src`). ESPHome builds the image
  descriptor lazily, and the widget asks for it once at startup when the image
  is still 0×0 — so without re-setting the source, the art downloads correctly
  and never appears.
- `buffer_size: 4096` on the `online_image`. The 64kB default pulls the whole
  image in one loop iteration; smaller bites spread it across several.
- **Only call `online_image.set_url` when the URL has actually changed.**
  `set_url()` does not compare against the URL it already holds — it clears the
  cached etag and last-modified and starts a fresh download every time. Home
  Assistant sensors fire `on_value` on every state *report*, not only on change,
  so a render script hung off a media player will call it several times a second
  and re-fetch the same cover forever. The symptom is `Image already being
  updated` in the log. The reference keeps the loaded URL in a global and
  compares; that took a playing track from 10 downloads in 50s to 1 in 91s.

### A media progress bar that does not drift

Polling the media player for its position does not work: it reports 0 for a
whole track on some integrations, and loses the value on resume.

Home Assistant gives you `media_position` **and** `media_position_updated_at`,
the timestamp at which that position was measured. Hold both, and the elapsed
time is arithmetic the device can do locally — which survives seeking, pausing
and track changes, and needs no polling at all. See the `globals:` block and
`render_progress`.

The reference reads a Music Assistant entity rather than the Sonos one, because
the Sonos entity reported position 0 for the whole track and lost it on resume.

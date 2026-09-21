# Hardware notes

What the panel actually does, as opposed to what its datasheets imply. Each of
these cost real debugging; they are written down so nobody has to repeat it.

## Pin map (V2)

| Function | GPIO |
|---|---|
| I²C module bus (expanders) SDA / SCL | 1 / 2 |
| I²C sensor bus SDA / SCL | 47 / 48 |
| LED chain data | 4 |
| CONFIG switch | 9 |
| TFT MOSI / SCLK / DC | 5 / 6 / 7 |
| Bar backlight (PNP, inverted) | 46 |
| Main backlight (PNP, inverted) | 3 |

From the [official V2 schematics](https://button.plus/support/docs/123). Button+
also publish [firmware downloads](https://button.plus/support/firmware) — take a
copy of the stock build before flashing ESPHome if you might want to go back.

**V2 is not V1.** V2 moved the display bus off IO35/36/37 and swapped the two
I²C buses, forced by the octal PSRAM — octal claims GPIO33-37 on the ESP32-S3,
exactly where V1 drove its displays.

GPIO3 and GPIO46 are strapping pins. That is Button+'s own design, so the
package sets `ignore_strapping_warning: true` rather than warn forever.

## No display has a hardware chip select

Every panel's CS hangs off the MCP23008 on its own module, so **every SPI
transaction costs an I²C write**. That is why both buses run at 400kHz instead
of the 50kHz ESPHome defaults to on the ESP32 — at 50kHz the I²C traffic would
dominate the frame time.

Each module carries one MCP23008, addressed by the slot it sits in (J0 `0x20`
display, J1-J3 `0x21`-`0x23` bars), with the same GP layout throughout:

| GP | |
|---|---|
| 1 | CS, right display |
| 2 | button, right |
| 5 | CS, left display (display module: its single panel) |
| 6 | button, left |
| 7 | LED type strap |

Check this against the I²C scan in your first boot log — which slot the display
module occupies depends on how the panel was assembled, and everything keys off
it.

This topology is unchanged from V1 and is documented in
[dixi83/ESPhome_ButtonPlus](https://github.com/dixi83/ESPhome_ButtonPlus), which
worked it out first. **Its GPIO numbers do not carry over**: V1 drove the
displays on IO35/36/37 and used the opposite assignment for the two I²C buses.
Octal PSRAM claims GPIO33-37 on the ESP32-S3, which is what forced the move.

Each panel also gets a **CS guard**: a `gpio` output on the same expander pin,
`inverted: true`. The expander powers up with every pin an input, so without it
the line floats until the display component claims it — and a floating chip
select during another panel's init means that panel latches this one's init
sequence too. Outputs are set up at priority 800 and displays at 600, so the
guard always wins.

## The SPI device-slot budget

The ESP32-S3 SPI host has **six** device slots (`SOC_SPI_MAX_CS_NUM`) and the
panel has **seven** displays.

A transient device still needs a free slot to register into, so the ceiling is
**five permanent holders plus one slot the rest take turns in**. Overflow it and
the losing panel fails every transmit with `Add device failed - err 101`.

So bar 3 gives up both its slots (`release_device: true` on both its panels) and
the main display keeps one. Measured, registering per transaction costs about
**8ms per refresh**. A bar only refreshes on a page change; the main display
redraws on the clock, the media progress bar and anything else you put there —
so the main display is the one worth keeping permanent:

| | main full redraw | paging all six bars |
|---|---|---|
| main transient | 80ms | 108ms |
| main permanent | **64ms** | 116ms |

If you have fewer than three bar modules you have spare slots and can drop
`release_device` accordingly.

## Bar panel geometry

Established on the hardware by putting a different candidate on each of the six
panels and drawing a border frame.

```
long axis  160  (offset_height 0, pad_height 0)
short axis 132  (offset_width 24, pad_width 28)
```

**The long axis is 160, not 162.** A 132×162 GRAM implies 162, and that is what
the datasheet geometry suggests, but at 162 the controller wraps each row two
pixels later than the data supplies: the image shears 2px per row and a border
rectangle renders as a single diagonal across the panel. 164 doubles the shear.

**The short-axis offset is 24, not 26.** 26 would centre the band in the
132-column GRAM, but the panel sits off-centre — 24 before, 28 after. Pinned
down by a leftover: at 26, GRAM columns 24 and 25 fell outside the written
window, so on panels whose previous frame had written there the old pixels
survived as a thin bright line along one long edge, while panels that never had
stayed dark.

**Right-hand panels need the mirror image.** The offsets are asymmetric, and
mipi_spi resolves them from opposite ends depending on rotation: 270 takes
`offset_width`, 90 takes `pad_width`. The band sits at GRAM column 24 either
way, so right-hand panels state 28/24 rather than 24/28. Declaring all six
identically leaves the right-hand ones 4px out — visible as their text sitting
slightly lower. The two panels in a bar are mounted 180° apart, which is why
this is right rather than a workaround.

`pad_width` and `pad_height` must be stated explicitly: ESPHome's built-in
ST7735 model is 128×160, so without them it computes a negative pad and rejects
the config with "Invalid offsets".

## The main panel is an ST7789V

Not an ILI9341, which is what the V1 community config uses. Both work well
enough to show a picture — they share the standard MIPI DCS commands that
matter (`CASET`, `RASET`, `RAMWR`, `MADCTL`, `COLMOD`, `SLPOUT`, `DISPON`) — but
their vendor init sequences differ completely in gamma, power rails and frame
rate, and ILI9341's model defaults include `mirror_x`, so driving this panel as
one renders it **mirrored** and mis-tuned.

It also needs `invert_colors: true`: the panel comes up showing the complement
of every colour. That is inversion, not a channel swap, which would have left
green untouched.

Rotation is **not** set on the display. LVGL refuses a display that rotates
itself and wants to own it, so set `rotation:` on your `lvgl:` block. Because
this driver reports hardware rotation, LVGL still rotates via MADCTL rather than
shuffling pixels in software.

## Refreshing all six bars at once is impossible

Six displays share one SPI bus and are chip-selected one at a time, so they can
never change at the same instant. The best available is a tight burst with
nothing interleaved, which is what `bp_show` does — it calls `lv_refr_now()` on
all six back to back rather than leaving each to its own 16ms refresh timer.
Left to the timers, other loop work lands between panels and the sweep is
visibly slower. That took paging from 172ms to ~113ms.

## Screen transition animations do not work here

MOVE, OVER and both FADE variants were tried, at 100ms to a full second.

A screen-load animation re-renders **both** pages and pushes the whole 320×240
frame over SPI on every frame — about 130ms each. Whatever duration is set, only
two or three frames are ever drawn, so the screen lands at an arbitrary halfway
position once and then snaps: it reads as a glitch, not as movement. Shortening
does not help, because the per-frame cost is fixed.

FADE_IN and FADE_OUT are indistinguishable: they cross-fade the same two images
with the z-order flipped. With both pages opaque and full screen the composite is
identical.

Nor can it be cached around. LVGL has `lv_snapshot_take()` for exactly this, but
ESPHome neither enables `LV_USE_SNAPSHOT` nor exposes a way to call it. Even with
rendering free, one full frame is ~50ms of SPI against the ~33ms smooth motion
needs.

Switching with no animation is a single ~70ms redraw — cheaper than any of the
above, and it looks intentional.

## LED chain

One SK6805 chain leaves the base on GPIO4 and runs through the module connectors
in slot order: the display module's eight first, then four per bar.

SK6805 is SK6812-compatible — hence `chipset: SK6812` and not WS2812. The
timings differ and the wrong one gives colour corruption on long chains.

Display module order, mapped by lighting them one at a time:

```
0  left front          4  left wall, middle
1  left wall, bottom    5  left wall, top
2  right front          6  right wall, middle
3  right wall, bottom   7  right wall, top
```

Two face into the room, six face the wall — three up each side at the three
connector positions. Bars follow left-front, left-wall, right-front, right-wall.

## Buttons

`on_click` fires on the release edge itself, and deliberately does **not** use an
`on_multi_click` timing: a multi-click sequence only completes once its trailing
OFF period elapses, so the event could not land until well after the button came
back up, which is perceptible.

`btn_click_max` (499ms) stops one below `btn_hold_min` (500ms), so the two meet
with neither a gap nor an overlap — no press can fire both, and none falls
through.

Hold fires while the button is still down: a long press should feel like it has
taken effect before you let go.

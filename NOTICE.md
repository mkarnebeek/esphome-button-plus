# Third-party notices

## Material Design Icons

`packages/bpv2-assets/materialdesignicons-webfont.ttf` is redistributed
unmodified, under the **Apache License 2.0**. The upstream licence statement is
reproduced verbatim in `packages/bpv2-assets/LICENSE-materialdesignicons.txt`:
the Pictogrammers Free License places the icons and the fonts under Apache 2.0
and the project's code under MIT.

Only the glyphs listed in `packages/parts/icons.yaml` are compiled into a
firmware; the rest of the font is never shipped to a device.

- Project: Material Design Icons — https://pictogrammers.com/library/mdi/
- Font source: https://github.com/Templarian/MaterialDesign-Webfont
- This build carries 7431 glyphs.

## Inter

`packages/parts/icons.yaml` fetches Inter through ESPHome's `gfonts://` source
at validation time. It is downloaded by the person building the firmware and is
not redistributed by this repository.

- Project: Inter, by Rasmus Andersson — https://rsms.me/inter/
- Licence: SIL Open Font License 1.1

## ESPhome_ButtonPlus (V1)

https://github.com/dixi83/ESPhome_ButtonPlus — MIT licensed.

The original ESPHome project for the Button+, targeting hardware revision 1. It
was the starting point for this repository: its documentation of the module
topology (an MCP23008 per module, addressed by base slot, with a common GP
layout for chip selects and buttons) carried over directly to V2.

No code is copied from it. V1 and V2 differ in their GPIO assignments, their I2C
bus roles and their main display controller, so every pin here was taken from
the V2 schematics instead.

## Button+

This is an unaffiliated, third-party project. It is not made, endorsed or
supported by Button+. The pin assignments come from the publicly published V2
schematics at https://button.plus/support/docs/123

"Button+" is used here only to say which hardware this configures.

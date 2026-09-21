// Two helpers for the Button+ bar panels, shared by all six so the icon table
// exists once in flash rather than once per panel lambda.
//
// The names here MUST stay in step with the mdi_icon font in parts/icons.yaml:
// ESPHome compiles only the glyphs that font lists into the firmware, so a name
// that resolves here to a glyph which was never compiled renders as a blank
// box. Adding an icon means adding a line to both files.
#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

namespace button_plus {

struct MdiIcon {
  const char *name;
  const char *glyph;
};

// Material Design Icons, private use area U+F0000 and up, as UTF-8.
static const MdiIcon MDI_ICONS[] = {
    // navigation
    {"arrow-left", "\363\260\201\215"},  // U+F004D
    {"arrow-right", "\363\260\201\224"},  // U+F0054
    {"arrow-up", "\363\260\201\235"},  // U+F005D
    {"arrow-down", "\363\260\201\205"},  // U+F0045
    {"chevron-left", "\363\260\205\201"},  // U+F0141
    {"chevron-right", "\363\260\205\202"},  // U+F0142
    {"menu", "\363\260\215\234"},  // U+F035C
    {"dots-horizontal", "\363\260\207\230"},  // U+F01D8
    {"close", "\363\260\205\226"},  // U+F0156
    {"check", "\363\260\204\254"},  // U+F012C
    {"plus", "\363\260\220\225"},  // U+F0415
    {"minus", "\363\260\215\264"},  // U+F0374
    // lighting
    {"lightbulb", "\363\260\214\265"},  // U+F0335
    {"lightbulb-group", "\363\261\211\223"},  // U+F1253
    {"lamp", "\363\260\232\265"},  // U+F06B5
    {"ceiling-light", "\363\260\235\251"},  // U+F0769
    {"floor-lamp", "\363\260\243\235"},  // U+F08DD
    {"track-light", "\363\260\244\224"},  // U+F0914
    {"string-lights", "\363\261\212\272"},  // U+F12BA
    {"wall-sconce", "\363\260\244\234"},  // U+F091C
    {"lightbulb-off", "\363\260\271\217"},  // U+F0E4F
    // window coverings
    {"curtains", "\363\261\241\206"},  // U+F1846
    {"curtains-closed", "\363\261\241\207"},  // U+F1847
    {"blinds", "\363\260\202\254"},  // U+F00AC
    {"blinds-open", "\363\261\200\221"},  // U+F1011
    {"window-shutter", "\363\261\204\234"},  // U+F111C
    {"window-shutter-open", "\363\261\204\236"},  // U+F111E
    {"garage", "\363\260\233\231"},  // U+F06D9
    {"garage-open", "\363\260\233\232"},  // U+F06DA
    {"gate", "\363\260\212\231"},  // U+F0299
    {"gate-open", "\363\261\205\252"},  // U+F116A
    {"stop", "\363\260\223\233"},  // U+F04DB
    // climate
    {"thermostat", "\363\260\216\223"},  // U+F0393
    {"fan", "\363\260\210\220"},  // U+F0210
    {"air-conditioner", "\363\260\200\233"},  // U+F001B
    {"radiator", "\363\260\220\270"},  // U+F0438
    {"fire", "\363\260\210\270"},  // U+F0238
    {"snowflake", "\363\260\234\227"},  // U+F0717
    {"water-boiler", "\363\260\276\222"},  // U+F0F92
    {"air-filter", "\363\260\265\203"},  // U+F0D43
    {"water-percent", "\363\260\226\216"},  // U+F058E
    {"heat-pump", "\363\261\251\203"},  // U+F1A43
    // media
    {"music", "\363\260\235\232"},  // U+F075A
    {"play", "\363\260\220\212"},  // U+F040A
    {"pause", "\363\260\217\244"},  // U+F03E4
    {"play-pause", "\363\260\220\216"},  // U+F040E
    {"stop-circle", "\363\260\231\246"},  // U+F0666
    {"skip-next", "\363\260\222\255"},  // U+F04AD
    {"skip-previous", "\363\260\222\256"},  // U+F04AE
    {"speaker", "\363\260\223\203"},  // U+F04C3
    {"television", "\363\260\224\202"},  // U+F0502
    {"radio", "\363\260\220\271"},  // U+F0439
    {"movie-open", "\363\260\277\216"},  // U+F0FCE
    {"volume-high", "\363\260\225\276"},  // U+F057E
    {"volume-medium", "\363\260\226\200"},  // U+F0580
    {"volume-off", "\363\260\226\201"},  // U+F0581
    {"volume-plus", "\363\260\235\235"},  // U+F075D
    {"volume-minus", "\363\260\235\236"},  // U+F075E
    {"cast", "\363\260\204\230"},  // U+F0118
    {"microphone", "\363\260\215\254"},  // U+F036C
    {"playlist-music", "\363\260\262\270"},  // U+F0CB8
    // household
    {"robot-vacuum", "\363\260\234\215"},  // U+F070D
    {"robot-vacuum-variant", "\363\260\244\210"},  // U+F0908
    {"washing-machine", "\363\260\234\252"},  // U+F072A
    {"dishwasher", "\363\260\252\254"},  // U+F0AAC
    {"tumble-dryer", "\363\260\244\227"},  // U+F0917
    {"broom", "\363\260\203\242"},  // U+F00E2
    {"fridge", "\363\260\212\220"},  // U+F0290
    {"stove", "\363\260\223\236"},  // U+F04DE
    {"toaster-oven", "\363\260\263\223"},  // U+F0CD3
    {"silverware-fork-knife", "\363\260\251\260"},  // U+F0A70
    {"coffee", "\363\260\205\266"},  // U+F0176
    // rooms
    {"home", "\363\260\213\234"},  // U+F02DC
    {"sofa", "\363\260\222\271"},  // U+F04B9
    {"bed", "\363\260\213\243"},  // U+F02E3
    {"shower", "\363\260\246\240"},  // U+F09A0
    {"desk", "\363\261\210\271"},  // U+F1239
    {"bookshelf", "\363\261\211\237"},  // U+F125F
    {"stairs", "\363\260\223\215"},  // U+F04CD
    {"door", "\363\260\240\232"},  // U+F081A
    {"window-closed", "\363\260\226\256"},  // U+F05AE
    {"home-floor-1", "\363\260\266\200"},  // U+F0D80
    {"home-floor-2", "\363\260\266\201"},  // U+F0D81
    // scenes
    {"palette", "\363\260\217\230"},  // U+F03D8
    {"weather-night", "\363\260\226\224"},  // U+F0594
    {"white-balance-sunny", "\363\260\226\250"},  // U+F05A8
    {"weather-sunset", "\363\260\226\232"},  // U+F059A
    {"movie", "\363\260\216\201"},  // U+F0381
    {"party-popper", "\363\261\201\226"},  // U+F1056
    {"sleep", "\363\260\222\262"},  // U+F04B2
    {"account-group", "\363\260\241\211"},  // U+F0849
    {"book-open-variant", "\363\261\223\267"},  // U+F14F7
    // status
    {"power", "\363\260\220\245"},  // U+F0425
    {"power-plug", "\363\260\232\245"},  // U+F06A5
    {"lock", "\363\260\214\276"},  // U+F033E
    {"lock-open", "\363\260\214\277"},  // U+F033F
    {"bell", "\363\260\202\232"},  // U+F009A
    {"bell-off", "\363\260\202\233"},  // U+F009B
    {"leaf", "\363\260\214\252"},  // U+F032A
    {"flash", "\363\260\211\201"},  // U+F0241
    {"water", "\363\260\226\214"},  // U+F058C
    {"wifi", "\363\260\226\251"},  // U+F05A9
    {"car", "\363\260\204\213"},  // U+F010B
    {"battery", "\363\260\201\271"},  // U+F0079
    {"cog", "\363\260\222\223"},  // U+F0493
    {"restart", "\363\260\234\211"},  // U+F0709
    {"alert", "\363\260\200\246"},  // U+F0026
    {"information", "\363\260\213\274"},  // U+F02FC
};

// Accepts a bare MDI name ("robot-vacuum") or the "mdi:" form Home Assistant
// uses, so a name can be copied straight out of HA. Returns nullptr when the
// name is not in the catalogue, which is the signal to render the value as
// plain text instead.
inline const char *mdi_glyph(std::string name) {
  size_t b = name.find_first_not_of(" \t");
  if (b == std::string::npos) return nullptr;
  size_t e = name.find_last_not_of(" \t");
  name = name.substr(b, e - b + 1);
  if (name.rfind("mdi:", 0) == 0) name = name.substr(4);
  for (char &c : name) {
    if (c >= 'A' && c <= 'Z') c += 32;
    if (c == '_') c = '-';
  }
  for (const auto &i : MDI_ICONS)
    if (name == i.name) return i.glyph;
  return nullptr;
}

struct NamedColor {
  const char *name;
  uint32_t rgb;
};

static const NamedColor NAMED_COLORS[] = {
    {"white", 0xFFFFFF},
    {"black", 0x000000},
    {"red", 0xE04545},
    {"green", 0x30C060},
    {"blue", 0x3080E0},
    {"yellow", 0xE8D030},
    {"orange", 0xE8A030},
    {"purple", 0xA060D0},
    {"pink", 0xE060A0},
    {"cyan", 0x40C8D0},
    {"magenta", 0xD040B0},
    {"lime", 0x90D030},
    {"teal", 0x30B0A0},
    {"amber", 0xE0B020},
    {"grey", 0x808080},
    {"gray", 0x808080},
    {"warmwhite", 0xFFE0B0},
};

// "#RRGGBB", "RRGGBB", "0xRRGGBB" or one of the names above. Anything else
// falls back, so a typo in Home Assistant leaves the panel readable rather
// than rendering black on black.
inline uint32_t parse_color(std::string s, uint32_t fallback) {
  size_t b = s.find_first_not_of(" \t");
  if (b == std::string::npos) return fallback;
  size_t e = s.find_last_not_of(" \t");
  s = s.substr(b, e - b + 1);
  for (char &c : s) if (c >= 'A' && c <= 'Z') c += 32;
  for (const auto &c : NAMED_COLORS)
    if (s == c.name) return c.rgb;
  if (!s.empty() && s[0] == '#') s = s.substr(1);
  else if (s.rfind("0x", 0) == 0) s = s.substr(2);
  if (s.size() != 6) return fallback;
  for (char c : s)
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return fallback;
  return (uint32_t) strtoul(s.c_str(), nullptr, 16);
}

}  // namespace button_plus

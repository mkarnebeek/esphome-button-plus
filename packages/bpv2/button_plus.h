// State behind the bar-button pages.
//
// A page is six buttons. What each one does is decided when the page is drawn
// and remembered here, so a press only has to look up its own slot rather than
// ask anyone what page is showing.
//
// Deliberately generic: this file knows nothing about which pages exist or what
// they do. The page definitions live in the device's own yaml.
#pragma once

#include <cstdint>
#include <string>

namespace button_plus {

struct Slot {
  std::string event;  // fired on a click, empty for a button that does nothing
  std::string hold;   // fired on a long press, empty for no hold action
  std::string go;     // page to navigate to instead of firing anything
};

// Function-local statics rather than inline variables: no ordering surprises,
// and no dependency on how ESPHome happens to compile this translation unit.
inline Slot *slots() {
  static Slot s[6];
  return s;
}

inline std::string &current_page() {
  static std::string page;
  return page;
}

}  // namespace button_plus

#include <cassert>

#include "touch_priority_dispatch.h"

using touch_priority::Route;

static void test_editor_consumes_every_touch() {
  constexpr int points[][2] = {
      {0, 0}, {479, 0}, {0, 479}, {479, 479},
      {19, 369}, {20, 370}, {229, 451}, {230, 452},
      {239, 400}, {240, 400}, {249, 400}, {250, 370}, {459, 451},
      {460, 452}, {300, 200}, {120, 100},
  };

  for (const auto& point : points) {
    assert(touch_priority::route(true, point[0], point[1], true) ==
           Route::VirtualEditor);
  }
  assert(touch_priority::route(false, 250, 370, true) == Route::None);
}

static void test_normal_mode_is_exclusive_and_half_open() {
  assert(touch_priority::route(true, 20, 370, false) == Route::ProbeWsl);
  assert(touch_priority::route(true, 229, 451, false) == Route::ProbeWsl);
  assert(touch_priority::route(true, 230, 370, false) == Route::None);
  assert(touch_priority::route(true, 20, 452, false) == Route::None);

  assert(touch_priority::route(true, 250, 370, false) == Route::Send3C);
  assert(touch_priority::route(true, 459, 451, false) == Route::Send3C);
  assert(touch_priority::route(true, 460, 370, false) == Route::None);
  assert(touch_priority::route(true, 250, 452, false) == Route::None);

  // The gap between the normal controls is intentionally unassigned.
  assert(touch_priority::route(true, 240, 400, false) == Route::None);
  assert(touch_priority::route(true, 249, 400, false) == Route::None);
}

static void test_editor_priority_over_button_rectangles() {
  assert(touch_priority::route(true, 20, 370, true) == Route::VirtualEditor);
  assert(touch_priority::route(true, 229, 451, true) == Route::VirtualEditor);
  assert(touch_priority::route(true, 250, 370, true) == Route::VirtualEditor);
  assert(touch_priority::route(true, 459, 451, true) == Route::VirtualEditor);
}

int main() {
  test_editor_consumes_every_touch();
  test_normal_mode_is_exclusive_and_half_open();
  test_editor_priority_over_button_rectangles();
  return 0;
}

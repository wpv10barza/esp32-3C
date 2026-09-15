#include <cassert>

#include "touch_priority_dispatch.h"

using touch_priority::Policy;
using touch_priority::Route;

int main() {
  const Policy normal{};

  assert(touch_priority::route(false, 100, 400, normal) == Route::None);
  assert(touch_priority::route(true, 20, 370, normal) == Route::ProbeWsl);
  assert(touch_priority::route(true, 229, 451, normal) == Route::ProbeWsl);
  assert(touch_priority::route(true, 230, 400, normal) == Route::None);
  assert(touch_priority::route(true, 250, 370, normal) == Route::Send3C);
  assert(touch_priority::route(true, 459, 451, normal) == Route::Send3C);
  assert(touch_priority::route(true, 460, 400, normal) == Route::None);
  assert(touch_priority::route(true, 240, 400, normal) == Route::None);
  assert(touch_priority::route(true, 100, 369, normal) == Route::None);

  Policy editing{};
  editing.editorActive = true;

  // Editor mode wins everywhere, including the physical button regions.
  assert(touch_priority::route(true, 30, 390, editing) == Route::VirtualEditor);
  assert(touch_priority::route(true, 300, 390, editing) == Route::VirtualEditor);
  assert(touch_priority::route(true, 240, 240, editing) == Route::VirtualEditor);
  assert(touch_priority::route(true, 479, 479, editing) == Route::VirtualEditor);
  assert(touch_priority::route(false, 300, 390, editing) == Route::None);

  return 0;
}

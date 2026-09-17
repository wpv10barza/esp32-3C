#include <cassert>

#include "touch_priority_dispatch.h"

using touch_priority::Route;

int main() {
  static_assert(touch_priority::kScreenWidth == 480);
  static_assert(touch_priority::kScreenHeight == 480);
  static_assert(touch_priority::kProbeWslButton.right <= 480);
  static_assert(touch_priority::kSend3CButton.right <= 480);
  static_assert(touch_priority::kSend3CButton.left - touch_priority::kProbeWslButton.right == 20);
  static_assert(touch_priority::kScreenHeight - touch_priority::kActionBottom >= 20);

  const int y = touch_priority::kActionTop + 1;

  // Left zone: PROBAR WSL.
  assert(touch_priority::route(true, 20, y, false) == Route::ProbeWsl);
  assert(touch_priority::route(true, 229, y, false) == Route::ProbeWsl);

  // Right zone: ENVIAR 3C.
  assert(touch_priority::route(true, 250, y, false) == Route::Send3C);
  assert(touch_priority::route(true, 459, y, false) == Route::Send3C);

  // Boundaries and dead space must not cross-trigger the other action.
  assert(touch_priority::route(true, 230, y, false) == Route::None);
  assert(touch_priority::route(true, 249, y, false) == Route::None);
  assert(touch_priority::route(true, 240, y, false) == Route::None);
  assert(touch_priority::route(true, 460, y, false) == Route::None);
  assert(touch_priority::route(true, 100, 369, false) == Route::None);
  assert(touch_priority::route(true, 100, 452, false) == Route::None);

  // No touch must never dispatch an action.
  assert(touch_priority::route(false, 100, 400, false) == Route::None);

  // The two action zones remain isolated while an editor is active.
  assert(touch_priority::route(true, 100, 400, true) == Route::None);
  assert(touch_priority::route(true, 350, 400, true) == Route::None);

  return 0;
}

#include <cassert>

#include "touch_priority_dispatch.h"

using touch_priority::Route;

int main() {
  static_assert(touch_priority::kScreenWidth == 480);
  static_assert(touch_priority::kScreenHeight == 480);
  static_assert(touch_priority::kProbeWslButton.right <= 480);
  static_assert(touch_priority::kSend3CButton.bottom <= 480);
  static_assert(touch_priority::kSend3CButton.left - touch_priority::kProbeWslButton.right == 20);

  // The complete action zones must remain visible inside the physical 480x480 LCD.
  assert(touch_priority::kProbeWslButton.left >= 0);
  assert(touch_priority::kProbeWslButton.top >= 0);
  assert(touch_priority::kProbeWslButton.right <= touch_priority::kScreenWidth);
  assert(touch_priority::kProbeWslButton.bottom <= touch_priority::kScreenHeight);
  assert(touch_priority::kSend3CButton.left >= 0);
  assert(touch_priority::kSend3CButton.top >= 0);
  assert(touch_priority::kSend3CButton.right <= touch_priority::kScreenWidth);
  assert(touch_priority::kSend3CButton.bottom <= touch_priority::kScreenHeight);
  assert(touch_priority::kScreenHeight - touch_priority::kSend3CButton.bottom >= 20);

  const int y = touch_priority::kActionTop + 1;
  // Half-open edges make the two zones deterministic and non-overlapping.
  assert(touch_priority::route(true, touch_priority::kProbeWslButton.left, y, false) == Route::ProbeWsl);
  assert(touch_priority::route(true, touch_priority::kProbeWslButton.right - 1, y, false) == Route::ProbeWsl);
  assert(touch_priority::route(true, touch_priority::kProbeWslButton.right, y, false) == Route::None);
  assert(touch_priority::route(true, touch_priority::kSend3CButton.left, y, false) == Route::Send3C);
  assert(touch_priority::route(true, touch_priority::kSend3CButton.right - 1, y, false) == Route::Send3C);
  assert(touch_priority::route(true, touch_priority::kSend3CButton.right, y, false) == Route::None);

  // The 20 px center gap and 20 px footer are dead space, not accidental triggers.
  assert(touch_priority::route(true, 240, y, false) == Route::None);
  assert(touch_priority::route(true, 240, touch_priority::kActionBottom, false) == Route::None);

  // Active editing has exclusive priority, so a held editor touch cannot leak into a button.
  assert(touch_priority::route(true, 40, y, true) == Route::VirtualEditor);
  assert(touch_priority::route(true, 440, y, true) == Route::VirtualEditor);

  // Conservative size-2 GFX budget for the critical labels.
  assert(touch_priority::kProbeWslLabelMaxWidth < touch_priority::kProbeWslButton.width());
  assert(touch_priority::kSend3CLabelMaxWidth < touch_priority::kSend3CButton.width());
  return 0;
}

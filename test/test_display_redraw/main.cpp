#include <cassert>
#include <cstdint>

#include "display_redraw.h"

using display_redraw::Plan;
using display_redraw::Rect;
using display_redraw::UpdateKind;

int main() {
  // Mode transitions are full-frame invalidations so no pixels from the old
  // state can survive into the new state.
  Plan plan = display_redraw::modeTransitionPlan(true);
  assert(plan.kind == UpdateKind::FullScreen);
  assert(plan.region.x == 0 && plan.region.y == 0);
  assert(plan.region.width == 480 && plan.region.height == 480);

  // A detail/text update without a mode change should not force a full frame.
  plan = display_redraw::modeTransitionPlan(false);
  assert(plan.kind == UpdateKind::None);

  // Typing inside the field invalidates only the changed text/cursor union.
  plan = display_redraw::typingUpdatePlan(Rect{60, 278, 80, 18},
                                          Rect{60, 278, 96, 18});
  assert(plan.kind == UpdateKind::Region);
  assert(plan.region.x == 60 && plan.region.y == 278);
  assert(plan.region.width == 96 && plan.region.height == 18);

  // Cursor movement must include the old cursor location as well as the new
  // one, otherwise a one-frame cursor artifact can remain on screen.
  plan = display_redraw::typingUpdatePlan(Rect{92, 280, 3, 20},
                                          Rect{128, 280, 3, 20});
  assert(plan.kind == UpdateKind::Region);
  assert(plan.region.x == 92);
  assert(plan.region.y == 280);
  assert(plan.region.width == 39);
  assert(plan.region.height == 20);

  // Out-of-field input is clipped to the editor region and cannot erase the
  // virtual keyboard or bottom buttons.
  plan = display_redraw::typingUpdatePlan(Rect{-50, 250, 600, 200},
                                          Rect{0, 0, 10, 10});
  assert(plan.kind == UpdateKind::Region);
  assert(plan.region.x == display_redraw::kCommandField.x);
  assert(plan.region.y == display_redraw::kCommandField.y);
  assert(plan.region.width == display_redraw::kCommandField.width);
  assert(plan.region.height == display_redraw::kCommandField.height);
  assert(display_redraw::valid(plan.region));

  // Invalid rectangles still return a safe editor-region update rather than
  // silently doing nothing and leaving stale glyphs behind.
  plan = display_redraw::typingUpdatePlan(Rect{0, 0, 0, 0},
                                          Rect{0, 0, 0, 0});
  assert(plan.kind == UpdateKind::Region);
  assert(plan.region.x == display_redraw::kCommandField.x);
  assert(plan.region.y == display_redraw::kCommandField.y);
  assert(plan.region.width == display_redraw::kCommandField.width);
  assert(plan.region.height == display_redraw::kCommandField.height);

  return 0;
}

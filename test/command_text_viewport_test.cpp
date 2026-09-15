#include <cassert>
#include <cstddef>
#include <cstdint>

#include "command_text_viewport.h"

using command_text_viewport::Window;

int main() {
  // Synthetic measured widths: characters alternate between narrow and wide
  // glyphs so the algorithm cannot accidentally depend on monospace text.
  constexpr uint16_t widths[] = {0, 8, 18, 26, 36, 44, 54, 64, 72, 84, 96, 106, 118};
  constexpr int viewport = 40;
  constexpr int cursorWidth = 2;
  constexpr int drawable = viewport - cursorWidth;

  Window view = command_text_viewport::compute(widths, 12, 0, viewport, cursorWidth);
  assert(view.first == 0);
  assert(view.cursorX == 0);
  assert(view.textWidth <= drawable);
  assert(view.cursorX <= drawable);

  // Moving the cursor into the command must shift the horizontal window
  // instead of deleting the suffix just to make the text fit.
  view = command_text_viewport::compute(widths, 12, 5, viewport, cursorWidth);
  assert(view.first > 0);
  assert(view.first <= 5);
  assert(view.cursorX >= 0);
  assert(view.cursorX <= drawable);
  assert(widths[view.last] - widths[view.first] <= drawable);
  assert(view.textWidth <= drawable);

  // Cursor at the end shows the trailing characters and keeps the insertion
  // point inside the field.
  view = command_text_viewport::compute(widths, 12, 12, viewport, cursorWidth);
  assert(view.last == 12);
  assert(view.first > 0);
  assert(view.cursorX <= drawable);
  assert(view.textWidth <= drawable);

  // Short text does not scroll at all.
  view = command_text_viewport::compute(widths, 4, 4, 100, cursorWidth);
  assert(view.first == 0);
  assert(view.last == 4);
  assert(view.cursorX == widths[4]);
  assert(view.textWidth + cursorWidth <= 100);

  // An oversized measured glyph is never returned as drawable text. The
  // cursor remains valid and the renderer can fail closed without overflow.
  constexpr uint16_t oversized[] = {0, 50, 58};
  view = command_text_viewport::compute(oversized, 2, 0, 20, cursorWidth);
  assert(view.first == 0);
  assert(view.last == 0);
  assert(view.textWidth == 0);
  assert(view.cursorX == 0);

  // Clamped cursor/text lengths never overrun the measured-width array or
  // violate the drawable bound.
  view = command_text_viewport::compute(widths, 999, 999, viewport, cursorWidth);
  assert(view.last <= 12);
  assert(view.first <= view.last);
  assert(view.cursorX <= drawable);
  assert(view.textWidth <= drawable);

  // Degenerate viewport leaves a safe empty result.
  view = command_text_viewport::compute(widths, 12, 4, 2, cursorWidth);
  assert(view.first == 0 && view.last == 0 && view.cursorX == 0);

  return 0;
}

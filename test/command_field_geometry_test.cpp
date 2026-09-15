#include <cassert>
#include <cstddef>

#include "command_field.h"

int main() {
  using namespace command_field;

  assert(insideScreen(kNormalBounds));
  assert(insideScreen(kEditingBounds));

  // Half-open bounds: left/top are included, right/bottom are excluded.
  assert(kNormalBounds.contains(kNormalBounds.left, kNormalBounds.top));
  assert(kNormalBounds.contains(kNormalBounds.right - 1, kNormalBounds.bottom - 1));
  assert(!kNormalBounds.contains(kNormalBounds.right, kNormalBounds.top));
  assert(!kNormalBounds.contains(kNormalBounds.left, kNormalBounds.bottom));

  // The normal field ends before the action buttons at y=370.
  assert(kNormalBounds.bottom <= 360);
  assert(!kNormalBounds.contains(240, 370));

  // The editing field is above the keyboard starting at y=216.
  assert(kEditingBounds.bottom <= 216);
  assert(!kEditingBounds.contains(240, 216));

  // Cursor position is always bounded by the current text length.
  assert(clampCursor(0, 4) == 0);
  assert(clampCursor(4, 4) == 4);
  assert(clampCursor(9, 4) == 4);

  return 0;
}

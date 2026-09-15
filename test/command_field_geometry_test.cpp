#include <cassert>

#include "command_field.h"

int main() {
  using namespace command_field;

  assert(insideScreen(kNormalBounds));
  assert(insideScreen(kEditingBounds));
  assert(kNormalBounds.bottom < 370);
  assert(kEditingBounds.bottom <= 216);

  // Half-open bounds keep every boundary unambiguous.
  assert(kEditingBounds.contains(kEditingBounds.left, kEditingBounds.top));
  assert(kEditingBounds.contains(kEditingBounds.right - 1, kEditingBounds.bottom - 1));
  assert(!kEditingBounds.contains(kEditingBounds.right, kEditingBounds.top));
  assert(!kEditingBounds.contains(kEditingBounds.left, kEditingBounds.bottom));

  const int contentWidth = kEditingBounds.width() - 2 * kHorizontalPadding;
  assert(contentWidth > kCursorWidth);

  assert(clampCursor(0, 48) == 0);
  assert(clampCursor(48, 48) == 48);
  assert(clampCursor(49, 48) == 48);

  return 0;
}

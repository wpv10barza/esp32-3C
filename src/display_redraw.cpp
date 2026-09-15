#include "display_redraw.h"

namespace display_redraw {

namespace {
Rect clampToCommandField(Rect region) {
  const int16_t left = region.x > kCommandField.x ? region.x : kCommandField.x;
  const int16_t top = region.y > kCommandField.y ? region.y : kCommandField.y;
  const int16_t rightA = region.x + region.width;
  const int16_t rightB = kCommandField.x + kCommandField.width;
  const int16_t bottomA = region.y + region.height;
  const int16_t bottomB = kCommandField.y + kCommandField.height;
  const int16_t right = rightA < rightB ? rightA : rightB;
  const int16_t bottom = bottomA < bottomB ? bottomA : bottomB;
  if (right <= left || bottom <= top) return Rect{0, 0, 0, 0};
  return Rect{left, top, static_cast<int16_t>(right - left),
              static_cast<int16_t>(bottom - top)};
}
}  // namespace

Plan typingUpdatePlan(Rect previousBounds, Rect nextBounds) {
  // Restrict invalidation to the editor. A malformed caller rectangle cannot
  // accidentally erase the keyboard, status area, or buttons.
  const Rect previous = clampToCommandField(previousBounds);
  const Rect next = clampToCommandField(nextBounds);
  const Rect dirty = unite(previous, next);
  if (!valid(dirty)) return Plan{UpdateKind::Region, kCommandField};
  return Plan{UpdateKind::Region, dirty};
}

}  // namespace display_redraw

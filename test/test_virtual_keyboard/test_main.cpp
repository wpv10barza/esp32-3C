#include <unity.h>

#include <array>
#include <string>

#include "virtual_keyboard.h"

using namespace virtual_keyboard;

namespace {

bool overlaps(const KeyRect& a, const KeyRect& b) {
  return a.left < b.right && b.left < a.right &&
         a.top < b.bottom && b.top < a.bottom;
}

void validateMode(KeyboardMode mode) {
  std::array<Key, 50> keys{};
  const size_t count = buildKeys(mode, keys.data(), keys.size());
  TEST_ASSERT_EQUAL_UINT(keyCount(mode), count);

  for (size_t i = 0; i < count; ++i) {
    const KeyRect& rect = keys[i].rect;
    TEST_ASSERT_TRUE(rect.left >= 0);
    TEST_ASSERT_TRUE(rect.top >= 0);
    TEST_ASSERT_TRUE(rect.right <= kScreenWidth);
    TEST_ASSERT_TRUE(rect.bottom <= kScreenHeight);
    TEST_ASSERT_TRUE(rect.left < rect.right);
    TEST_ASSERT_TRUE(rect.top < rect.bottom);

    for (size_t j = i + 1; j < count; ++j) {
      TEST_ASSERT_FALSE(overlaps(rect, keys[j].rect));
    }
  }
}

const Key* findKey(KeyboardMode mode, KeyKind kind, const char* label) {
  static std::array<Key, 50> keys{};
  const size_t count = buildKeys(mode, keys.data(), keys.size());
  for (size_t i = 0; i < count; ++i) {
    if (keys[i].definition.kind == kind &&
        std::string(keys[i].definition.label) == label) {
      return &keys[i];
    }
  }
  return nullptr;
}

}  // namespace

void setUp() {}
void tearDown() {}

void test_alpha_layout_is_inside_screen_and_non_overlapping() {
  validateMode(KeyboardMode::Alpha);
}

void test_numeric_layout_is_inside_screen_and_non_overlapping() {
  validateMode(KeyboardMode::NumericSymbols);
}

void test_alpha_has_requested_edit_controls() {
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::Alpha, KeyKind::Space, "SPACE"));
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::Alpha, KeyKind::Backspace, "BKSP"));
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::Alpha, KeyKind::Enter, "ENTER"));
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::Alpha, KeyKind::ToggleAlphaNumeric, "123"));
}

void test_numeric_has_requested_edit_controls() {
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::NumericSymbols, KeyKind::Space, "SPACE"));
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::NumericSymbols, KeyKind::Backspace, "BKSP"));
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::NumericSymbols, KeyKind::Enter, "ENTER"));
  TEST_ASSERT_NOT_NULL(findKey(KeyboardMode::NumericSymbols, KeyKind::ToggleAlphaNumeric, "ABC"));
}

void test_hit_test_uses_unambiguous_boundaries() {
  const Key* q = findKey(KeyboardMode::Alpha, KeyKind::Character, "Q");
  TEST_ASSERT_NOT_NULL(q);

  Key matched{};
  TEST_ASSERT_TRUE(hitTest(KeyboardMode::Alpha, q->rect.left, q->rect.top, &matched));
  TEST_ASSERT_EQUAL_STRING("Q", matched.definition.label);

  TEST_ASSERT_TRUE(hitTest(KeyboardMode::Alpha,
                           q->rect.right - 1,
                           q->rect.bottom - 1,
                           &matched));
  TEST_ASSERT_EQUAL_STRING("Q", matched.definition.label);

  TEST_ASSERT_FALSE(hitTest(KeyboardMode::Alpha, q->rect.right, q->rect.top, nullptr));
}

void test_spanning_controls_have_full_expected_width() {
  const Key* space = findKey(KeyboardMode::Alpha, KeyKind::Space, "SPACE");
  const Key* enter = findKey(KeyboardMode::Alpha, KeyKind::Enter, "ENTER");
  const Key* toggle = findKey(KeyboardMode::Alpha, KeyKind::ToggleAlphaNumeric, "123");
  TEST_ASSERT_NOT_NULL(space);
  TEST_ASSERT_NOT_NULL(enter);
  TEST_ASSERT_NOT_NULL(toggle);

  TEST_ASSERT_EQUAL_INT(6 * kKeyWidth + 5 * kColumnGap,
                        space->rect.right - space->rect.left);
  TEST_ASSERT_EQUAL_INT(2 * kKeyWidth + kColumnGap,
                        enter->rect.right - enter->rect.left);
  TEST_ASSERT_EQUAL_INT(2 * kKeyWidth + kColumnGap,
                        toggle->rect.right - toggle->rect.left);
}

int main(int argc, char** argv) {
  UNITY_BEGIN();
  RUN_TEST(test_alpha_layout_is_inside_screen_and_non_overlapping);
  RUN_TEST(test_numeric_layout_is_inside_screen_and_non_overlapping);
  RUN_TEST(test_alpha_has_requested_edit_controls);
  RUN_TEST(test_numeric_has_requested_edit_controls);
  RUN_TEST(test_hit_test_uses_unambiguous_boundaries);
  RUN_TEST(test_spanning_controls_have_full_expected_width);
  return UNITY_END();
}

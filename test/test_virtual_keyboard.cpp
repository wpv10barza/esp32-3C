#include <cstddef>

#include <unity.h>
#include "virtual_keyboard.h"

using namespace virtual_keyboard;

namespace {

bool overlaps(const KeyRect& a, const KeyRect& b) {
  return a.left < b.right && b.left < a.right &&
         a.top < b.bottom && b.top < a.bottom;
}

void assertMatrixIsValid(KeyboardMode mode) {
  Key keys[50]{};
  const size_t count = buildKeys(mode, keys, 50);
  TEST_ASSERT_EQUAL_UINT(keyCount(mode), count);

  for (size_t i = 0; i < count; ++i) {
    const KeyRect& r = keys[i].rect;
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, r.left);
    TEST_ASSERT_GREATER_OR_EQUAL_INT(0, r.top);
    TEST_ASSERT_LESS_OR_EQUAL_INT(kScreenWidth, r.right);
    TEST_ASSERT_LESS_OR_EQUAL_INT(kScreenHeight, r.bottom);
    TEST_ASSERT_LESS_THAN_INT(r.left, r.right);
    TEST_ASSERT_LESS_THAN_INT(r.top, r.bottom);

    for (size_t j = i + 1; j < count; ++j) {
      TEST_ASSERT_FALSE_MESSAGE(overlaps(r, keys[j].rect),
                                "virtual keyboard keys overlap");
    }
  }
}

void test_alpha_matrix_geometry() {
  assertMatrixIsValid(KeyboardMode::Alpha);
}

void test_numeric_matrix_geometry() {
  assertMatrixIsValid(KeyboardMode::NumericSymbols);
}

void test_hit_test_boundaries_are_deterministic() {
  Key key{};
  TEST_ASSERT_TRUE(hitTest(KeyboardMode::Alpha, kKeyboardX + 1,
                           kKeyboardY + 1, &key));
  TEST_ASSERT_EQUAL_STRING("Q", key.definition.label);

  const int firstRight = kKeyboardX + kKeyWidth;
  TEST_ASSERT_FALSE(hitTest(KeyboardMode::Alpha, firstRight, kKeyboardY + 10,
                            &key));
  TEST_ASSERT_TRUE(hitTest(KeyboardMode::Alpha, firstRight + kColumnGap,
                           kKeyboardY + 10, &key));
  TEST_ASSERT_EQUAL_STRING("W", key.definition.label);

  TEST_ASSERT_FALSE(hitTest(KeyboardMode::Alpha, 0, 0, &key));
  TEST_ASSERT_FALSE(hitTest(KeyboardMode::Alpha, kScreenWidth, kScreenHeight - 1,
                            &key));
}

void test_special_controls_are_in_final_row() {
  Key keys[50]{};
  const size_t count = buildKeys(KeyboardMode::Alpha, keys, 50);
  bool sawToggle = false;
  bool sawSpace = false;
  bool sawBackspace = false;
  bool sawEnter = false;

  for (size_t i = 0; i < count; ++i) {
    if (keys[i].rect.top != rowOrigin(4)) continue;
    switch (keys[i].definition.kind) {
      case KeyKind::ToggleAlphaNumeric: sawToggle = true; break;
      case KeyKind::Space: sawSpace = true; break;
      case KeyKind::Backspace: sawBackspace = true; break;
      case KeyKind::Enter: sawEnter = true; break;
      case KeyKind::Character: break;
    }
  }

  TEST_ASSERT_TRUE(sawToggle);
  TEST_ASSERT_TRUE(sawSpace);
  TEST_ASSERT_TRUE(sawBackspace);
  TEST_ASSERT_TRUE(sawEnter);
}

}  // namespace

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_alpha_matrix_geometry);
  RUN_TEST(test_numeric_matrix_geometry);
  RUN_TEST(test_hit_test_boundaries_are_deterministic);
  RUN_TEST(test_special_controls_are_in_final_row);
  UNITY_END();
}

void loop() {}

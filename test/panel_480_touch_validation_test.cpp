#include <unity.h>

#include "touch_priority_dispatch.h"

using touch_priority::Route;

void test_action_zones_are_inside_480_square() {
  TEST_ASSERT_EQUAL_INT(480, touch_priority::kScreenWidth);
  TEST_ASSERT_EQUAL_INT(480, touch_priority::kScreenHeight);

  TEST_ASSERT_TRUE(touch_priority::kProbeWslButton.left >= 0);
  TEST_ASSERT_TRUE(touch_priority::kProbeWslButton.top >= 0);
  TEST_ASSERT_TRUE(touch_priority::kProbeWslButton.right <= 480);
  TEST_ASSERT_TRUE(touch_priority::kProbeWslButton.bottom <= 480);

  TEST_ASSERT_TRUE(touch_priority::kSend3CButton.left >= 0);
  TEST_ASSERT_TRUE(touch_priority::kSend3CButton.top >= 0);
  TEST_ASSERT_TRUE(touch_priority::kSend3CButton.right <= 480);
  TEST_ASSERT_TRUE(touch_priority::kSend3CButton.bottom <= 480);
}

void test_action_zones_are_separate_and_have_footer() {
  TEST_ASSERT_EQUAL_INT(20, touch_priority::kSend3CButton.left - touch_priority::kProbeWslButton.right);
  TEST_ASSERT_EQUAL_INT(20, 480 - touch_priority::kSend3CButton.bottom);
  TEST_ASSERT_TRUE(touch_priority::kProbeWslButton.right <= touch_priority::kSend3CButton.left);
}

void test_button_boundaries_are_half_open() {
  const int y = touch_priority::kActionTop + 1;

  TEST_ASSERT_TRUE(touch_priority::route(true, touch_priority::kProbeWslButton.left, y, false) == Route::ProbeWsl);
  TEST_ASSERT_TRUE(touch_priority::route(true, touch_priority::kProbeWslButton.right - 1, y, false) == Route::ProbeWsl);
  TEST_ASSERT_TRUE(touch_priority::route(true, touch_priority::kProbeWslButton.right, y, false) == Route::None);

  TEST_ASSERT_TRUE(touch_priority::route(true, touch_priority::kSend3CButton.left, y, false) == Route::Send3C);
  TEST_ASSERT_TRUE(touch_priority::route(true, touch_priority::kSend3CButton.right - 1, y, false) == Route::Send3C);
  TEST_ASSERT_TRUE(touch_priority::route(true, touch_priority::kSend3CButton.right, y, false) == Route::None);
}

void test_gap_and_footer_do_not_trigger_actions() {
  const int y = touch_priority::kActionTop + 1;
  TEST_ASSERT_TRUE(touch_priority::route(true, 240, y, false) == Route::None);
  TEST_ASSERT_TRUE(touch_priority::route(true, 240, touch_priority::kActionBottom, false) == Route::None);
}

void test_editor_has_priority_over_action_buttons() {
  const int y = touch_priority::kActionTop + 10;
  TEST_ASSERT_TRUE(touch_priority::route(true, 40, y, true) == Route::VirtualEditor);
  TEST_ASSERT_TRUE(touch_priority::route(true, 440, y, true) == Route::VirtualEditor);
}

void test_unclipped_button_label_budgets() {
  TEST_ASSERT_TRUE(touch_priority::kProbeWslLabelMaxWidth < touch_priority::kProbeWslButton.width());
  TEST_ASSERT_TRUE(touch_priority::kSend3CLabelMaxWidth < touch_priority::kSend3CButton.width());
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_action_zones_are_inside_480_square);
  RUN_TEST(test_action_zones_are_separate_and_have_footer);
  RUN_TEST(test_button_boundaries_are_half_open);
  RUN_TEST(test_gap_and_footer_do_not_trigger_actions);
  RUN_TEST(test_editor_has_priority_over_action_buttons);
  RUN_TEST(test_unclipped_button_label_budgets);
  UNITY_END();
}

void loop() {}

#include <stdint.h>

#include <unity.h>

#include "touch_key_feedback.h"

void test_first_press_is_accepted() {
  touch_key_feedback::Debouncer debouncer(70);
  TEST_ASSERT_TRUE(debouncer.press(3, 1000));
  TEST_ASSERT_TRUE(debouncer.isDown());
  TEST_ASSERT_EQUAL_INT(3, debouncer.activeKey());
}

void test_held_touch_is_not_repeated() {
  touch_key_feedback::Debouncer debouncer(70);
  TEST_ASSERT_TRUE(debouncer.press(3, 1000));
  TEST_ASSERT_FALSE(debouncer.press(3, 1200));
  TEST_ASSERT_FALSE(debouncer.press(4, 1200));
}

void test_repress_before_debounce_window_is_blocked() {
  touch_key_feedback::Debouncer debouncer(70);
  TEST_ASSERT_TRUE(debouncer.press(3, 1000));
  debouncer.release();
  TEST_ASSERT_FALSE(debouncer.press(3, 1069));
}

void test_repress_at_debounce_boundary_is_accepted() {
  touch_key_feedback::Debouncer debouncer(70);
  TEST_ASSERT_TRUE(debouncer.press(3, 1000));
  debouncer.release();
  TEST_ASSERT_TRUE(debouncer.press(3, 1070));
}

void test_different_key_is_still_blocked_during_quiet_interval() {
  touch_key_feedback::Debouncer debouncer(70);
  TEST_ASSERT_TRUE(debouncer.press(3, 1000));
  debouncer.release();
  TEST_ASSERT_FALSE(debouncer.press(4, 1060));
}

void test_millis_wraparound_does_not_break_debounce() {
  touch_key_feedback::Debouncer debouncer(70);
  const uint32_t start = 0xFFFFFFF0u;
  TEST_ASSERT_TRUE(debouncer.press(3, start));
  debouncer.release();
  TEST_ASSERT_FALSE(debouncer.press(3, 0x00000020u));
  debouncer.release();
  TEST_ASSERT_TRUE(debouncer.press(3, 0x00000036u));
}

void test_highlight_stays_active_until_expiry() {
  touch_key_feedback::Highlight highlight(90);
  highlight.press(7, 1000);
  TEST_ASSERT_TRUE(highlight.active(7, 1000));
  TEST_ASSERT_TRUE(highlight.active(7, 1089));
  TEST_ASSERT_FALSE(highlight.active(7, 1090));
  TEST_ASSERT_TRUE(highlight.expired(1090));
}

void test_new_highlight_replaces_previous_key() {
  touch_key_feedback::Highlight highlight(90);
  highlight.press(7, 1000);
  highlight.press(8, 1020);
  TEST_ASSERT_FALSE(highlight.active(7, 1020));
  TEST_ASSERT_TRUE(highlight.active(8, 1020));
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_first_press_is_accepted);
  RUN_TEST(test_held_touch_is_not_repeated);
  RUN_TEST(test_repress_before_debounce_window_is_blocked);
  RUN_TEST(test_repress_at_debounce_boundary_is_accepted);
  RUN_TEST(test_different_key_is_still_blocked_during_quiet_interval);
  RUN_TEST(test_millis_wraparound_does_not_break_debounce);
  RUN_TEST(test_highlight_stays_active_until_expiry);
  RUN_TEST(test_new_highlight_replaces_previous_key);
  UNITY_END();
}

void loop() {}

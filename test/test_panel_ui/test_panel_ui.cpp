#include <Arduino.h>
#include <unity.h>

#include "panel_ui.h"

void test_screen_geometry_is_480x480() {
  TEST_ASSERT_EQUAL_INT(480, panel_ui::kScreenWidth);
  TEST_ASSERT_EQUAL_INT(480, panel_ui::kScreenHeight);
}

void test_touch_targets_are_inside_screen_and_disjoint() {
  TEST_ASSERT_TRUE(panel_ui::kLeftButton.insideScreen());
  TEST_ASSERT_TRUE(panel_ui::kRightButton.insideScreen());
  TEST_ASSERT_EQUAL_INT(
    16, panel_ui::kRightButton.x -
        (panel_ui::kLeftButton.x + panel_ui::kLeftButton.width));
}

void test_left_target_maps_only_to_wsl() {
  TEST_ASSERT_EQUAL(
    panel_ui::TouchAction::ProbarWSL,
    panel_ui::actionForTouch(panel_ui::kLeftButton.x + 1,
                             panel_ui::kLeftButton.y + panel_ui::kLeftButton.height / 2));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::ProbarWSL,
                    panel_ui::actionForTouch(120, 455));
}

void test_right_target_maps_only_to_3c() {
  TEST_ASSERT_EQUAL(
    panel_ui::TouchAction::Enviar3C,
    panel_ui::actionForTouch(panel_ui::kRightButton.x + 1,
                             panel_ui::kRightButton.y + panel_ui::kRightButton.height / 2));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::Enviar3C,
                    panel_ui::actionForTouch(360, 455));
}

void test_gap_and_outside_area_do_not_trigger() {
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::None, panel_ui::actionForTouch(239, 400));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::None, panel_ui::actionForTouch(240, 400));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::None, panel_ui::actionForTouch(241, 400));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::None, panel_ui::actionForTouch(100, 351));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::None, panel_ui::actionForTouch(100, 456));
  TEST_ASSERT_EQUAL(panel_ui::TouchAction::None, panel_ui::actionForTouch(480, 400));
}

void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_screen_geometry_is_480x480);
  RUN_TEST(test_touch_targets_are_inside_screen_and_disjoint);
  RUN_TEST(test_left_target_maps_only_to_wsl);
  RUN_TEST(test_right_target_maps_only_to_3c);
  RUN_TEST(test_gap_and_outside_area_do_not_trigger);
  UNITY_END();
}

void loop() {}

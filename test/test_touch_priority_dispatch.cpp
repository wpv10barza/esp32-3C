#include <unity.h>

#include "touch_priority_dispatch.h"

using touch_priority::Policy;
using touch_priority::Route;

void test_no_touch_is_ignored() {
  const Policy policy{};
  TEST_ASSERT_EQUAL(static_cast<int>(Route::None),
                    static_cast<int>(touch_priority::route(false, 100, 400, policy)));
}

void test_normal_mode_routes_only_inside_buttons() {
  const Policy policy{};

  TEST_ASSERT_EQUAL(static_cast<int>(Route::ProbeWsl),
                    static_cast<int>(touch_priority::route(true, 20, 370, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::ProbeWsl),
                    static_cast<int>(touch_priority::route(true, 229, 451, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::None),
                    static_cast<int>(touch_priority::route(true, 230, 400, policy)));

  TEST_ASSERT_EQUAL(static_cast<int>(Route::Send3C),
                    static_cast<int>(touch_priority::route(true, 250, 370, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::Send3C),
                    static_cast<int>(touch_priority::route(true, 459, 451, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::None),
                    static_cast<int>(touch_priority::route(true, 460, 400, policy)));

  TEST_ASSERT_EQUAL(static_cast<int>(Route::None),
                    static_cast<int>(touch_priority::route(true, 240, 400, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::None),
                    static_cast<int>(touch_priority::route(true, 100, 369, policy)));
}

void test_editor_mode_has_absolute_priority() {
  Policy policy{};
  policy.editorActive = true;

  TEST_ASSERT_EQUAL(static_cast<int>(Route::VirtualEditor),
                    static_cast<int>(touch_priority::route(true, 30, 390, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::VirtualEditor),
                    static_cast<int>(touch_priority::route(true, 300, 390, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::VirtualEditor),
                    static_cast<int>(touch_priority::route(true, 240, 240, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::VirtualEditor),
                    static_cast<int>(touch_priority::route(true, 479, 479, policy)));
  TEST_ASSERT_EQUAL(static_cast<int>(Route::None),
                    static_cast<int>(touch_priority::route(false, 300, 390, policy)));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_no_touch_is_ignored);
  RUN_TEST(test_normal_mode_routes_only_inside_buttons);
  RUN_TEST(test_editor_mode_has_absolute_priority);
  return UNITY_END();
}

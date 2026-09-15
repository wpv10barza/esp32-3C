#include <unity.h>

#include "touch_hit_test.h"

using touch::KeyFrame;

void test_shared_vertical_boundary_is_not_double_triggered() {
  constexpr KeyFrame frames[] = {
    {0, 0, 100, 100},
    {100, 0, 100, 100},
  };

  TEST_ASSERT_EQUAL_size_t(0, touch::hitTest(frames, 2, 99, 50));
  TEST_ASSERT_EQUAL_size_t(1, touch::hitTest(frames, 2, 100, 50));
  TEST_ASSERT_EQUAL_size_t(1, touch::hitTest(frames, 2, 199, 50));
  TEST_ASSERT_EQUAL_size_t(touch::kNoKey, touch::hitTest(frames, 2, 200, 50));
}

void test_shared_horizontal_boundary_is_not_double_triggered() {
  constexpr KeyFrame frames[] = {
    {0, 0, 100, 50},
    {0, 50, 100, 50},
  };

  TEST_ASSERT_EQUAL_size_t(0, touch::hitTest(frames, 2, 50, 49));
  TEST_ASSERT_EQUAL_size_t(1, touch::hitTest(frames, 2, 50, 50));
  TEST_ASSERT_EQUAL_size_t(1, touch::hitTest(frames, 2, 50, 99));
  TEST_ASSERT_EQUAL_size_t(touch::kNoKey, touch::hitTest(frames, 2, 50, 100));
}

void test_outer_edges_are_half_open() {
  constexpr KeyFrame frame = {20, 30, 40, 50};

  TEST_ASSERT_FALSE(touch::contains(frame, 19, 30));
  TEST_ASSERT_TRUE(touch::contains(frame, 20, 30));
  TEST_ASSERT_TRUE(touch::contains(frame, 59, 79));
  TEST_ASSERT_FALSE(touch::contains(frame, 60, 79));
  TEST_ASSERT_FALSE(touch::contains(frame, 59, 80));
}

void test_overlapping_frames_fail_closed() {
  constexpr KeyFrame frames[] = {
    {0, 0, 100, 100},
    {50, 0, 100, 100},
  };

  TEST_ASSERT_EQUAL_size_t(touch::kNoKey, touch::hitTest(frames, 2, 75, 50));
  TEST_ASSERT_EQUAL_size_t(0, touch::hitTest(frames, 2, 25, 50));
  TEST_ASSERT_EQUAL_size_t(1, touch::hitTest(frames, 2, 125, 50));
}

void test_invalid_frame_never_matches() {
  constexpr KeyFrame frames[] = {
    {0, 0, 0, 20},
    {0, 0, 20, -1},
  };

  TEST_ASSERT_EQUAL_size_t(touch::kNoKey, touch::hitTest(frames, 2, 0, 0));
}

void test_null_and_empty_inputs_are_safe() {
  TEST_ASSERT_EQUAL_size_t(touch::kNoKey, touch::hitTest(nullptr, 0, 10, 10));
  constexpr KeyFrame frame = {0, 0, 20, 20};
  TEST_ASSERT_EQUAL_size_t(touch::kNoKey, touch::hitTest(&frame, 0, 10, 10));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_shared_vertical_boundary_is_not_double_triggered);
  RUN_TEST(test_shared_horizontal_boundary_is_not_double_triggered);
  RUN_TEST(test_outer_edges_are_half_open);
  RUN_TEST(test_overlapping_frames_fail_closed);
  RUN_TEST(test_invalid_frame_never_matches);
  RUN_TEST(test_null_and_empty_inputs_are_safe);
  return UNITY_END();
}

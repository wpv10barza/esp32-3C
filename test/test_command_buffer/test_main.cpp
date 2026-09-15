#include <unity.h>

#include "command_buffer.h"

using input::CommandBuffer;

void test_starts_empty() {
  CommandBuffer<8> buffer;
  TEST_ASSERT_TRUE(buffer.empty());
  TEST_ASSERT_EQUAL_UINT32(0, buffer.length());
  TEST_ASSERT_EQUAL_UINT32(0, buffer.cursor());
  TEST_ASSERT_EQUAL_STRING("", buffer.c_str());
}

void test_insert_appends_and_advances_cursor() {
  CommandBuffer<8> buffer;
  TEST_ASSERT_TRUE(buffer.insert('A'));
  TEST_ASSERT_TRUE(buffer.insert('B'));
  TEST_ASSERT_TRUE(buffer.insert('C'));
  TEST_ASSERT_EQUAL_STRING("ABC", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT32(3, buffer.length());
  TEST_ASSERT_EQUAL_UINT32(3, buffer.cursor());
}

void test_insert_middle_shifts_suffix() {
  CommandBuffer<8> buffer;
  buffer.assign("ACD");
  TEST_ASSERT_TRUE(buffer.moveCursorTo(1));
  TEST_ASSERT_TRUE(buffer.insert('B'));
  TEST_ASSERT_EQUAL_STRING("ABCD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT32(2, buffer.cursor());
}

void test_backspace_middle_shifts_suffix_left() {
  CommandBuffer<8> buffer;
  buffer.assign("ABCD");
  TEST_ASSERT_TRUE(buffer.moveCursorTo(3));
  TEST_ASSERT_TRUE(buffer.backspace());
  TEST_ASSERT_EQUAL_STRING("ABD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT32(2, buffer.cursor());
}

void test_erase_middle_keeps_cursor() {
  CommandBuffer<8> buffer;
  buffer.assign("ABCD");
  TEST_ASSERT_TRUE(buffer.moveCursorTo(1));
  TEST_ASSERT_TRUE(buffer.erase());
  TEST_ASSERT_EQUAL_STRING("ACD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT32(1, buffer.cursor());
}

void test_backspace_at_start_is_guarded() {
  CommandBuffer<8> buffer;
  buffer.assign("ABC");
  TEST_ASSERT_TRUE(buffer.moveCursorTo(0));
  TEST_ASSERT_FALSE(buffer.backspace());
  TEST_ASSERT_EQUAL_STRING("ABC", buffer.c_str());
}

void test_erase_at_end_is_guarded() {
  CommandBuffer<8> buffer;
  buffer.assign("ABC");
  TEST_ASSERT_FALSE(buffer.erase());
  TEST_ASSERT_EQUAL_STRING("ABC", buffer.c_str());
}

void test_insert_at_capacity_is_rejected_without_mutation() {
  CommandBuffer<5> buffer;
  buffer.assign("ABCD");
  TEST_ASSERT_TRUE(buffer.full());
  TEST_ASSERT_FALSE(buffer.insert('E'));
  TEST_ASSERT_EQUAL_STRING("ABCD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT32(4, buffer.length());
  TEST_ASSERT_EQUAL_UINT32(4, buffer.cursor());
}

void test_assign_truncates_and_null_terminates() {
  CommandBuffer<5> buffer;
  TEST_ASSERT_EQUAL_UINT32(4, buffer.assign("123456789"));
  TEST_ASSERT_EQUAL_STRING("1234", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT32(4, buffer.length());
  TEST_ASSERT_EQUAL_UINT32(4, buffer.cursor());
}

void test_assign_null_clears_buffer() {
  CommandBuffer<8> buffer;
  buffer.assign("ABC");
  TEST_ASSERT_EQUAL_UINT32(0, buffer.assign(nullptr));
  TEST_ASSERT_TRUE(buffer.empty());
  TEST_ASSERT_EQUAL_STRING("", buffer.c_str());
}

void setup() {}
void loop() {
  UNITY_BEGIN();
  RUN_TEST(test_starts_empty);
  RUN_TEST(test_insert_appends_and_advances_cursor);
  RUN_TEST(test_insert_middle_shifts_suffix);
  RUN_TEST(test_backspace_middle_shifts_suffix_left);
  RUN_TEST(test_erase_middle_keeps_cursor);
  RUN_TEST(test_backspace_at_start_is_guarded);
  RUN_TEST(test_erase_at_end_is_guarded);
  RUN_TEST(test_insert_at_capacity_is_rejected_without_mutation);
  RUN_TEST(test_assign_truncates_and_null_terminates);
  RUN_TEST(test_assign_null_clears_buffer);
  UNITY_END();
}

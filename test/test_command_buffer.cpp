#include <unity.h>

#include "command_buffer.h"
#include "command_edit_session.h"

void test_insert_at_middle_preserves_order() {
  CommandBuffer<32> buffer;
  TEST_ASSERT_TRUE(buffer.set("AC"));
  buffer.setCursor(1);

  TEST_ASSERT_TRUE(buffer.insert('B'));
  TEST_ASSERT_EQUAL_STRING("ABC", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(3, buffer.length());
  TEST_ASSERT_EQUAL_UINT(2, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_multi_character_insert_at_arbitrary_cursor() {
  CommandBuffer<32> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABEF"));
  buffer.setCursor(2);

  TEST_ASSERT_TRUE(buffer.insert("CD", 2));
  TEST_ASSERT_EQUAL_STRING("ABCDEF", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(4, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_backspace_removes_character_before_cursor() {
  CommandBuffer<32> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABCD"));
  buffer.setCursor(2);

  TEST_ASSERT_TRUE(buffer.backspace());
  TEST_ASSERT_EQUAL_STRING("ACD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(1, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_forward_delete_removes_character_at_cursor() {
  CommandBuffer<32> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABCD"));
  buffer.setCursor(1);

  TEST_ASSERT_TRUE(buffer.deleteForward());
  TEST_ASSERT_EQUAL_STRING("ACD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(1, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_cursor_navigation_is_clamped_to_text_bounds() {
  CommandBuffer<32> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABC"));

  buffer.moveHome();
  buffer.moveLeft();
  TEST_ASSERT_EQUAL_UINT(0, buffer.cursor());

  buffer.moveRight();
  buffer.moveRight();
  buffer.moveRight();
  buffer.moveRight();
  TEST_ASSERT_EQUAL_UINT(3, buffer.cursor());

  buffer.moveEnd();
  buffer.moveRight();
  TEST_ASSERT_EQUAL_UINT(3, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_set_cursor_clamps_and_editing_remains_valid() {
  CommandBuffer<32> buffer;
  TEST_ASSERT_TRUE(buffer.set("AB"));

  buffer.setCursor(999);
  TEST_ASSERT_EQUAL_UINT(2, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.insert('C'));
  TEST_ASSERT_EQUAL_STRING("ABC", buffer.c_str());

  buffer.setCursor(999);
  TEST_ASSERT_TRUE(buffer.backspace());
  TEST_ASSERT_EQUAL_STRING("AB", buffer.c_str());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_capacity_guard_does_not_corrupt_existing_text() {
  CommandBuffer<4> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABCD"));
  buffer.setCursor(2);

  TEST_ASSERT_FALSE(buffer.insert('X'));
  TEST_ASSERT_FALSE(buffer.insert("YZ"));
  TEST_ASSERT_EQUAL_STRING("ABCD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(4, buffer.length());
  TEST_ASSERT_EQUAL_UINT(2, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());

  TEST_ASSERT_TRUE(buffer.deleteForward());
  TEST_ASSERT_EQUAL_STRING("ABD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(2, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_delete_and_backspace_at_boundaries_are_noops() {
  CommandBuffer<16> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABC"));

  buffer.moveHome();
  TEST_ASSERT_FALSE(buffer.backspace());
  TEST_ASSERT_TRUE(buffer.deleteForward());
  TEST_ASSERT_EQUAL_STRING("BC", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(0, buffer.cursor());

  buffer.set("ABC");
  buffer.moveEnd();
  TEST_ASSERT_FALSE(buffer.deleteForward());
  TEST_ASSERT_TRUE(buffer.backspace());
  TEST_ASSERT_EQUAL_STRING("AB", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(2, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_failed_set_keeps_previous_valid_text() {
  CommandBuffer<4> buffer;
  TEST_ASSERT_TRUE(buffer.set("ABCD"));
  TEST_ASSERT_FALSE(buffer.set("ABCDE"));

  TEST_ASSERT_EQUAL_STRING("ABCD", buffer.c_str());
  TEST_ASSERT_EQUAL_UINT(4, buffer.length());
  TEST_ASSERT_EQUAL_UINT(4, buffer.cursor());
  TEST_ASSERT_TRUE(buffer.invariantHolds());
}

void test_cancel_discards_draft_and_keeps_committed_command() {
  CommandBuffer<32> commandBuffer;
  TEST_ASSERT_TRUE(commandBuffer.set("ORIGINAL"));
  CommandEditSession<32> session(commandBuffer);

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_TRUE(session.editing());
  TEST_ASSERT_TRUE(session.draft().set("CHANGED"));
  TEST_ASSERT_EQUAL_STRING("CHANGED", session.value());
  TEST_ASSERT_EQUAL_STRING("ORIGINAL", commandBuffer.c_str());

  TEST_ASSERT_TRUE(session.cancel());
  TEST_ASSERT_FALSE(session.editing());
  TEST_ASSERT_EQUAL_STRING("ORIGINAL", commandBuffer.c_str());
  TEST_ASSERT_EQUAL_STRING("ORIGINAL", session.value());
  TEST_ASSERT_TRUE(session.invariantHolds());
}

void test_ok_commits_draft_and_leaves_normal_state() {
  CommandBuffer<32> commandBuffer;
  TEST_ASSERT_TRUE(commandBuffer.set("ORIGINAL"));
  CommandEditSession<32> session(commandBuffer);

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_TRUE(session.draft().set("SAVED COMMAND"));
  TEST_ASSERT_EQUAL_STRING("SAVED COMMAND", session.value());

  TEST_ASSERT_TRUE(session.ok());
  TEST_ASSERT_FALSE(session.editing());
  TEST_ASSERT_EQUAL_STRING("SAVED COMMAND", commandBuffer.c_str());
  TEST_ASSERT_EQUAL_STRING("SAVED COMMAND", session.value());
  TEST_ASSERT_TRUE(session.invariantHolds());
}

void test_repeated_ok_or_cancel_cannot_mutate_after_transition() {
  CommandBuffer<32> commandBuffer;
  TEST_ASSERT_TRUE(commandBuffer.set("A"));
  CommandEditSession<32> session(commandBuffer);

  TEST_ASSERT_FALSE(session.ok());
  TEST_ASSERT_FALSE(session.cancel());
  TEST_ASSERT_EQUAL_STRING("A", commandBuffer.c_str());

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_TRUE(session.draft().set("B"));
  TEST_ASSERT_TRUE(session.ok());
  TEST_ASSERT_EQUAL_STRING("B", commandBuffer.c_str());
  TEST_ASSERT_FALSE(session.ok());
  TEST_ASSERT_FALSE(session.cancel());
  TEST_ASSERT_EQUAL_STRING("B", commandBuffer.c_str());
  TEST_ASSERT_TRUE(session.invariantHolds());
}

void test_second_edit_session_starts_from_last_committed_value() {
  CommandBuffer<32> commandBuffer;
  TEST_ASSERT_TRUE(commandBuffer.set("FIRST"));
  CommandEditSession<32> session(commandBuffer);

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_TRUE(session.draft().set("SECOND"));
  TEST_ASSERT_TRUE(session.cancel());
  TEST_ASSERT_EQUAL_STRING("FIRST", commandBuffer.c_str());

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_EQUAL_STRING("FIRST", session.draft().c_str());
  TEST_ASSERT_TRUE(session.draft().set("THIRD"));
  TEST_ASSERT_TRUE(session.ok());
  TEST_ASSERT_EQUAL_STRING("THIRD", commandBuffer.c_str());
  TEST_ASSERT_TRUE(session.invariantHolds());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_insert_at_middle_preserves_order);
  RUN_TEST(test_multi_character_insert_at_arbitrary_cursor);
  RUN_TEST(test_backspace_removes_character_before_cursor);
  RUN_TEST(test_forward_delete_removes_character_at_cursor);
  RUN_TEST(test_cursor_navigation_is_clamped_to_text_bounds);
  RUN_TEST(test_set_cursor_clamps_and_editing_remains_valid);
  RUN_TEST(test_capacity_guard_does_not_corrupt_existing_text);
  RUN_TEST(test_delete_and_backspace_at_boundaries_are_noops);
  RUN_TEST(test_failed_set_keeps_previous_valid_text);
  RUN_TEST(test_cancel_discards_draft_and_keeps_committed_command);
  RUN_TEST(test_ok_commits_draft_and_leaves_normal_state);
  RUN_TEST(test_repeated_ok_or_cancel_cannot_mutate_after_transition);
  RUN_TEST(test_second_edit_session_starts_from_last_committed_value);
  return UNITY_END();
}
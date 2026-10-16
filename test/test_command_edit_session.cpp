#include <unity.h>

#include "command_edit_session.h"

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

void test_ok_commits_draft_and_leaves_editing_mode() {
  CommandBuffer<32> commandBuffer;
  TEST_ASSERT_TRUE(commandBuffer.set("ORIGINAL"));
  CommandEditSession<32> session(commandBuffer);

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_TRUE(session.draft().set("SAVED COMMAND"));
  TEST_ASSERT_EQUAL_STRING("SAVED COMMAND", session.value());
  TEST_ASSERT_EQUAL_STRING("ORIGINAL", commandBuffer.c_str());

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

void test_reentry_while_editing_is_rejected_without_reset() {
  CommandBuffer<32> commandBuffer;
  TEST_ASSERT_TRUE(commandBuffer.set("BASE"));
  CommandEditSession<32> session(commandBuffer);

  TEST_ASSERT_TRUE(session.begin());
  TEST_ASSERT_TRUE(session.draft().set("IN PROGRESS"));
  TEST_ASSERT_FALSE(session.begin());
  TEST_ASSERT_TRUE(session.editing());
  TEST_ASSERT_EQUAL_STRING("IN PROGRESS", session.draft().c_str());
  TEST_ASSERT_EQUAL_STRING("BASE", commandBuffer.c_str());
  TEST_ASSERT_TRUE(session.cancel());
  TEST_ASSERT_EQUAL_STRING("BASE", commandBuffer.c_str());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_cancel_discards_draft_and_keeps_committed_command);
  RUN_TEST(test_ok_commits_draft_and_leaves_editing_mode);
  RUN_TEST(test_repeated_ok_or_cancel_cannot_mutate_after_transition);
  RUN_TEST(test_second_edit_session_starts_from_last_committed_value);
  RUN_TEST(test_reentry_while_editing_is_rejected_without_reset);
  return UNITY_END();
}

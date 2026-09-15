#include <unity.h>

#include "command_validation.h"

void test_null_command_is_rejected() {
  TEST_ASSERT_FALSE(command_validation::hasExecutableText(nullptr));
}

void test_empty_command_is_rejected() {
  TEST_ASSERT_FALSE(command_validation::hasExecutableText(""));
}

void test_spaces_only_command_is_rejected() {
  TEST_ASSERT_FALSE(command_validation::hasExecutableText("   "));
}

void test_all_ascii_whitespace_is_rejected() {
  TEST_ASSERT_FALSE(command_validation::hasExecutableText("\t\n\r\f\v"));
}

void test_leading_and_trailing_whitespace_does_not_hide_command() {
  TEST_ASSERT_TRUE(command_validation::hasExecutableText("  C3  "));
}

void test_single_punctuation_is_an_executable_nonblank_token() {
  TEST_ASSERT_TRUE(command_validation::hasExecutableText("!"));
}

void test_utf8_bytes_are_not_treated_as_ascii_whitespace() {
  TEST_ASSERT_TRUE(command_validation::hasExecutableText("á"));
}

void test_embedded_ascii_control_before_text_still_allows_command() {
  const char command[] = {'\n', '\t', 'X', '\0'};
  TEST_ASSERT_TRUE(command_validation::hasExecutableText(command));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_null_command_is_rejected);
  RUN_TEST(test_empty_command_is_rejected);
  RUN_TEST(test_spaces_only_command_is_rejected);
  RUN_TEST(test_all_ascii_whitespace_is_rejected);
  RUN_TEST(test_leading_and_trailing_whitespace_does_not_hide_command);
  RUN_TEST(test_single_punctuation_is_an_executable_nonblank_token);
  RUN_TEST(test_utf8_bytes_are_not_treated_as_ascii_whitespace);
  RUN_TEST(test_embedded_ascii_control_before_text_still_allows_command);
  return UNITY_END();
}

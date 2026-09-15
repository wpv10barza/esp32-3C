#pragma once

#include <string>

namespace command_ui {

enum class Mode {
  Normal,
  EditingCommand,
};

enum class EventType {
  BeginEditing,
  InputText,
  Backspace,
  Clear,
  CancelEditing,
  CommitEditing,
};

struct Event {
  EventType type;
  std::string text;

  static Event beginEditing() { return {EventType::BeginEditing, {}}; }
  static Event inputText(const std::string& value) { return {EventType::InputText, value}; }
  static Event backspace() { return {EventType::Backspace, {}}; }
  static Event clear() { return {EventType::Clear, {}}; }
  static Event cancelEditing() { return {EventType::CancelEditing, {}}; }
  static Event commitEditing() { return {EventType::CommitEditing, {}}; }
};

struct Transition {
  Mode mode;
  bool changed = false;
  bool commandReady = false;
};

class EditingCommandState {
 public:
  Mode mode() const { return mode_; }
  bool isEditing() const { return mode_ == Mode::EditingCommand; }
  const std::string& commandBuffer() const { return commandBuffer_; }

  // Starts editing from Normal mode. The existing buffer is preserved unless
  // an explicit seed is provided, which makes re-entry deterministic.
  Transition beginEditing(const std::string& seed = {}) {
    if (mode_ != Mode::Normal) return {mode_, false, false};
    if (!seed.empty()) commandBuffer_ = seed;
    mode_ = Mode::EditingCommand;
    return {mode_, true, false};
  }

  Transition handle(const Event& event) {
    switch (event.type) {
      case EventType::BeginEditing:
        return beginEditing();

      case EventType::InputText:
        if (!isEditing() || event.text.empty()) return {mode_, false, false};
        commandBuffer_ += event.text;
        return {mode_, true, false};

      case EventType::Backspace:
        if (!isEditing() || commandBuffer_.empty()) return {mode_, false, false};
        commandBuffer_.pop_back();
        return {mode_, true, false};

      case EventType::Clear:
        if (!isEditing() || commandBuffer_.empty()) return {mode_, false, false};
        commandBuffer_.clear();
        return {mode_, true, false};

      case EventType::CancelEditing:
        if (!isEditing()) return {mode_, false, false};
        mode_ = Mode::Normal;
        return {mode_, true, false};

      case EventType::CommitEditing:
        if (!isEditing() || commandBuffer_.empty()) return {mode_, false, false};
        mode_ = Mode::Normal;
        return {mode_, true, true};
    }

    return {mode_, false, false};
  }

 private:
  Mode mode_ = Mode::Normal;
  std::string commandBuffer_;
};

}  // namespace command_ui

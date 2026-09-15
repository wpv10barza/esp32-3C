#pragma once

#include <cstddef>

#include "command_edit_session.h"

enum class EditingCommandMode : unsigned char {
  Normal,
  EditingCommand,
};

template <std::size_t Capacity>
class EditingCommandState {
 public:
  using Buffer = CommandBuffer<Capacity>;

  explicit EditingCommandState(Buffer& committed) : session_(committed) {}

  EditingCommandMode mode() const {
    return session_.editing() ? EditingCommandMode::EditingCommand
                              : EditingCommandMode::Normal;
  }

  bool isEditing() const { return session_.editing(); }
  bool begin() { return session_.begin(); }
  bool ok() { return session_.ok(); }
  bool cancel() { return session_.cancel(); }
  bool invariantHolds() const { return session_.invariantHolds(); }
  CommandCommitRejection lastRejection() const { return session_.lastRejection(); }

  Buffer& draft() { return session_.draft(); }
  const Buffer& draft() const { return session_.draft(); }
  const char* value() const { return session_.value(); }

 private:
  CommandEditSession<Capacity> session_;
};

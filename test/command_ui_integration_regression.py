from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIG = (ROOT / "include" / "app_config.h").read_text(encoding="utf-8")
PANEL = (ROOT / "src" / "panel_4848s040_main.cpp").read_text(encoding="utf-8")
ESP_HI = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")
VKEY = (ROOT / "include" / "virtual_keyboard.h").read_text(encoding="utf-8")
FIELD = (ROOT / "include" / "command_field.h").read_text(encoding="utf-8")


def test_runtime_command_buffer_is_shared_contract():
    assert "class RuntimeCommandBuffer : public String" in CONFIG
    assert "RuntimeCommandBuffer commandBuffer(DEFAULT_3C_COMMAND_VALUE);" in CONFIG
    assert "static constexpr char defaultCommand[]" not in CONFIG
    assert "RuntimeCommandBuffer& defaultCommand = commandBuffer;" in CONFIG


def test_panel_uses_single_editor_buffer_and_syncs_runtime_command():
    assert '#include "command_buffer.h"' in PANEL
    assert '#include "command_text_viewport.h"' in PANEL
    assert '#include "command_field.h"' in PANEL
    assert '#include "editing_command_state.h"' in PANEL
    assert "Command commandBuffer;" in PANEL
    assert "EditingCommandState<kCommandCapacity> commandEditor(commandBuffer);" in PANEL
    assert "app_config::commandBuffer.set(commandBuffer.c_str())" in PANEL
    assert "send3CCommand(app_config::commandBuffer);" in PANEL
    assert "send3CCommand(app_config::defaultCommand);" not in PANEL
    assert "beginCommandEditing" in PANEL
    assert "commandEditor.ok()" in PANEL
    assert "commandEditor.cancel()" in PANEL


def test_other_firmware_target_still_uses_runtime_command_buffer():
    assert "send3CCommand(app_config::commandBuffer);" in ESP_HI
    assert "send3CCommand(app_config::defaultCommand);" not in ESP_HI


def test_shared_touch_keyboard_path_is_present():
    assert '#include "touch_priority_dispatch.h"' in PANEL
    assert '#include "virtual_keyboard.h"' in PANEL
    assert "touch_priority::route" in PANEL
    assert "virtual_keyboard::hitTest" in PANEL
    assert "constexpr int kRows = 4;" in VKEY
    assert "KeyKind::CursorLeft" in VKEY
    assert "KeyKind::CursorRight" in VKEY
    assert "KeyKind::Enter" in VKEY
    assert "kEditingBounds" in FIELD
    assert "kNormalBounds" in FIELD


if __name__ == "__main__":
    test_runtime_command_buffer_is_shared_contract()
    test_panel_uses_single_editor_buffer_and_syncs_runtime_command()
    test_other_firmware_target_still_uses_runtime_command_buffer()
    test_shared_touch_keyboard_path_is_present()
    print("command UI static integration checks: PASS")

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PANEL = (ROOT / "src" / "panel_4848s040_main.cpp").read_text(encoding="utf-8")
VKEY = (ROOT / "include" / "virtual_keyboard.h").read_text(encoding="utf-8")


def test_panel_uses_fixed_buffer_and_editing_state():
    assert '#include "command_buffer.h"' in PANEL
    assert '#include "editing_command_state.h"' in PANEL
    assert 'Command commandBuffer;' in PANEL
    assert 'EditingCommandState<kCommandCapacity> commandEditor(commandBuffer);' in PANEL
    assert 'send3CCommand(commandBuffer.c_str());' in PANEL
    assert 'send3CCommand(app_config::defaultCommand);' not in PANEL


def test_panel_uses_shared_touch_and_keyboard_layers():
    assert '#include "touch_priority_dispatch.h"' in PANEL
    assert '#include "virtual_keyboard.h"' in PANEL
    assert 'virtual_keyboard::hitTest' in PANEL
    assert 'touch_priority::route' in PANEL


def test_keyboard_has_one_control_row_and_four_rows_total():
    assert 'constexpr int kRows = 4;' in VKEY
    assert 'KeyKind::ToggleAlphaNumeric' in VKEY
    assert 'KeyKind::CursorLeft' in VKEY
    assert 'KeyKind::CursorRight' in VKEY
    assert 'KeyKind::Enter' in VKEY


if __name__ == "__main__":
    test_panel_uses_fixed_buffer_and_editing_state()
    test_panel_uses_shared_touch_and_keyboard_layers()
    test_keyboard_has_one_control_row_and_four_rows_total()
    print("command UI static integration checks: PASS")

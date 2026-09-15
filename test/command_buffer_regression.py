from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONFIG = (ROOT / "include" / "app_config.h").read_text(encoding="utf-8")
PANEL = (ROOT / "src" / "panel_4848s040_main.cpp").read_text(encoding="utf-8")
ESP_HI = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")


def test_command_buffer_is_runtime_source():
    assert "static String commandBuffer = DEFAULT_3C_COMMAND_VALUE;" in CONFIG
    assert "static constexpr char defaultCommand[]" not in CONFIG


def test_panel_send_path_consumes_runtime_buffer_directly():
    assert "send3CCommand(app_config::commandBuffer);" in PANEL
    assert "send3CCommand(app_config::defaultCommand);" not in PANEL
    assert 'send3CCommand("Cambia la tarea J10 a mensual");' not in PANEL


def test_esp_hi_send_path_consumes_runtime_buffer_directly():
    assert "send3CCommand(app_config::commandBuffer);" in ESP_HI
    assert "send3CCommand(app_config::defaultCommand);" not in ESP_HI
    assert 'send3CCommand("Cambia la tarea J10 a mensual");' not in ESP_HI


if __name__ == "__main__":
    test_command_buffer_is_runtime_source()
    test_panel_send_path_consumes_runtime_buffer_directly()
    test_esp_hi_send_path_consumes_runtime_buffer_directly()
    print("commandBuffer regression checks: PASS")

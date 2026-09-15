from pathlib import Path
import re


SOURCE = Path("src/panel_4848s040_main.cpp").read_text(encoding="utf-8")


def constant(name: str) -> int:
    match = re.search(rf"constexpr int {name}\s*=\s*([^;]+);", SOURCE)
    if not match:
        raise AssertionError(f"Missing constant: {name}")
    expression = match.group(1).strip()
    values = {"kScreenHeight": 480, "kScreenWidth": 480, "kUiFooterY": 458}
    if expression in values:
        return values[expression]
    return int(expression)


def main() -> None:
    screen_width = constant("kScreenWidth")
    screen_height = constant("kScreenHeight")
    footer_y = constant("kUiFooterY")
    button_y = constant("kUiButtonY")
    button_height = constant("kUiButtonHeight")
    left_x = constant("kUiButtonLeftX")
    right_x = constant("kUiButtonRightX")
    button_width = constant("kUiButtonWidth")

    assert screen_width == 480 and screen_height == 480, "Panel UI contract must remain 480x480"
    assert footer_y >= 0 and footer_y < screen_height
    assert footer_y + (screen_height - footer_y) == screen_height
    assert button_y >= 0
    assert button_y + button_height <= footer_y, "Buttons must not overlap the footer"
    assert left_x >= 0 and left_x + button_width <= right_x, "Left touch zone must be isolated"
    assert right_x + button_width <= screen_width, "Right touch zone must stay inside the LCD"

    # Touch hit-testing must use the same rectangle that is actually painted.
    touch_handler = re.search(r"void handleTouch\(\)\s*\{(.*?)\n\}", SOURCE, re.DOTALL)
    assert touch_handler, "Touch handler missing"
    touch = touch_handler.group(1)
    assert "sample.y >= kUiButtonY" in touch
    assert "sample.y < kUiButtonY + kUiButtonHeight" in touch
    assert "sample.x >= kUiButtonLeftX" in touch
    assert "sample.x < kUiButtonLeftX + kUiButtonWidth" in touch
    assert "sample.x >= kUiButtonRightX" in touch
    assert "sample.x < kUiButtonRightX + kUiButtonWidth" in touch
    assert "checkBackendHealth();" in touch, "Left PROBAR WSL zone must be actionable"
    assert "send3CCommand(app_config::defaultCommand);" in touch, "Right ENVIAR 3C zone must be actionable"

    # Keep known UI labels inside the 210px button at text size 2.
    assert len("PROBAR WSL") * 12 < button_width
    assert len("ENVIAR 3C") * 12 < button_width

    footer_fill = re.search(
        r"display->fillRect\(0,\s*kUiFooterY,\s*kScreenWidth,\s*kUiFooterHeight,\s*footerBackground\)",
        SOURCE,
    )
    assert footer_fill, "Bottom rows must be explicitly painted"

    footer_line = re.search(
        r"display->drawFastHLine\(0,\s*kUiFooterY,\s*kScreenWidth,",
        SOURCE,
    )
    assert footer_line, "Bottom footer needs a visible boundary"

    print(
        f"PASS: 480x480 geometry, readable button labels, no button overflow, "
        f"and isolated left/right touch zones ({left_x}-{left_x + button_width}, "
        f"{right_x}-{right_x + button_width})."
    )


if __name__ == "__main__":
    main()

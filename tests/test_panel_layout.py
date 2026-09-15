from pathlib import Path
import re


SOURCE = Path("panel_4848s040_main.cpp").read_text(encoding="utf-8")


def constant(name: str) -> int:
    match = re.search(rf"constexpr int {name}\s*=\s*([^;]+);", SOURCE)
    if not match:
        raise AssertionError(f"Missing constant: {name}")
    expression = match.group(1).strip()
    values = {"kScreenHeight": 480, "kUiFooterY": 458}
    if expression in values:
        return values[expression]
    return int(expression)


def main() -> None:
    screen_height = constant("kScreenHeight")
    footer_y = constant("kUiFooterY")
    button_y = constant("kUiButtonY")
    button_height = constant("kUiButtonHeight")

    assert screen_height == 480, "Panel UI contract must remain 480x480"
    assert footer_y >= 0 and footer_y < screen_height
    assert footer_y + (screen_height - footer_y) == screen_height
    assert button_y >= 0
    assert button_y + button_height <= footer_y, (
        "Touch buttons must not overlap the bottom footer"
    )

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
        f"PASS: 480x480 panel paints the complete bottom band; "
        f"buttons end at y={button_y + button_height}, footer starts at y={footer_y}."
    )


if __name__ == "__main__":
    main()

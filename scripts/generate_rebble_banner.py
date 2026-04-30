#!/usr/bin/env python3
"""Generate a minimal Rebble Appstore banner.

Usage:
  python scripts/generate_rebble_banner.py \
    --screenshot screenshots/almost-five.png \
    --output screenshots/rebble-banner-minimal.png
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont


def _load_fonts(scale: float) -> tuple[ImageFont.ImageFont, ImageFont.ImageFont, ImageFont.ImageFont]:
    title_size = max(18, int(76 * scale))
    subtitle_size = max(12, int(34 * scale))
    chip_size = max(10, int(24 * scale))
    try:
        title = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial Bold.ttf", title_size)
        subtitle = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial.ttf", subtitle_size)
        chip = ImageFont.truetype("/System/Library/Fonts/Supplemental/Arial.ttf", chip_size)
        return title, subtitle, chip
    except Exception:
        fallback = ImageFont.load_default()
        return fallback, fallback, fallback


def _chip(
    draw: ImageDraw.ImageDraw,
    x: int,
    y: int,
    text: str,
    font: ImageFont.ImageFont,
    color: tuple[int, int, int],
    scale: float,
) -> int:
    left, top, right, bottom = draw.textbbox((0, 0), text, font=font)
    text_w, text_h = right - left, bottom - top
    pad_x, pad_y = max(6, int(16 * scale)), max(4, int(10 * scale))
    width = int(text_w + pad_x * 2)
    height = int(text_h + pad_y * 2)
    draw.rounded_rectangle((x, y, x + width, y + height), radius=max(4, int(12 * scale)), outline=color, width=max(1, int(2 * scale)))
    draw.text((x + pad_x, y + pad_y - 1), text, fill=color, font=font)
    return x + width + max(4, int(14 * scale))


def generate_banner(screenshot: Path, output: Path, watch_scale: float, width: int, height: int) -> None:
    banner_w, banner_h = width, height
    ui_scale = min(banner_w / 1400.0, banner_h / 560.0)
    bg = (11, 11, 11)
    fg = (240, 240, 240)
    subtle = (205, 205, 205)
    accent = (127, 168, 201)
    divider = (55, 55, 55)
    watch_border = (120, 120, 120)

    title_font, subtitle_font, chip_font = _load_fonts(ui_scale)

    img = Image.new("RGB", (banner_w, banner_h), bg)
    draw = ImageDraw.Draw(img)

    x0, y0 = int(90 * ui_scale), int(110 * ui_scale)
    draw.text((x0, y0), "Almost Five", fill=fg, font=title_font)
    draw.text((x0, y0 + int(110 * ui_scale)), "Words-based time, built for clarity", fill=subtle, font=subtitle_font)

    chip_x = x0
    chip_y = y0 + int(190 * ui_scale)
    for label in ("Multi-platform", "Language-ready", "Low-noise UI"):
        chip_x = _chip(draw, chip_x, chip_y, label, chip_font, accent, ui_scale)

    divider_x = int(760 * ui_scale)
    draw.line(
        (divider_x, int(80 * ui_scale), divider_x, banner_h - int(80 * ui_scale)),
        fill=divider,
        width=max(1, int(2 * ui_scale)),
    )

    watch_x = int(930 * ui_scale)
    watch_y = int(75 * ui_scale)
    watch_w = int(330 * ui_scale)
    watch_h = int(410 * ui_scale)
    draw.rounded_rectangle(
        (watch_x, watch_y, watch_x + watch_w, watch_y + watch_h),
        radius=max(10, int(38 * ui_scale)),
        outline=watch_border,
        width=max(1, int(6 * ui_scale)),
    )

    screen_margin = int(26 * ui_scale)
    screen_box = (
        watch_x + screen_margin,
        watch_y + screen_margin,
        watch_x + watch_w - screen_margin,
        watch_y + watch_h - screen_margin,
    )
    draw.rounded_rectangle(screen_box, radius=max(6, int(22 * ui_scale)), fill=(0, 0, 0))

    shot = Image.open(screenshot).convert("RGB")

    # Preserve full screenshot height and width; only scale + center.
    # watch_scale enlarges from the padded baseline but is clamped to avoid cropping.
    inner_pad = int(16 * ui_scale)
    padded_w = screen_box[2] - screen_box[0] - inner_pad
    padded_h = screen_box[3] - screen_box[1] - inner_pad
    fit_ratio = min(padded_w / shot.width, padded_h / shot.height)
    max_safe_ratio = min(
        (screen_box[2] - screen_box[0]) / shot.width,
        (screen_box[3] - screen_box[1]) / shot.height,
    )
    ratio = min(fit_ratio * watch_scale, max_safe_ratio)
    new_size = (int(shot.width * ratio), int(shot.height * ratio))
    resized = shot.resize(new_size, Image.Resampling.NEAREST)
    screen_w = screen_box[2] - screen_box[0]
    screen_h = screen_box[3] - screen_box[1]
    px = screen_box[0] + (screen_w - new_size[0]) // 2
    py = screen_box[1] + (screen_h - new_size[1]) // 2
    img.paste(resized, (px, py))

    output.parent.mkdir(parents=True, exist_ok=True)
    img.save(output)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate minimal Rebble banner.")
    parser.add_argument(
        "--screenshot",
        type=Path,
        default=Path("screenshots/almost-five.png"),
        help="Input watch screenshot PNG.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("screenshots/rebble-banner-minimal.png"),
        help="Output banner PNG path.",
    )
    parser.add_argument(
        "--watch-scale",
        type=float,
        default=1.0,
        help="Scale factor for the watch screenshot inside the frame (e.g. 1.2 for 20%% larger).",
    )
    parser.add_argument("--width", type=int, default=720, help="Banner width in pixels.")
    parser.add_argument("--height", type=int, default=320, help="Banner height in pixels.")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    generate_banner(args.screenshot, args.output, args.watch_scale, args.width, args.height)
    print(f"Saved {args.output}")


if __name__ == "__main__":
    main()

#!/usr/bin/env python3
"""Generate Rebble icon PNGs from a watch screenshot.

Default behavior removes the bottom date line by detecting a large vertical
gap between the main time text and lower metadata text.
"""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image


def _row_runs_with_ink(img: Image.Image, threshold: int = 16) -> list[tuple[int, int]]:
    gray = img.convert("L")
    width, height = gray.size
    px = gray.load()

    rows = []
    for y in range(height):
        has_ink = False
        for x in range(width):
            if px[x, y] > threshold:
                has_ink = True
                break
        rows.append(has_ink)

    runs: list[tuple[int, int]] = []
    start = None
    for y, has_ink in enumerate(rows):
        if has_ink and start is None:
            start = y
        elif not has_ink and start is not None:
            runs.append((start, y - 1))
            start = None
    if start is not None:
        runs.append((start, height - 1))
    return runs


def _crop_without_date(img: Image.Image, gap_threshold: int = 18, pad_top: int = 6, pad_bottom: int = 6) -> Image.Image:
    runs = _row_runs_with_ink(img)
    if not runs:
        return img

    keep_end = runs[0][1]
    for idx in range(len(runs) - 1):
        keep_end = runs[idx][1]
        gap = runs[idx + 1][0] - runs[idx][1] - 1
        if gap >= gap_threshold:
            break
    else:
        keep_end = runs[-1][1]

    top = max(0, runs[0][0] - pad_top)
    bottom = min(img.height, keep_end + pad_bottom + 1)
    return img.crop((0, top, img.width, bottom))


def _make_square_icon(img: Image.Image, size: int, fill: tuple[int, int, int] = (0, 0, 0), padding: int = 4) -> Image.Image:
    canvas = Image.new("RGB", (size, size), fill)
    max_w = size - padding * 2
    max_h = size - padding * 2
    ratio = min(max_w / img.width, max_h / img.height)
    new_size = (max(1, int(img.width * ratio)), max(1, int(img.height * ratio)))
    scaled = img.resize(new_size, Image.Resampling.NEAREST)
    x = (size - new_size[0]) // 2
    y = (size - new_size[1]) // 2
    canvas.paste(scaled, (x, y))
    return canvas


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate 80x80 and 144x144 Rebble icon PNGs.")
    parser.add_argument("--input", type=Path, default=Path("screenshots/almost-five.png"), help="Input screenshot PNG.")
    parser.add_argument("--small-output", type=Path, default=Path("screenshots/rebble-icon-small-80.png"))
    parser.add_argument("--large-output", type=Path, default=Path("screenshots/rebble-icon-large-144.png"))
    parser.add_argument("--gap-threshold", type=int, default=18, help="Rows gap considered the split before date text.")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    src = Image.open(args.input).convert("RGB")
    cropped = _crop_without_date(src, gap_threshold=args.gap_threshold)

    small = _make_square_icon(cropped, 80)
    large = _make_square_icon(cropped, 144)

    args.small_output.parent.mkdir(parents=True, exist_ok=True)
    args.large_output.parent.mkdir(parents=True, exist_ok=True)
    small.save(args.small_output)
    large.save(args.large_output)

    print(f"Saved {args.small_output}")
    print(f"Saved {args.large_output}")


if __name__ == "__main__":
    main()

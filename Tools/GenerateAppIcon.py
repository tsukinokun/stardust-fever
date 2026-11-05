"""exe とウィンドウのアイコン（StardustFever/Assets/Icon/App.ico）を作る。

    python Tools/GenerateAppIcon.py

絵柄は「金の星と夜空」。紫〜紺の星雲の角丸の四角に、玉と同じ金色に光る五芒星を置き、周りに星屑を散らす。
大きさごとに4倍で描いてから縮める。32 以下は細かい星屑と星雲を省き、星を大きく、輪郭をはっきりさせる。
premake5.lua の tsukino_icon() が exe に埋め込み、Window がタイトルバーとタスクバーにも使う。
"""
import math
import os
import random

from PIL import Image, ImageChops, ImageDraw, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUTPUT_DIR = os.path.join(ROOT, "StardustFever", "Assets", "Icon")
SIZES = [256, 128, 64, 48, 32, 24, 16]
SUPERSAMPLE = 4

SKY_INNER = (88, 46, 150)    # 中心の紫
SKY_OUTER = (8, 10, 38)      # 縁の紺
RIM = (170, 110, 255)        # 縁の光
GOLD_LIGHT = (255, 236, 160)
GOLD_DARK = (232, 150, 40)
GOLD_EDGE = (150, 80, 10)


def solid(size, color, alpha=255):
    return Image.new("RGBA", (size, size), color + (alpha,))


def rounded_mask(size, radius):
    mask = Image.new("L", (size, size), 0)
    ImageDraw.Draw(mask).rounded_rectangle([0, 0, size - 1, size - 1], radius=radius, fill=255)
    return mask


def star_points(cx, cy, outer, inner):
    points = []
    for i in range(10):
        angle = -math.pi / 2.0 + i * math.pi / 5.0  # 先端を真上から
        radius = outer if i % 2 == 0 else inner
        points.append((cx + radius * math.cos(angle), cy + radius * math.sin(angle)))
    return points


def draw_icon(size):
    simple = size <= 32
    s = size * SUPERSAMPLE
    rng = random.Random(7)

    #-------------------------------------------------------------
    # 夜空: 中心が紫、縁が紺の放射グラデーション
    #-------------------------------------------------------------
    gradient = Image.radial_gradient("L").resize((s, s), Image.BILINEAR)  # 中心 0 → 縁 255
    gradient = gradient.point(lambda v: min(255, int(v * 1.25)))
    sky = Image.composite(solid(s, SKY_OUTER), solid(s, SKY_INNER), gradient)

    if not simple:
        # 星雲のもや（ぼかした楕円をいくつか加算気味に重ねる）
        haze = Image.new("RGBA", (s, s), (0, 0, 0, 0))
        draw = ImageDraw.Draw(haze)
        for color in [(200, 60, 200), (60, 110, 230), (150, 70, 240)]:
            x, y = rng.uniform(0.15, 0.85) * s, rng.uniform(0.15, 0.85) * s
            rx, ry = rng.uniform(0.25, 0.4) * s, rng.uniform(0.12, 0.25) * s
            draw.ellipse([x - rx, y - ry, x + rx, y + ry], fill=color + (70,))
        haze = haze.filter(ImageFilter.GaussianBlur(s * 0.08))
        sky = Image.alpha_composite(sky, haze)

        # 遠い星（点）
        dots = ImageDraw.Draw(sky)
        for _ in range(28):
            x, y = rng.uniform(0.08, 0.92) * s, rng.uniform(0.08, 0.92) * s
            r = rng.uniform(0.004, 0.009) * s
            v = rng.randint(170, 255)
            dots.ellipse([x - r, y - r, x + r, y + r], fill=(v, v, 255, 255))

    #-------------------------------------------------------------
    # 金の五芒星: 外側の光 → 山ごとに左右で明暗を変えた面 → 細い縁
    #-------------------------------------------------------------
    cx, cy = s * 0.5, s * 0.53
    outer = s * (0.44 if simple else 0.36)
    inner = outer * 0.44
    points = star_points(cx, cy, outer, inner)

    glow = Image.new("RGBA", (s, s), (0, 0, 0, 0))
    ImageDraw.Draw(glow).polygon(star_points(cx, cy, outer * 1.08, inner * 1.1), fill=(255, 200, 90, 200))
    glow = glow.filter(ImageFilter.GaussianBlur(s * (0.035 if simple else 0.06)))
    sky = Image.alpha_composite(sky, glow)

    star = Image.new("RGBA", (s, s), (0, 0, 0, 0))
    draw = ImageDraw.Draw(star)
    for i in range(10):
        a, b = points[i], points[(i + 1) % 10]
        # 先端から谷へ向かう面（i が偶数）を明るく、谷から先端へ向かう面を暗くして立体に見せる
        color = GOLD_LIGHT if i % 2 == 0 else GOLD_DARK
        draw.polygon([(cx, cy), a, b], fill=color + (255,))
    draw.polygon(points, outline=GOLD_EDGE + (255,), width=max(1, int(s * (0.018 if simple else 0.008))))
    sky = Image.alpha_composite(sky, star)

    if not simple:
        # 星屑のきらめき（十字）
        sparkle = ImageDraw.Draw(sky)
        for x, y, length in [(0.2, 0.22, 0.07), (0.8, 0.2, 0.05), (0.82, 0.8, 0.06), (0.17, 0.78, 0.04)]:
            px, py, l = x * s, y * s, length * s
            w = max(1, int(s * 0.006))
            sparkle.line([(px - l, py), (px + l, py)], fill=(255, 245, 210, 255), width=w)
            sparkle.line([(px, py - l), (px, py + l)], fill=(255, 245, 210, 255), width=w)
            sparkle.ellipse([px - w * 1.5, py - w * 1.5, px + w * 1.5, py + w * 1.5], fill=(255, 255, 255, 255))

    #-------------------------------------------------------------
    # 角丸の四角に切り抜き、縁に細い紫の光を入れる
    #-------------------------------------------------------------
    radius = int(s * 0.2)
    rim = Image.new("RGBA", (s, s), (0, 0, 0, 0))
    ImageDraw.Draw(rim).rounded_rectangle([0, 0, s - 1, s - 1], radius=radius, outline=RIM + (255,), width=max(2, int(s * 0.02)))
    rim = rim.filter(ImageFilter.GaussianBlur(s * 0.006))
    sky = Image.alpha_composite(sky, rim)
    mask = rounded_mask(s, radius)
    sky.putalpha(ImageChops.multiply(sky.getchannel("A"), mask))

    return sky.resize((size, size), Image.LANCZOS)


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    images = {size: draw_icon(size) for size in SIZES}
    images[256].save(os.path.join(OUTPUT_DIR, "App.png"))
    images[256].save(os.path.join(OUTPUT_DIR, "App.ico"), format="ICO",
                     sizes=[(size, size) for size in SIZES],
                     append_images=[images[size] for size in SIZES if size != 256])
    print(f"wrote {os.path.join(OUTPUT_DIR, 'App.ico')} ({', '.join(str(size) for size in SIZES)})")


if __name__ == "__main__":
    main()

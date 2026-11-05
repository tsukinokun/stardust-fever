"""HUD の素材（StardustFever/Assets/Textures/UI/*.png）を生成する。

どれも白で描き、色は SpriteComponent::tintColor で付ける。
同じ形を色違いで使い回すことで、素材を増やさずに見た目の強弱を付けられる。

    python Tools/GenerateUiTextures.py

9スライスで伸ばす素材（panel_fill / panel_frame / pill）は、角の半径を sliceBorder に合わせてある。
"""
import math
import os

from PIL import Image, ImageDraw, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "StardustFever", "Assets", "Textures", "UI")

# 高解像度で描いてから縮め、縁のギザギザを消す
SUPERSAMPLE = 4


def supersampled(size, draw_fn):
    w, h = size
    big = Image.new("RGBA", (w * SUPERSAMPLE, h * SUPERSAMPLE), (255, 255, 255, 0))
    draw_fn(ImageDraw.Draw(big), SUPERSAMPLE)
    return big.resize((w, h), Image.LANCZOS)


def save(image, name):
    os.makedirs(OUT, exist_ok=True)
    image.save(os.path.join(OUT, name))
    print("wrote", name, image.size)


def panel_fill():
    """角丸の板。上が少し明るい（光が上から当たっているように見せる）。sliceBorder 24。"""
    size = 96
    radius = 22

    def draw(d, s):
        d.rounded_rectangle([0, 0, size * s - 1, size * s - 1], radius=radius * s, fill=(255, 255, 255, 255))

    shape = supersampled((size, size), draw)
    # 縦のグラデーションを α に掛ける（上 1.0 → 下 0.82）
    pixels = shape.load()
    for y in range(size):
        k = 1.0 - 0.18 * (y / (size - 1))
        for x in range(size):
            r, g, b, a = pixels[x, y]
            pixels[x, y] = (r, g, b, int(a * k))
    return shape


def panel_frame():
    """角丸の縁取り。外側へ少しにじむ光を付ける。sliceBorder 24。"""
    size = 96
    radius = 22

    def draw(d, s):
        d.rounded_rectangle([3 * s, 3 * s, (size - 3) * s - 1, (size - 3) * s - 1], radius=(radius - 3) * s, outline=(255, 255, 255, 255), width=2 * s)

    line = supersampled((size, size), draw)
    glow = line.filter(ImageFilter.GaussianBlur(3))
    return Image.alpha_composite(glow, line)


def pill():
    """両端が丸い帯（ピル・キーのバッジ・ゲージの背景）。sliceBorder 16。"""
    w, h = 64, 32

    def draw(d, s):
        d.rounded_rectangle([0, 0, w * s - 1, h * s - 1], radius=(h // 2) * s, fill=(255, 255, 255, 255))

    return supersampled((w, h), draw)


def bar_fill():
    """ゲージの中身。上半分に光の筋を入れて立体に見せる。横に伸ばして使う。"""
    w, h = 64, 24
    image = Image.new("RGBA", (w, h))
    pixels = image.load()
    for y in range(h):
        t = y / (h - 1)
        # 上 1/3 に明るい筋、下へ向かって少し暗く
        shine = 1.0 if t < 0.35 else 0.78 - 0.18 * (t - 0.35) / 0.65
        for x in range(w):
            v = int(255 * shine)
            pixels[x, y] = (v, v, v, 255)
    return image


def glow():
    """柔らかい光の点（粒子・保留の玉・数字の後ろの光）。"""
    size = 64
    image = Image.new("RGBA", (size, size))
    pixels = image.load()
    c = (size - 1) / 2.0
    for y in range(size):
        for x in range(size):
            d = math.hypot(x - c, y - c) / c
            a = max(0.0, 1.0 - d) ** 2.2
            pixels[x, y] = (255, 255, 255, int(255 * a))
    return image


def star_points(cx, cy, outer, inner, rotation=-math.pi / 2):
    points = []
    for i in range(10):
        r = outer if i % 2 == 0 else inner
        a = rotation + i * math.pi / 5
        points.append((cx + r * math.cos(a), cy + r * math.sin(a)))
    return points


def star(filled=True):
    """星のアイコン。filled=False は未発見用の輪郭。"""
    size = 64

    def draw(d, s):
        pts = star_points(size * s / 2, size * s / 2 + 2 * s, 29 * s, 12 * s)
        if filled:
            d.polygon(pts, fill=(255, 255, 255, 255))
        else:
            d.line(pts + [pts[0]], fill=(255, 255, 255, 255), width=3 * s, joint="curve")

    shape = supersampled((size, size), draw)
    halo = shape.filter(ImageFilter.GaussianBlur(4))
    return Image.alpha_composite(halo, shape)


def white():
    """一色の板（閃光・区切り線）。"""
    return Image.new("RGBA", (8, 8), (255, 255, 255, 255))


def main():
    save(panel_fill(), "panel_fill.png")
    save(panel_frame(), "panel_frame.png")
    save(pill(), "pill.png")
    save(bar_fill(), "bar_fill.png")
    save(glow(), "glow.png")
    save(star(True), "star.png")
    save(star(False), "star_outline.png")
    save(white(), "white.png")


if __name__ == "__main__":
    main()

"""盤面の宇宙の素材（StardustFever/Assets/Textures/Space/*.png）を生成する。

どれも加算のワールドスプライト（SpriteComponent の blendMode Additive）で使う前提。
黒（α 0）は何も足さないので、明るいところだけが光って見える。

    python Tools/GenerateSpaceTextures.py

乱数の種は固定してあり、何度実行しても同じ画像になる。
"""
import math
import os
import random

from PIL import Image, ImageChops, ImageDraw, ImageFilter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "StardustFever", "Assets", "Textures", "Space")


def save(image, name):
    os.makedirs(OUT, exist_ok=True)
    image.save(os.path.join(OUT, name))
    print("wrote", name, image.size)


def fractal_noise(size, seed, octaves=6):
    """小さな乱数の画像を拡大して重ね、雲のような濃淡を作る（L、0〜255）。"""
    rng = random.Random(seed)
    result = None
    total = 0.0
    for octave in range(octaves):
        cells = 2 ** (octave + 2)
        small = Image.new("L", (cells, cells))
        small.putdata([rng.randint(0, 255) for _ in range(cells * cells)])
        layer = small.resize((size, size), Image.BICUBIC).filter(ImageFilter.GaussianBlur(size / cells / 2.5))
        weight = 0.55**octave
        # 重み付きの平均を順に取る（L のまま足すと 255 で頭打ちになるため）
        result = layer if result is None else Image.blend(result, layer, weight / (total + weight))
        total += weight
    # 平均を取ると濃淡が真ん中に寄るので、幅を広げ直す
    return result.point(lambda v: max(0, min(255, int((v - 128) * 2.2 + 128))))


def radial_falloff(size, power):
    """中心が 1、縁が 0 になる丸いぼかし（L）。星雲の端を消して、四角い板に見せない。"""
    image = Image.new("L", (size, size))
    half = (size - 1) / 2.0
    image.putdata(
        [
            int(255 * max(0.0, 1.0 - math.hypot((x - half) / half, (y - half) / half)) ** power)
            for y in range(size)
            for x in range(size)
        ]
    )
    return image


def nebula(seed, inner, outer):
    """星雲。濃いところを inner の色、薄いところを outer の色にし、α は雲の濃さ × 丸いぼかし。"""
    size = 512
    cloud = fractal_noise(size, seed)
    # 濃淡を強める（薄い霧は消し、濃いところだけ残す）
    cloud = cloud.point(lambda v: int(255 * min(1.0, max(0.0, (v - 105) / 110.0)) ** 1.6))
    alpha = ImageChops.multiply(cloud, radial_falloff(size, 1.4))

    detail = fractal_noise(size, seed + 101, octaves=5)
    image = Image.new("RGBA", (size, size))
    pixels = []
    for c, d in zip(alpha.tobytes(), detail.tobytes()):
        t = d / 255.0
        r = int(inner[0] * t + outer[0] * (1 - t))
        g = int(inner[1] * t + outer[1] * (1 - t))
        b = int(inner[2] * t + outer[2] * (1 - t))
        pixels.append((r, g, b, c))
    image.putdata(pixels)
    return image.filter(ImageFilter.GaussianBlur(1.5))


def starfield():
    """遠い星空。小さな点を明るさと色をばらつかせて散らし、ごく一部だけ大きく光らせる。"""
    size = 1024
    rng = random.Random(7)
    image = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    d = ImageDraw.Draw(image)
    tints = [(255, 255, 255), (200, 220, 255), (255, 236, 200), (255, 210, 190), (190, 210, 255)]
    for _ in range(1400):
        x, y = rng.uniform(0, size), rng.uniform(0, size)
        brightness = rng.random() ** 2.6
        radius = 0.6 + 1.2 * brightness
        r, g, b = rng.choice(tints)
        d.ellipse([x - radius, y - radius, x + radius, y + radius], fill=(r, g, b, int(60 + 195 * brightness)))
    glow = image.filter(ImageFilter.GaussianBlur(2.5))
    return Image.alpha_composite(glow, image)


def flare():
    """星の光芒。丸い光に、十字の細い光を重ねる（白。色は tint で付ける）。"""
    size = 256
    half = size / 2.0
    image = Image.new("L", (size, size))
    values = []
    for y in range(size):
        for x in range(size):
            dx, dy = (x - half) / half, (y - half) / half
            dist = math.hypot(dx, dy)
            core = math.exp(-dist * dist * 40.0)
            halo = math.exp(-dist * 5.0) * 0.45
            cross = (math.exp(-abs(dy) * 90.0) * max(0.0, 1.0 - abs(dx)) ** 2 + math.exp(-abs(dx) * 90.0) * max(0.0, 1.0 - abs(dy)) ** 2) * 0.8
            values.append(int(255 * min(1.0, core + halo + cross)))
    image.putdata(values)
    white = Image.new("RGBA", (size, size), (255, 255, 255, 0))
    white.putalpha(image)
    return white


def sparkle():
    """粒子用の小さな四芒星。玉の尾と釘の火花に使う。"""
    size = 64
    half = size / 2.0
    image = Image.new("L", (size, size))
    values = []
    for y in range(size):
        for x in range(size):
            dx, dy = (x - half + 0.5) / half, (y - half + 0.5) / half
            dist = math.hypot(dx, dy)
            core = math.exp(-dist * dist * 18.0)
            cross = math.exp(-abs(dy) * 22.0) * max(0.0, 1.0 - abs(dx)) ** 1.5 + math.exp(-abs(dx) * 22.0) * max(0.0, 1.0 - abs(dy)) ** 1.5
            values.append(int(255 * min(1.0, core + cross * 0.7)))
    image.putdata(values)
    white = Image.new("RGBA", (size, size), (255, 255, 255, 0))
    white.putalpha(image)
    return white


def streak():
    """流れ星。左下が頭（明るい）、右上へ尾が細く消えていく斜めの筋。
    ワールドスプライトはカメラを向いて回らないので、向きを画像に描き込み、左下へ動かして使う。"""
    size = 256
    image = Image.new("L", (size, size))
    values = []
    head = (size * 0.12, size * 0.88)
    tail = (size * 0.95, size * 0.05)
    lx, ly = tail[0] - head[0], tail[1] - head[1]
    length = math.hypot(lx, ly)
    ux, uy = lx / length, ly / length
    for y in range(size):
        for x in range(size):
            px, py = x - head[0], y - head[1]
            along = (px * ux + py * uy) / length
            across = abs(px * -uy + py * ux)
            if along < -0.05 or along > 1.0:
                values.append(0)
                continue
            fade = max(0.0, 1.0 - max(along, 0.0)) ** 2.2
            width = 1.2 + 3.0 * fade
            line = math.exp(-((across / width) ** 2)) * fade
            headGlow = math.exp(-(px * px + py * py) / 60.0)
            values.append(int(255 * min(1.0, line + headGlow)))
    image.putdata(values)
    white = Image.new("RGBA", (size, size), (255, 255, 255, 0))
    white.putalpha(image.filter(ImageFilter.GaussianBlur(0.8)))
    return white


def main():
    save(nebula(11, (150, 90, 255), (40, 30, 120)), "nebula_violet.png")
    save(nebula(23, (90, 200, 255), (20, 50, 140)), "nebula_blue.png")
    save(starfield(), "starfield.png")
    save(flare(), "flare.png")
    save(sparkle(), "sparkle.png")
    save(streak(), "streak.png")


if __name__ == "__main__":
    main()

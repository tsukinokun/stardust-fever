"""ビッグバンで手に入る天体の絵（StardustFever/Assets/Textures/SkyObjects/*.png）を作る。

    python Tools/GenerateSkyObjectTextures.py

写真は使わず、雲の濃淡と点の散らばりから描く。乱数の種は固定してあり、何度実行しても
同じ画像になる。α は明るさと同じにしてあるので、加算でも半透明でも暗い背景に馴染む。
照準の絵（Textures/UI/reticle.png）もここで作る。
"""
import math
import os
import random
import sys

from PIL import Image, ImageChops, ImageDraw, ImageFilter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from GenerateSpaceTextures import fractal_noise, radial_falloff  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "StardustFever", "Assets", "Textures", "SkyObjects")
UI_OUT = os.path.join(ROOT, "StardustFever", "Assets", "Textures", "UI")
SIZE = 256
HALF = SIZE / 2.0


def black():
    return Image.new("RGB", (SIZE, SIZE))


def tint(mask, color):
    """濃さ（L）に色を掛ける。"""
    return Image.merge("RGB", [mask.point(lambda v, c=c: v * c // 255) for c in color])


def add(*layers):
    result = layers[0]
    for layer in layers[1:]:
        result = ImageChops.add(result, layer)
    return result


def contrast(mask, low, high, power=1.0):
    return mask.point(lambda v: int(255 * min(1.0, max(0.0, (v - low) / float(high - low))) ** power))


def shape(draw_function, blur):
    """図形を描いてぼかした濃さ（L）を作る。"""
    mask = Image.new("L", (SIZE, SIZE))
    draw_function(ImageDraw.Draw(mask))
    return mask.filter(ImageFilter.GaussianBlur(blur))


def ellipse_mask(cx, cy, rx, ry, blur):
    return shape(lambda d: d.ellipse((cx - rx, cy - ry, cx + rx, cy + ry), fill=255), blur)


def cloud(seed, low=100, high=210, power=1.3):
    return contrast(fractal_noise(SIZE, seed), low, high, power)


def gas(seed, mask, color, low=100, high=210):
    return tint(ImageChops.multiply(cloud(seed, low, high), mask), color)


def point(image, x, y, radius, color):
    """やわらかい光の点を足す。"""
    size = int(radius * 8) + 2
    layer = Image.new("L", (SIZE, SIZE))
    ImageDraw.Draw(layer).ellipse((x - radius, y - radius, x + radius, y + radius), fill=255)
    box = (max(0, int(x) - size), max(0, int(y) - size), min(SIZE, int(x) + size), min(SIZE, int(y) + size))
    halo = layer.crop(box).filter(ImageFilter.GaussianBlur(radius * 1.2))
    sharp = layer.crop(box).filter(ImageFilter.GaussianBlur(max(0.5, radius * 0.35)))
    patch = ImageChops.add(tint(sharp, color), tint(halo, color))
    region = image.crop(box)
    image.paste(ImageChops.add(region, patch), box)


def stars(image, rng, count, spread, color, cx=HALF, cy=HALF, smallest=0.5, largest=1.3, gaussian=True):
    for _ in range(count):
        if gaussian:
            x, y = rng.gauss(cx, spread), rng.gauss(cy, spread)
        else:
            angle, distance = rng.uniform(0, math.tau), spread * math.sqrt(rng.random())
            x, y = cx + math.cos(angle) * distance, cy + math.sin(angle) * distance
        if 4 < x < SIZE - 4 and 4 < y < SIZE - 4:
            shade = rng.uniform(0.45, 1.0)
            point(image, x, y, rng.uniform(smallest, largest), tuple(int(c * shade) for c in color))


def fade_edge(image):
    """四隅へ向かって消し、四角い板に見えないようにする。"""
    edge = radial_falloff(SIZE, 0.45).point(lambda v: min(255, v * 2))
    return ImageChops.multiply(image, Image.merge("RGB", [edge, edge, edge]))


def finish(image, name, directory=OUT):
    """明るさを α に移して書き出す。"""
    image = fade_edge(image)
    r, g, b = image.split()
    alpha = ImageChops.lighter(ImageChops.lighter(r, g), b)
    pixels = []
    for pr, pg, pb, pa in zip(r.tobytes(), g.tobytes(), b.tobytes(), alpha.tobytes()):
        scale = 255.0 / pa if pa else 0.0
        pixels.append((min(255, int(pr * scale)), min(255, int(pg * scale)), min(255, int(pb * scale)), pa))
    result = Image.new("RGBA", (SIZE, SIZE))
    result.putdata(pixels)
    os.makedirs(directory, exist_ok=True)
    result.save(os.path.join(directory, name + ".png"))
    print("wrote", name)


def star(color, core, name):
    """恒星。やわらかい光に細い光条を足す。"""
    image = black()
    glow = radial_falloff(SIZE, 5.0)
    bright = radial_falloff(SIZE, 14.0)
    spikes = Image.new("L", (SIZE, SIZE))
    draw = ImageDraw.Draw(spikes)
    draw.line((HALF, HALF - 92, HALF, HALF + 92), fill=120, width=1)
    draw.line((HALF - 92, HALF, HALF + 92, HALF), fill=120, width=1)
    spikes = ImageChops.multiply(spikes.filter(ImageFilter.GaussianBlur(0.7)), radial_falloff(SIZE, 2.0))
    image = add(image, tint(glow, color), tint(bright, core), tint(spikes, color))
    finish(image, name)


def orion():
    fan = shape(lambda d: d.polygon([(128, 150), (28, 58), (70, 26), (128, 16), (186, 30), (232, 74)], fill=255), 20)
    wings = shape(lambda d: d.ellipse((46, 72, 210, 206), fill=200), 24)
    mask = ImageChops.lighter(fan, wings)
    image = add(gas(201, mask, (235, 60, 95), 85, 190), gas(202, mask, (120, 70, 210), 110, 215), tint(ellipse_mask(128, 138, 42, 34, 20), (120, 235, 215)), tint(ellipse_mask(128, 138, 18, 15, 10), (255, 245, 225)))
    for x, y in ((122, 134), (133, 131), (129, 143), (138, 141)):
        point(image, x, y, 1.5, (235, 245, 255))
    stars(image, random.Random(5), 50, 62, (200, 215, 255))
    finish(image, "m42")


def lagoon():
    mask = ellipse_mask(128, 128, 104, 72, 24)
    lane = shape(lambda d: d.line((36, 94, 118, 132, 220, 152), fill=255, width=20), 9)
    image = add(gas(211, mask, (235, 55, 85), 85, 190), gas(212, mask, (255, 130, 110), 130, 220))
    image = ImageChops.multiply(image, Image.merge("RGB", [ImageChops.invert(lane)] * 3))
    stars(image, random.Random(6), 60, 48, (210, 225, 255), 150, 120)
    finish(image, "m8")


def crab():
    mask = ellipse_mask(128, 128, 96, 70, 16)
    noise = fractal_noise(SIZE, 221, octaves=6)
    ridges = noise.point(lambda v: int(170 * max(0.0, 1.0 - abs(v - 128) / 46.0) ** 2.0)).filter(ImageFilter.GaussianBlur(1.6))
    filaments = tint(ImageChops.multiply(ridges, mask), (255, 90, 65))
    second = fractal_noise(SIZE, 222, octaves=6).point(lambda v: int(120 * max(0.0, 1.0 - abs(v - 128) / 40.0) ** 2.0)).filter(ImageFilter.GaussianBlur(1.6))
    image = add(filaments, tint(ImageChops.multiply(second, mask), (255, 195, 120)), gas(223, mask, (150, 50, 60), 90, 210), tint(ellipse_mask(128, 128, 66, 48, 26), (75, 115, 215)))
    point(image, 128, 128, 1.6, (220, 235, 255))
    finish(image, "m1")


def ring():
    outer = ellipse_mask(128, 128, 84, 70, 8)
    inner = ellipse_mask(128, 128, 48, 38, 12)
    band = ImageChops.subtract(outer, inner)
    noise = cloud(231, 60, 200, 0.8)
    image = add(tint(ImageChops.multiply(band, noise), (240, 80, 70)), tint(ImageChops.multiply(ellipse_mask(128, 128, 62, 50, 10), ImageChops.invert(ellipse_mask(128, 128, 26, 20, 12))), (90, 215, 190)), tint(ellipse_mask(128, 128, 44, 34, 14), (50, 95, 200)))
    point(image, 128, 128, 1.2, (225, 240, 255))
    finish(image, "m57")


def dumbbell():
    lobes = shape(lambda d: d.polygon([(66, 30), (190, 30), (146, 128), (190, 226), (66, 226), (110, 128)], fill=255), 16)
    disk = ellipse_mask(128, 128, 88, 96, 22).point(lambda v: v // 3)
    image = add(gas(241, lobes, (90, 225, 185), 70, 180), tint(ImageChops.multiply(lobes, cloud(242, 120, 220)), (240, 95, 110)), tint(disk, (150, 40, 60)))
    finish(image.rotate(25, Image.BICUBIC), "m27")


def coalsack():
    """暗黒星雲。天の川の淡い光の中に、星の少ない黒い穴として見せる。"""
    rng = random.Random(7)
    milky = tint(cloud(251, 60, 200), (70, 85, 135))
    image = add(milky, black())
    stars(image, rng, 300, 118, (195, 210, 245), smallest=0.4, largest=0.9, gaussian=False)
    hole = ImageChops.multiply(ellipse_mask(122, 134, 70, 88, 24), cloud(252, 40, 160, 0.7))
    image = ImageChops.multiply(image, Image.merge("RGB", [ImageChops.invert(hole)] * 3))
    edge = radial_falloff(SIZE, 0.9)
    image = ImageChops.multiply(image, Image.merge("RGB", [edge, edge, edge]))
    finish(image, "coalsack")


def eta_carinae():
    lobes = add(ellipse_mask(96, 96, 56, 62, 14), ellipse_mask(160, 160, 56, 62, 14))
    image = add(gas(261, lobes, (240, 130, 60), 70, 190), tint(lobes.point(lambda v: v // 3), (210, 60, 40)), tint(radial_falloff(SIZE, 12.0), (255, 250, 230)))
    finish(image, "etacar")


def open_cluster(name, seed, count, spread, color, gas_color=None, large=2.0):
    rng = random.Random(seed)
    image = black()
    if gas_color:
        image = gas(seed + 300, radial_falloff(SIZE, 0.8), gas_color, 110, 215)
    stars(image, rng, count, spread, color, smallest=0.8, largest=large)
    finish(image, name)


def double_cluster():
    rng = random.Random(9)
    image = black()
    stars(image, rng, 70, 26, (205, 225, 255), 86, 118, 0.7, 1.7)
    stars(image, rng, 70, 26, (205, 225, 255), 170, 138, 0.7, 1.7)
    point(image, 96, 122, 1.7, (255, 180, 120))
    point(image, 164, 130, 1.7, (255, 170, 110))
    finish(image, "doublecluster")


def globular(name, seed, count):
    rng = random.Random(seed)
    image = tint(radial_falloff(SIZE, 4.0), (120, 95, 60))
    stars(image, rng, count, 34, (255, 225, 170), smallest=0.5, largest=1.1)
    stars(image, rng, count // 4, 60, (205, 220, 255), smallest=0.5, largest=1.2)
    finish(image, name)


def spiral(name, seed, tilt, angle):
    """渦巻銀河。正面の円盤を描き、潰して傾ける。"""
    noise = fractal_noise(SIZE, seed).load()
    disk = Image.new("RGB", (SIZE, SIZE))
    pixels = disk.load()
    for y in range(SIZE):
        for x in range(SIZE):
            dx, dy = x - HALF, y - HALF
            radius = math.hypot(dx, dy) / HALF
            if radius >= 1.0:
                continue
            arm = 0.5 + 0.5 * math.cos(2.0 * (math.atan2(dy, dx) - 3.4 * math.log(radius + 0.06)))
            texture = noise[x, y] / 255.0
            bulge = math.exp(-(radius / 0.15) ** 2)
            spread = math.exp(-radius * 2.2) * (0.3 + 0.7 * arm ** 2.0) * (0.6 + texture) * 1.7
            r = 255 * bulge + 110 * spread
            g = 225 * bulge + 140 * spread
            b = 165 * bulge + 215 * spread
            pixels[x, y] = (min(255, int(r)), min(255, int(g)), min(255, int(b)))
    squashed = disk.resize((SIZE, int(SIZE * tilt)), Image.BICUBIC)
    image = black()
    image.paste(squashed, (0, (SIZE - squashed.size[1]) // 2))
    image = image.rotate(angle, Image.BICUBIC).filter(ImageFilter.GaussianBlur(0.7))
    finish(image, name)


def magellanic(name, seed, bar_angle, knots):
    rng = random.Random(seed)
    bar = shape(lambda d: d.ellipse((34, 92, 222, 164), fill=255), 22).rotate(bar_angle, Image.BICUBIC)
    halo = ellipse_mask(128, 128, 100, 84, 30).point(lambda v: v // 2)
    mask = ImageChops.lighter(bar, halo)
    image = add(gas(seed + 1, mask, (145, 165, 235), 70, 190), gas(seed + 2, mask, (235, 225, 205), 120, 220))
    stars(image, rng, 110, 38, (225, 232, 255), smallest=0.4, largest=0.8)
    for x, y in knots:
        point(image, x, y, 4.0, (245, 80, 120))
    edge = radial_falloff(SIZE, 0.9)
    finish(ImageChops.multiply(image, Image.merge("RGB", [edge, edge, edge])), name)


def black_hole(name, ring_color, hot_color, tilt):
    """暗い芯のまわりを回る光の輪。手前に回ってくる側が明るい。"""
    image = Image.new("RGB", (SIZE, SIZE))
    pixels = image.load()
    for y in range(SIZE):
        for x in range(SIZE):
            dx, dy = (x - HALF) / HALF, (y - HALF) / HALF / tilt
            radius = math.hypot(dx, dy)
            band = math.exp(-((radius - 0.46) / 0.13) ** 2)
            side = 0.45 + 0.55 * (0.5 + 0.5 * math.cos(math.atan2(dy, dx) - 2.4))
            value = band * side
            hot = value ** 2.5
            pixels[x, y] = tuple(min(255, int(ring_color[i] * value + hot_color[i] * hot)) for i in range(3))
    finish(image.filter(ImageFilter.GaussianBlur(1.2)), name)


def cygnus_x1():
    image = black()
    disk = ImageChops.subtract(ellipse_mask(128, 128, 92, 30, 8), ellipse_mask(128, 128, 36, 12, 6))
    jet = shape(lambda d: d.line((128, 18, 128, 238), fill=200, width=3), 3)
    image = add(tint(disk, (240, 150, 80)), tint(ImageChops.multiply(jet, radial_falloff(SIZE, 1.3)), (140, 180, 255)), tint(ellipse_mask(128, 128, 30, 14, 8), (30, 20, 30)))
    point(image, 196, 72, 7.0, (150, 190, 255))
    finish(image.rotate(-18, Image.BICUBIC), "cygx1")


def unknown():
    """まだ手に入れていない天体。どれも同じ、淡くぼんやりした光にして正体を隠す。"""
    finish(add(tint(radial_falloff(SIZE, 3.0), (105, 118, 150)), tint(radial_falloff(SIZE, 9.0), (150, 160, 185))), "unknown")


def reticle():
    """星図の照準。細い円と4つの目盛り。"""
    scale = 4
    size = 64 * scale
    image = Image.new("L", (size, size))
    draw = ImageDraw.Draw(image)
    center = size / 2
    radius = 22 * scale
    draw.ellipse((center - radius, center - radius, center + radius, center + radius), outline=255, width=scale)
    for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
        draw.line((center + dx * radius, center + dy * radius, center + dx * (radius + 7 * scale), center + dy * (radius + 7 * scale)), fill=255, width=scale)
    mask = image.resize((64, 64), Image.LANCZOS)
    result = Image.new("RGBA", (64, 64), (255, 255, 255, 0))
    result.putalpha(mask)
    result.save(os.path.join(UI_OUT, "reticle.png"))
    print("wrote reticle")


def main():
    star((230, 70, 45), (255, 170, 130), "star_red")
    star((245, 145, 70), (255, 215, 170), "star_orange")
    star((245, 215, 130), (255, 245, 215), "star_yellow")
    star((205, 215, 245), (255, 255, 255), "star_white")
    star((110, 165, 255), (225, 240, 255), "star_blue")

    orion()
    lagoon()
    crab()
    ring()
    dumbbell()
    coalsack()
    eta_carinae()

    open_cluster("m45", 101, 26, 42, (190, 215, 255), (70, 110, 215), 2.8)
    open_cluster("hyades", 102, 34, 56, (255, 225, 180), None, 2.2)
    open_cluster("m44", 103, 80, 44, (240, 235, 220), None, 1.8)
    open_cluster("m7", 104, 90, 48, (215, 228, 255), None, 1.8)
    double_cluster()
    globular("m13", 105, 900)
    globular("omegacen", 106, 1400)

    spiral("m31", 301, 0.36, 32)
    magellanic("lmc", 311, 18, [(176, 104)])
    magellanic("smc", 312, -32, [])
    black_hole("sgra", (235, 125, 50), (255, 240, 190), 0.82)
    cygnus_x1()
    unknown()
    reticle()


if __name__ == "__main__":
    main()

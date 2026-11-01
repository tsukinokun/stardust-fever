"""盤面の配置データ（StardustFever/Assets/Data/BoardLayout.json）を生成する。

釘を数百本手で並べるのは現実的ではないため、規則（格子・除外範囲）から生成する。
入賞口の位置や星座の星の位置の周りには釘を打たない。
星座の星の位置は Constellations.json から読むので、星座を足したら再生成すること。

    python Tools/GenerateBoardLayout.py

単位はワールド単位（cm）。盤面は X が -30〜30、Y が 0〜91。
"""
import json
import math
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ASSETS = "StardustFever/Assets"
CONSTELLATIONS_PATH = os.path.join(ROOT, ASSETS, "Data", "Constellations.json")
OUTPUT_PATH = os.path.join(ROOT, ASSETS, "Data", "BoardLayout.json")

BALL_RADIUS = 1.5
WALL_THICKNESS_HALF = 1.0

# 盤面の外枠
BOARD_HALF_WIDTH = 30.0
ARC_CENTER = (0.0, 60.0)
ARC_THICKNESS_HALF = 0.6
ARC_RADIUS = BOARD_HALF_WIDTH + ARC_THICKNESS_HALF  # 壁の中心線。内側の面が半径30になる
ARC_SEGMENTS = 64  # 少ないと継ぎ目の角が外へ飛び出して見える

# 発射レーン（左端）。外壁の内面 x=-30 と内壁の面 x=-26 の間を玉が昇る
LANE_INNER_WALL_X = -25.5
LANE_INNER_WALL_HALF_THICKNESS = 0.5
LANE_TOP_Y = 58.0
LAUNCHER_POSITION = (-28.0, 5.0)

# 入賞口（prefab, x, y）。釘はこの周りに打たない
POCKETS = [
    ("Prefabs/StartPocket/Prefab.json", 0.0, 9.0),
    ("Prefabs/SidePocket/Prefab.json", -15.0, 20.0),
    ("Prefabs/SidePocket/Prefab.json", 15.0, 20.0),
]
POCKET_CLEARANCE = 4.0

# スタート入賞口の両脇の命釘
GUARD_PINS = [(-2.9, 11.6), (2.9, 11.6)]

# 釘の格子
PIN_ROW_START_Y = 13.5
PIN_ROW_END_Y = 56.0
PIN_ROW_SPACING = 3.6
PIN_COLUMN_SPACING = 4.6
PIN_MIN_X = -21.5  # 発射レーンの内壁との間に玉が通れる隙間を残す
PIN_MAX_X = 26.0   # 右の外壁との間に玉が通れる隙間を残す
STAR_CLEARANCE = 2.8


def asset(path):
    return ASSETS + "/" + path


def load_star_positions():
    with open(CONSTELLATIONS_PATH, encoding="utf-8") as f:
        data = json.load(f)
    positions = []
    for constellation in data["ConstellationTable"]["constellations"]:
        for star in constellation["stars"]:
            positions.append((star["x"], star["y"]))
    return positions


def is_clear(x, y, star_positions):
    for _, px, py in POCKETS:
        if math.hypot(x - px, y - py) < POCKET_CLEARANCE:
            return False
    for gx, gy in GUARD_PINS:
        if math.hypot(x - gx, y - gy) < 2.0:
            return False
    for sx, sy in star_positions:
        if math.hypot(x - sx, y - sy) < STAR_CLEARANCE:
            return False
    return True


def generate_pins(star_positions):
    pins = [{"x": x, "y": y} for x, y in GUARD_PINS]
    row = 0
    y = PIN_ROW_START_Y
    while y <= PIN_ROW_END_Y + 1e-6:
        offset = 0.0 if row % 2 == 0 else PIN_COLUMN_SPACING * 0.5
        x = -BOARD_HALF_WIDTH + offset
        while x <= PIN_MAX_X + 1e-6:
            if x >= PIN_MIN_X and is_clear(x, y, star_positions):
                pins.append({"x": round(x, 2), "y": round(y, 2)})
            x += PIN_COLUMN_SPACING
        y += PIN_ROW_SPACING
        row += 1
    return pins


def wall(x, y, half_width, half_height, angle_deg):
    return {"x": round(x, 3), "y": round(y, 3), "halfWidth": round(half_width, 3), "halfHeight": round(half_height, 3), "angleDeg": round(angle_deg, 3)}


def generate_walls():
    walls = []
    side_half = ARC_CENTER[1] / 2.0
    # 左右の外壁（縦）
    side_x = BOARD_HALF_WIDTH + WALL_THICKNESS_HALF
    walls.append(wall(-side_x, side_half, side_half, WALL_THICKNESS_HALF, 90.0))
    walls.append(wall(side_x, side_half, side_half, WALL_THICKNESS_HALF, 90.0))
    # 上の半円。弦の長さより少し長くして、継ぎ目に玉が引っかからないようにする
    step = math.pi / ARC_SEGMENTS
    chord_half = ARC_RADIUS * math.sin(step / 2.0) * 1.02
    for i in range(ARC_SEGMENTS):
        theta = math.pi - (i + 0.5) * step
        x = ARC_CENTER[0] + ARC_RADIUS * math.cos(theta)
        y = ARC_CENTER[1] + ARC_RADIUS * math.sin(theta)
        walls.append(wall(x, y, chord_half, ARC_THICKNESS_HALF, math.degrees(theta) - 90.0))
    # 発射レーンの内壁と床
    lane_bottom = 2.0
    walls.append(wall(LANE_INNER_WALL_X, (lane_bottom + LANE_TOP_Y) / 2.0, (LANE_TOP_Y - lane_bottom) / 2.0, LANE_INNER_WALL_HALF_THICKNESS, 90.0))
    walls.append(wall(-28.0, -WALL_THICKNESS_HALF, 3.5, WALL_THICKNESS_HALF, 0.0))
    return walls


def generate_objects():
    objects = [{"prefab": asset(prefab), "x": x, "y": y} for prefab, x, y in POCKETS]
    objects.append({"prefab": asset("Prefabs/Drain/Prefab.json"), "x": 2.0, "y": -3.0})
    objects.append({"prefab": asset("Prefabs/Foul/Prefab.json"), "x": -28.0, "y": 1.5})
    objects.append({"prefab": asset("Prefabs/Launcher/Prefab.json"), "x": LAUNCHER_POSITION[0], "y": LAUNCHER_POSITION[1]})
    objects.append({"prefab": asset("Prefabs/Backboard/Prefab.json"), "x": 0.0, "y": 45.0})
    return objects


def main():
    star_positions = load_star_positions()
    layout = {
        "BoardLayout": {
            "pinPrefab": asset("Prefabs/Pin/Prefab.json"),
            "wallPrefab": asset("Prefabs/Wall/Prefab.json"),
            "pins": generate_pins(star_positions),
            "walls": generate_walls(),
            "objects": generate_objects(),
        }
    }
    with open(OUTPUT_PATH, "w", encoding="utf-8", newline="\n") as f:
        json.dump(layout, f, ensure_ascii=False, indent=4)
        f.write("\n")
    print(f"wrote {OUTPUT_PATH}: {len(layout['BoardLayout']['pins'])} pins, {len(layout['BoardLayout']['walls'])} walls")


if __name__ == "__main__":
    main()

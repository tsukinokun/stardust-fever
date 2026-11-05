"""盤面の配置データ（StardustFever/Assets/Data/BoardLayout.json）を生成する。

釘を数百本手で並べるのは現実的ではないため、規則（格子・除外範囲）から生成する。
入賞口は本物のパチンコのように、上が開いた杯（左右の柱＋口の底のセンサー）にする。
口に落ちた玉だけが入賞し、柱に当たった玉は弾かれる。横の入賞口はチューリップで、柱の上に羽が付く。
入賞口の周りの釘は、柱や羽と玉が挟まる隙間を作らないよう抜く。
星ゲットチャンスの星釘は88星座のどこにでも出るので、星の周りの釘は抜かない
（チャンス中だけ StarChanceSystem が重なる釘を隠して当たり判定を外す）。

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

# 入賞口の種類。口の内側の幅の半分（mouth）、柱の太さの半分（post）、
# 柱の下端と上端（口の底のセンサーの中心からの高さ）、チューリップの羽（wing: 長さの半分・太さの半分・閉じた角度・開いた角度）
POCKET_TYPES = {
    "start": {
        "sensor": "Prefabs/StartPocket/Prefab.json", "post": "Prefabs/PocketPostStart/Prefab.json",
        "mouth": 2.0, "postHalf": 0.35, "bottom": -0.7, "top": 1.8, "wing": None,
    },
    "tulip": {
        "sensor": "Prefabs/SidePocket/Prefab.json", "post": "Prefabs/PocketPostTulip/Prefab.json",
        "mouth": 2.0, "postHalf": 0.35, "bottom": -0.7, "top": 1.2,
        "wing": {"halfLength": 1.5, "halfThickness": 0.22, "closedDeg": 90.0, "openDeg": 40.0,  # 右の羽の角度。左の羽は 180 - 角度（TulipWingLeft/Right の Prefab と揃える）
                
                 "left": "Prefabs/TulipWingLeft/Prefab.json", "right": "Prefabs/TulipWingRight/Prefab.json"},
    },
    "starGate": {
        "sensor": "Prefabs/StarGate/Prefab.json", "post": "Prefabs/PocketPostStarGate/Prefab.json",
        "mouth": 2.3, "postHalf": 0.3, "bottom": -0.7, "top": 1.2, "wing": None,
    },
}

# 入賞口（種類, x, y）。x, y は口の底のセンサーの中心。
# 星の門はスタート入賞口とチューリップの間に左右1つずつ置く（落ちてくる道の途中なので邪魔にならない）
POCKETS = [
    ("start", 0.0, 9.0),
    ("tulip", -15.0, 20.0),
    ("tulip", 15.0, 20.0),
    ("starGate", -8.0, 15.0),
    ("starGate", 8.0, 15.0),
]

# 玉を消してしまうセンサー（prefab, x, y）。BuildSkyCatalog.py が星との隙間を確かめるのに使う
SENSORS = [(POCKET_TYPES[kind]["sensor"], x, y) for kind, x, y in POCKETS]

PIN_RADIUS = 0.5
# 入賞口の部品と、釘や別の入賞口の部品との隙間の決まり。
# 玉の直径より少し広ければ玉は通り抜け、ほぼ0なら玉は入れない。その間の幅だと玉が2つの間に乗って止まる
PASSABLE_GAP = BALL_RADIUS * 2.0 + 0.2
TOUCHING_GAP = 0.3

# 釘の格子
PIN_ROW_START_Y = 13.5
PIN_ROW_END_Y = 56.0
PIN_ROW_SPACING = 3.6
PIN_COLUMN_SPACING = 4.6
PIN_MIN_X = -21.5  # 発射レーンの内壁との間に玉が通れる隙間を残す
PIN_MAX_X = 26.0   # 右の外壁との間に玉が通れる隙間を残す


def asset(path):
    return ASSETS + "/" + path


#-------------------------------------------------------------
# 入賞口の部品の形（距離を測るための線分と太さ）
#-------------------------------------------------------------
def box_segments(cx, cy, hx, hy):
    corners = [(cx - hx, cy - hy), (cx + hx, cy - hy), (cx + hx, cy + hy), (cx - hx, cy + hy)]
    return [(corners[i], corners[(i + 1) % 4], 0.0) for i in range(4)]


def wing_segment(hinge, angle_deg, wing):
    angle = math.radians(angle_deg)
    tip = (hinge[0] + math.cos(angle) * wing["halfLength"] * 2.0, hinge[1] + math.sin(angle) * wing["halfLength"] * 2.0)
    return (hinge, tip, wing["halfThickness"])


def pocket_parts(kind, x, y):
    """入賞口の部品。posts は柱の箱 (cx, cy, hx, hy)、wings は羽 (蝶番, 閉じた角度, 開いた角度)、shapes は距離を測る線分。"""
    spec = POCKET_TYPES[kind]
    mouth, post = spec["mouth"], spec["postHalf"]
    post_hy = (spec["top"] - spec["bottom"]) / 2.0
    post_cy = y + (spec["top"] + spec["bottom"]) / 2.0
    posts = [(x - mouth - post, post_cy, post, post_hy), (x + mouth + post, post_cy, post, post_hy)]
    shapes = []
    for post_box in posts:
        shapes.extend(box_segments(*post_box))
    wings = []
    wing = spec["wing"]
    if wing:
        top = y + spec["top"]
        # 左の羽は 180 - 角度、右の羽は角度（どちらも蝶番は柱の上の内側の角）
        for side, hinge in ((-1, (x - mouth, top)), (1, (x + mouth, top))):
            closed = wing["closedDeg"] if side > 0 else 180.0 - wing["closedDeg"]
            opened = wing["openDeg"] if side > 0 else 180.0 - wing["openDeg"]
            wings.append((side, hinge, closed, opened))
            # 開け閉めの途中の形もすべて含める
            steps = 6
            for i in range(steps + 1):
                shapes.append(wing_segment(hinge, closed + (opened - closed) * i / steps, wing))
    return posts, wings, shapes


def point_segment_distance(p, a, b):
    ax, ay = a
    bx, by = b
    dx, dy = bx - ax, by - ay
    length2 = dx * dx + dy * dy
    t = 0.0 if length2 <= 1e-12 else max(0.0, min(1.0, ((p[0] - ax) * dx + (p[1] - ay) * dy) / length2))
    return math.hypot(p[0] - (ax + dx * t), p[1] - (ay + dy * t))


def segments_intersect(a, b, c, d):
    def orient(p, q, r):
        return (q[0] - p[0]) * (r[1] - p[1]) - (q[1] - p[1]) * (r[0] - p[0])
    return orient(a, b, c) * orient(a, b, d) < 0 and orient(c, d, a) * orient(c, d, b) < 0


def segment_distance(s1, s2):
    (a, b, r1), (c, d, r2) = s1, s2
    if segments_intersect(a, b, c, d):
        return -(r1 + r2)
    raw = min(point_segment_distance(a, c, d), point_segment_distance(b, c, d),
              point_segment_distance(c, a, b), point_segment_distance(d, a, b))
    return raw - r1 - r2


def gap_to_parts(x, y, radius, shapes):
    return min(point_segment_distance((x, y), a, b) - r - radius for a, b, r in shapes)


ALL_SHAPES = [pocket_parts(kind, x, y)[2] for kind, x, y in POCKETS]


def is_clear(x, y):
    """釘を打ってよいか。入賞口の部品との隙間が、玉が通れる幅より狭い所には打たない。"""
    for shapes in ALL_SHAPES:
        if gap_to_parts(x, y, PIN_RADIUS, shapes) < PASSABLE_GAP:
            return False
    return True


def check_pocket_gaps():
    """別々の入賞口の部品の間に、玉が挟まって止まる幅の隙間が無いか確かめる。"""
    errors = []
    for i in range(len(POCKETS)):
        for j in range(i + 1, len(POCKETS)):
            gap = min(segment_distance(s1, s2) for s1 in ALL_SHAPES[i] for s2 in ALL_SHAPES[j])
            if TOUCHING_GAP <= gap < PASSABLE_GAP:
                errors.append(f"{POCKETS[i]} and {POCKETS[j]} leave a gap of {gap:.2f} (needs < {TOUCHING_GAP} or >= {PASSABLE_GAP})")
    return errors


def generate_pins():
    pins = []
    row = 0
    y = PIN_ROW_START_Y
    while y <= PIN_ROW_END_Y + 1e-6:
        offset = 0.0 if row % 2 == 0 else PIN_COLUMN_SPACING * 0.5
        x = -BOARD_HALF_WIDTH + offset
        while x <= PIN_MAX_X + 1e-6:
            if x >= PIN_MIN_X and is_clear(x, y):
                pins.append({"x": round(x, 2), "y": round(y, 2)})
            x += PIN_COLUMN_SPACING
        y += PIN_ROW_SPACING
        row += 1
    return pins


def wall(x, y, half_width, half_height, angle_deg, prefab=None):
    result = {"x": round(x, 3), "y": round(y, 3), "halfWidth": round(half_width, 3), "halfHeight": round(half_height, 3), "angleDeg": round(angle_deg, 3)}
    if prefab:
        result["prefab"] = asset(prefab)
    return result


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
    # 入賞口の柱（入賞口の色の Prefab）
    for kind, x, y in POCKETS:
        posts, _, _ = pocket_parts(kind, x, y)
        for cx, cy, hx, hy in posts:
            walls.append(wall(cx, cy, hx, hy, 0.0, POCKET_TYPES[kind]["post"]))
    return walls


def generate_objects():
    objects = [{"prefab": asset(prefab), "x": x, "y": y} for prefab, x, y in SENSORS]
    # チューリップの羽は蝶番の位置に置く（向きと位置は TulipSystem が蝶番を中心に回して決める）
    for kind, x, y in POCKETS:
        _, wings, _ = pocket_parts(kind, x, y)
        wing = POCKET_TYPES[kind]["wing"]
        for side, hinge, _, _ in wings:
            objects.append({"prefab": asset(wing["left"] if side < 0 else wing["right"]), "x": round(hinge[0], 3), "y": round(hinge[1], 3)})
    objects.append({"prefab": asset("Prefabs/Drain/Prefab.json"), "x": 2.0, "y": -3.0})
    objects.append({"prefab": asset("Prefabs/Foul/Prefab.json"), "x": -28.0, "y": 1.5})
    objects.append({"prefab": asset("Prefabs/Launcher/Prefab.json"), "x": LAUNCHER_POSITION[0], "y": LAUNCHER_POSITION[1]})
    objects.append({"prefab": asset("Prefabs/Backboard/Prefab.json"), "x": 0.0, "y": 45.0})
    return objects


def main():
    errors = check_pocket_gaps()
    if errors:
        for e in errors:
            print("error:", e)
        print("BoardLayout.json was NOT written.")
        return 1
    layout = {
        "BoardLayout": {
            "pinPrefab": asset("Prefabs/Pin/Prefab.json"),
            "wallPrefab": asset("Prefabs/Wall/Prefab.json"),
            "pins": generate_pins(),
            "walls": generate_walls(),
            "objects": generate_objects(),
        }
    }
    with open(OUTPUT_PATH, "w", encoding="utf-8", newline="\n") as f:
        json.dump(layout, f, ensure_ascii=False, indent=4)
        f.write("\n")
    print(f"wrote {OUTPUT_PATH}: {len(layout['BoardLayout']['pins'])} pins, {len(layout['BoardLayout']['walls'])} walls")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

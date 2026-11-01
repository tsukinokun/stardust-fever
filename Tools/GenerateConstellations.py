"""星座の星の盤面上の位置（Constellations.json の x / y）を、空の座標（ra / dec）から計算し直す。

星を手で置くと、入賞口（役物）と重なった星は玉が先に入賞して消えるため、つなげなくなる。
ここでは各星座を「星を出せる範囲」に収め、書き出した後に役物との距離を確かめる。

    python Tools/GenerateConstellations.py
    python Tools/GenerateBoardLayout.py     # 星の周りの釘を抜き直す

ra は時（0〜24）、dec は度。空は東が左なので、赤経が大きいほど左へ置く。
役物との距離が足りない星があると、一覧を出して終了コード 1 で止まる（ファイルは書き換えない）。
"""
import json
import math
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import GenerateBoardLayout as board  # noqa: E402  入賞口の位置は盤面の生成と同じ定義を使う

CONSTELLATIONS_PATH = board.CONSTELLATIONS_PATH

# 星を出せる範囲。下は入賞口の上端(20.8)+星の半径+玉の直径より上、左は発射レーン、右と上は外壁から離す
STAR_FIELD_MIN = (-20.0, 26.0)
STAR_FIELD_MAX = (24.0, 56.0)
# 縦横の倍率の比の上限。ベルトの三つ星のように詰まった星を横へ広げるために、形を少しだけ崩してよい
MAX_AXIS_STRETCH = 2.0

STAR_RADIUS = 1.6        # StarPin Prefab の当たり判定の半径
BALL_DIAMETER = board.BALL_RADIUS * 2.0
MIN_STAR_SPACING = 3.6   # これより近い星同士は見分けにくい（つなぐ分には問題ない）


def load_json(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def prefab_sensor_box(prefab_rel, x, y):
    """Prefab の CollisionComponent の大きさから、盤面上の当たり判定の箱 (minx, miny, maxx, maxy) を作る。"""
    prefab = load_json(os.path.join(board.ROOT, board.asset(prefab_rel)))
    for entry in prefab["Components"]:
        if entry["key"] == "CollisionComponent":
            extent = load_json(os.path.join(board.ROOT, entry["value"]))["CollisionComponent"]["extent"]
            return (x - extent["x"], y - extent["y"], x + extent["x"], y + extent["y"])
    raise ValueError(f"{prefab_rel} has no CollisionComponent")


def blocking_boxes():
    """星と重なってはいけない、玉を消してしまう物（入賞口・アウト口）の箱。"""
    boxes = [(prefab, prefab_sensor_box(prefab, x, y)) for prefab, x, y in board.POCKETS]
    boxes.append(("Prefabs/Drain/Prefab.json", prefab_sensor_box("Prefabs/Drain/Prefab.json", 2.0, -3.0)))
    return boxes


def distance_to_box(px, py, box):
    dx = max(box[0] - px, 0.0, px - box[2])
    dy = max(box[1] - py, 0.0, py - box[3])
    return math.hypot(dx, dy)


def project(stars):
    """空の座標を、星座の中心を原点にした平面（度）へ写す。"""
    ras = [s["ra"] for s in stars]
    decs = [s["dec"] for s in stars]
    ra0 = (min(ras) + max(ras)) / 2.0
    dec0 = (min(decs) + max(decs)) / 2.0
    scale = 15.0 * math.cos(math.radians(dec0))
    return [(-(s["ra"] - ra0) * scale, s["dec"] - dec0) for s in stars]


def fit_to_field(points):
    xs = [p[0] for p in points]
    ys = [p[1] for p in points]
    span_x = max(max(xs) - min(xs), 1e-6)
    span_y = max(max(ys) - min(ys), 1e-6)
    field_w = STAR_FIELD_MAX[0] - STAR_FIELD_MIN[0]
    field_h = STAR_FIELD_MAX[1] - STAR_FIELD_MIN[1]
    sx = field_w / span_x
    sy = field_h / span_y
    sx = min(sx, sy * MAX_AXIS_STRETCH)
    sy = min(sy, sx * MAX_AXIS_STRETCH)
    cx = (STAR_FIELD_MIN[0] + STAR_FIELD_MAX[0]) / 2.0
    cy = (STAR_FIELD_MIN[1] + STAR_FIELD_MAX[1]) / 2.0
    mx = (max(xs) + min(xs)) / 2.0
    my = (max(ys) + min(ys)) / 2.0
    return [(round(cx + (x - mx) * sx, 2), round(cy + (y - my) * sy, 2)) for x, y in points]


def main():
    data = load_json(CONSTELLATIONS_PATH)
    boxes = blocking_boxes()
    errors = []
    warnings = []

    for constellation in data["ConstellationTable"]["constellations"]:
        stars = constellation["stars"]
        missing = [s["starId"] for s in stars if "ra" not in s or "dec" not in s]
        if missing:
            errors.append(f"{constellation['id']}: ra/dec missing for {', '.join(missing)}")
            continue

        positions = fit_to_field(project(stars))
        for star, (x, y) in zip(stars, positions):
            star["x"] = x
            star["y"] = y
            # 星の縁と役物の間に玉が1つ入る隙間が無いと、星へ向かう玉が先に役物へ吸われる
            for prefab, box in boxes:
                gap = distance_to_box(x, y, box) - STAR_RADIUS
                if gap < BALL_DIAMETER:
                    errors.append(f"{constellation['id']}/{star['starId']} at ({x}, {y}) is {gap:.2f} from {prefab} (needs {BALL_DIAMETER})")

        for i in range(len(stars)):
            for j in range(i + 1, len(stars)):
                d = math.hypot(stars[i]["x"] - stars[j]["x"], stars[i]["y"] - stars[j]["y"])
                if d < MIN_STAR_SPACING:
                    warnings.append(f"{constellation['id']}: {stars[i]['starId']} and {stars[j]['starId']} are {d:.2f} apart")

        print(constellation["id"], [(s["starId"], s["x"], s["y"]) for s in stars])

    for w in warnings:
        print("warning:", w)
    if errors:
        for e in errors:
            print("error:", e)
        print("Constellations.json was NOT written.")
        return 1

    with open(CONSTELLATIONS_PATH, "w", encoding="utf-8", newline="\n") as f:
        json.dump(data, f, ensure_ascii=False, indent=4)
        f.write("\n")
    print(f"wrote {CONSTELLATIONS_PATH}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

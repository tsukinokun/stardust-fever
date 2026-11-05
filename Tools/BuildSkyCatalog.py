"""88星座と、その線をつくる星のデータを公開データから作る。

    python Tools/BuildSkyCatalog.py --download   # 元データを Tools/SkyData/ へ取ってくる（初回だけ）
    python Tools/BuildSkyCatalog.py              # Stars.json / Constellations.json / ConstellationGroups.json を書き出す
    python Tools/GenerateBoardLayout.py          # 盤面の釘を打ち直す

元データ（Tools/SkyData/ はコミットしない。クレジットは Docs/Credits.md）
- 星座の線: Stellarium の modern sky culture（index.json）。CC BY-SA 4.0
- 星の位置・等級・スペクトル型・バイエル符号: HYG Database v4.1。CC BY-SA 4.0
- 星の固有名: IAU WGSN（IAU-CSN.txt）。パブリックドメイン

盤面へは、星座ごとに中心から平射図法で写し、「星を出せる範囲」に収める。
星が多い星座は、明るい星から線でつながる順に MAX_BOARD_STARS 個まで選ぶ（抽選の対象は線の星すべて）。
役物（入賞口・アウト口・星の門）との隙間が足りない星があると、一覧を出して終了コード 1 で止まる。
"""
import csv
import json
import math
import os
import re
import sys
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import GenerateBoardLayout as board  # noqa: E402
import SkyNames as names  # noqa: E402

SKY_DATA = os.path.join(board.ROOT, "Tools", "SkyData")
SOURCES = {
    "index.json": "https://raw.githubusercontent.com/Stellarium/stellarium/master/skycultures/modern/index.json",
    "description.md": "https://raw.githubusercontent.com/Stellarium/stellarium/master/skycultures/modern/description.md",
    "hygdata_v41.csv": "https://raw.githubusercontent.com/astronexus/HYG-Database/main/hyg/CURRENT/hygdata_v41.csv",
    "IAU-CSN.txt": "https://www.pas.rochester.edu/~emamajek/WGSN/IAU-CSN.txt",
}
STARS_PATH = os.path.join(board.ROOT, board.ASSETS, "Data", "Stars.json")
CONSTELLATIONS_PATH = board.CONSTELLATIONS_PATH
GROUPS_PATH = os.path.join(board.ROOT, board.ASSETS, "Data", "ConstellationGroups.json")

# 星を出せる範囲。下は入賞口の上端(20.8)+星の半径+玉の直径より上、左は発射レーン、右と上は外壁と星の門から離す
STAR_FIELD_MIN = (-20.0, 26.0)
STAR_FIELD_MAX = (24.0, 56.0)
MAX_AXIS_STRETCH = 2.0      # 縦横の倍率の比の上限（形を少しだけ崩してよい）
MAX_BOARD_STARS = 12        # 盤面に出す星の上限
MIN_BOARD_STARS = 3
MIN_STAR_SPACING = 3.4      # これより近い星は暗い方を盤面から外す（星の直径 3.2 ＋少し）
STAR_RADIUS = 1.6           # StarPin Prefab の当たり判定の半径
BALL_DIAMETER = board.BALL_RADIUS * 2.0
SPECTRAL_TYPES = "OBAFGKM"


def download():
    os.makedirs(SKY_DATA, exist_ok=True)
    for file_name, url in SOURCES.items():
        path = os.path.join(SKY_DATA, file_name)
        print(f"download {url}")
        urllib.request.urlretrieve(url, path)
    print(f"saved to {SKY_DATA}")


def load_json(path):
    with open(path, encoding="utf-8") as f:
        return json.load(f)


def write_json(path, data):
    with open(path, "w", encoding="utf-8", newline="\n") as f:
        json.dump(data, f, ensure_ascii=False, indent=4)
        f.write("\n")


#-------------------------------------------------------------
# 元データの読み込み
#-------------------------------------------------------------
def load_lines():
    """略符 → 線の HIP の組の一覧（重複は除く）。"""
    data = load_json(os.path.join(SKY_DATA, "index.json"))
    result = {}
    for constellation in data["constellations"]:
        abbr = constellation["id"].split()[-1]
        pairs = []
        seen = set()
        for polyline in constellation["lines"]:
            hips = [int(h) for h in polyline]
            for a, b in zip(hips, hips[1:]):
                key = (min(a, b), max(a, b))
                if a != b and key not in seen:
                    seen.add(key)
                    pairs.append((a, b))
        result[abbr] = pairs
    return result


def load_hyg(hips):
    rows = {}
    with open(os.path.join(SKY_DATA, "hygdata_v41.csv"), encoding="utf-8") as f:
        for row in csv.DictReader(f):
            if row["hip"] and int(row["hip"]) in hips:
                rows[int(row["hip"])] = row
    return rows


def load_iau_names():
    """HIP → IAU の固有名。列は空白区切りだが名前に空白を含むことがあるので、名前は先頭18文字から取る。"""
    pattern = re.compile(r"\s(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+(\d{4}-\d\d-\d\d)")
    result = {}
    with open(os.path.join(SKY_DATA, "IAU-CSN.txt"), encoding="utf-8") as f:
        for line in f:
            if line.startswith(("#", "$")) or not line.strip():
                continue
            match = pattern.search(line)
            if match and match.group(3) != "_":
                result[int(match.group(3))] = line[:18].strip()
    return result


#-------------------------------------------------------------
# 星
#-------------------------------------------------------------
def designation(row):
    """バイエル符号（無ければフラムスティード番号）を「オリオン座α」の形にする。"""
    constellation_name = names.CONSTELLATIONS.get(row["con"], (row["con"], ""))[0]
    bayer = row["bayer"]
    if bayer:
        match = re.match(r"^([A-Za-z]+)-?(\d*)$", bayer)
        if match and match.group(1) in names.GREEK:
            return constellation_name + names.GREEK[match.group(1)] + match.group(2)
    if row["flam"]:
        return f"{constellation_name}{row['flam']}番星"
    return f"HIP {row['hip']}"


def make_star(hip, row, iau_name, missing_names):
    magnitude = float(row["mag"])
    spectral = next((c for c in row["spect"] if c in SPECTRAL_TYPES), "A")
    desig = designation(row)
    if iau_name:
        name = names.STAR_NAMES.get(iau_name)
        if not name:
            missing_names.append(iau_name)
            name = iau_name
    else:
        name = desig
    # 明るい星ほど珍しく、持つと効果が大きい
    brightness = min(max((5.0 - magnitude) / 6.0, 0.0), 1.0)
    return {
        "id": f"hip_{hip}",
        "name": name,
        "nameEn": iau_name or "",
        "designation": desig,
        "constellationId": row["con"].lower(),
        "ra": round(float(row["ra"]), 5),
        "dec": round(float(row["dec"]), 5),
        "magnitude": round(magnitude, 2),
        "spectralType": spectral,
        "basePayout": round(50.0 * 3.0 ** ((5.0 - magnitude) / 1.5)),
        "levelBonus": round(0.01 + 0.03 * brightness, 4),
    }


#-------------------------------------------------------------
# 盤面への配置
#-------------------------------------------------------------
def unit_vector(ra_hours, dec_deg):
    ra = math.radians(ra_hours * 15.0)
    dec = math.radians(dec_deg)
    return (math.cos(dec) * math.cos(ra), math.cos(dec) * math.sin(ra), math.sin(dec))


def project(stars):
    """星座の中心から平射図法で写す。空は東が左なので、東を -x にする。"""
    vectors = [unit_vector(s["ra"], s["dec"]) for s in stars]
    cx, cy, cz = (sum(v[i] for v in vectors) for i in range(3))
    length = math.sqrt(cx * cx + cy * cy + cz * cz)
    center = (cx / length, cy / length, cz / length)
    east = (-center[1], center[0], 0.0)  # 北極 × 中心
    east_length = math.hypot(east[0], east[1])
    east = (east[0] / east_length, east[1] / east_length, 0.0) if east_length > 1e-6 else (1.0, 0.0, 0.0)
    north = (center[1] * east[2] - center[2] * east[1], center[2] * east[0] - center[0] * east[2], center[0] * east[1] - center[1] * east[0])
    points = []
    for v in vectors:
        k = 2.0 / (1.0 + sum(a * b for a, b in zip(v, center)))
        points.append((-k * sum(a * b for a, b in zip(v, east)), k * sum(a * b for a, b in zip(v, north))))
    return points


def fit_to_field(points):
    xs = [p[0] for p in points]
    ys = [p[1] for p in points]
    span_x = max(max(xs) - min(xs), 1e-6)
    span_y = max(max(ys) - min(ys), 1e-6)
    sx = (STAR_FIELD_MAX[0] - STAR_FIELD_MIN[0]) / span_x
    sy = (STAR_FIELD_MAX[1] - STAR_FIELD_MIN[1]) / span_y
    sx = min(sx, sy * MAX_AXIS_STRETCH)
    sy = min(sy, sx * MAX_AXIS_STRETCH)
    cx = (STAR_FIELD_MIN[0] + STAR_FIELD_MAX[0]) / 2.0
    cy = (STAR_FIELD_MIN[1] + STAR_FIELD_MAX[1]) / 2.0
    mx = (max(xs) + min(xs)) / 2.0
    my = (max(ys) + min(ys)) / 2.0
    return [(round(cx + (x - mx) * sx, 2), round(cy + (y - my) * sy, 2)) for x, y in points]


def choose_board_stars(hips, pairs, magnitudes):
    """明るい星から、線でつながっている星を優先して MAX_BOARD_STARS 個まで選ぶ。"""
    neighbours = {h: set() for h in hips}
    for a, b in pairs:
        neighbours[a].add(b)
        neighbours[b].add(a)
    remaining = sorted(hips, key=lambda h: magnitudes[h])
    chosen = [remaining.pop(0)]
    while remaining and len(chosen) < MAX_BOARD_STARS:
        linked = [h for h in remaining if neighbours[h] & set(chosen)]
        pick = linked[0] if linked else remaining[0]
        remaining.remove(pick)
        chosen.append(pick)
    return chosen


def place_on_board(abbr, hips, pairs, star_by_hip, boxes, errors):
    magnitudes = {h: star_by_hip[h]["magnitude"] for h in hips}
    chosen = choose_board_stars(hips, pairs, magnitudes)
    while True:
        positions = fit_to_field(project([star_by_hip[h] for h in chosen]))
        closest = None
        for i in range(len(chosen)):
            for j in range(i + 1, len(chosen)):
                d = math.hypot(positions[i][0] - positions[j][0], positions[i][1] - positions[j][1])
                if d < MIN_STAR_SPACING and (closest is None or d < closest[0]):
                    closest = (d, i, j)
        if closest is None or len(chosen) <= MIN_BOARD_STARS:
            break
        #-------------------------------------------------------------
        # 近すぎる2つの暗い方を外すか、形の端にある星を外して全体を大きく写すか、より暗い星を外す。
        # 端の暗い星が広がりを決めていると、オリオン座の三つ星のような明るく詰まった星が先に消えてしまうため
        #-------------------------------------------------------------
        _, i, j = closest
        pair_victim = i if magnitudes[chosen[i]] > magnitudes[chosen[j]] else j
        xs = [p[0] for p in positions]
        ys = [p[1] for p in positions]
        edges = {xs.index(min(xs)), xs.index(max(xs)), ys.index(min(ys)), ys.index(max(ys))}
        edge_victim = max(edges, key=lambda k: magnitudes[chosen[k]])
        victim = edge_victim if magnitudes[chosen[edge_victim]] > magnitudes[chosen[pair_victim]] else pair_victim
        chosen.pop(victim)

    for hip, (x, y) in zip(chosen, positions):
        for prefab, box in boxes:
            gap = distance_to_box(x, y, box) - STAR_RADIUS
            if gap < BALL_DIAMETER:
                errors.append(f"{abbr}/hip_{hip} at ({x}, {y}) is {gap:.2f} from {prefab} (needs {BALL_DIAMETER})")

    index = {h: i for i, h in enumerate(chosen)}
    board_lines = [{"from": index[a], "to": index[b]} for a, b in pairs if a in index and b in index]
    board_stars = [{"starId": f"hip_{h}", "x": x, "y": y} for h, (x, y) in zip(chosen, positions)]
    return {"stars": board_stars, "lines": board_lines}


def prefab_sensor_box(prefab_rel, x, y):
    prefab = load_json(os.path.join(board.ROOT, board.asset(prefab_rel)))
    for entry in prefab["Components"]:
        if entry["key"] == "CollisionComponent":
            extent = load_json(os.path.join(board.ROOT, entry["value"]))["CollisionComponent"]["extent"]
            return (x - extent["x"], y - extent["y"], x + extent["x"], y + extent["y"])
    raise ValueError(f"{prefab_rel} has no CollisionComponent")


def blocking_boxes():
    boxes = [(prefab, prefab_sensor_box(prefab, x, y)) for prefab, x, y in board.SENSORS]
    boxes.append(("Prefabs/Drain/Prefab.json", prefab_sensor_box("Prefabs/Drain/Prefab.json", 2.0, -3.0)))
    return boxes


def distance_to_box(px, py, box):
    dx = max(box[0] - px, 0.0, px - box[2])
    dy = max(box[1] - py, 0.0, py - box[3])
    return math.hypot(dx, dy)


#-------------------------------------------------------------
# 書き出し
#-------------------------------------------------------------
def main():
    if "--download" in sys.argv:
        download()
        return 0

    lines = load_lines()
    hips = {h for pairs in lines.values() for pair in pairs for h in pair}
    hyg = load_hyg(hips)
    iau = load_iau_names()
    missing_rows = sorted(hips - hyg.keys())
    if missing_rows:
        print("error: not in HYG:", missing_rows)
        return 1

    missing_names = []
    star_by_hip = {h: make_star(h, hyg[h], iau.get(h), missing_names) for h in sorted(hips)}
    for name in sorted(set(missing_names)):
        print(f"warning: no Japanese name for '{name}' (add it to SkyNames.STAR_NAMES)")

    boxes = blocking_boxes()
    errors = []
    constellations = []
    for abbr in sorted(lines, key=lambda a: names.CONSTELLATIONS[a][0]):
        pairs = lines[abbr]
        order = []
        for a, b in pairs:
            for h in (a, b):
                if h not in order:
                    order.append(h)
        index = {h: i for i, h in enumerate(order)}
        kind, value = names.constellation_effect(abbr)
        constellations.append({
            "id": abbr.lower(),
            "name": names.CONSTELLATIONS[abbr][0],
            "nameEn": names.CONSTELLATIONS[abbr][1],
            "stars": [{"starId": f"hip_{h}"} for h in order],
            "lines": [{"from": index[a], "to": index[b]} for a, b in pairs],
            "effect": {"type": kind, "value": value},
            "board": place_on_board(abbr, order, pairs, star_by_hip, boxes, errors),
        })

    if errors:
        for e in errors:
            print("error:", e)
        print("Nothing was written.")
        return 1

    aliases = []
    for legacy, hip in names.LEGACY_STAR_IDS.items():
        if hip not in star_by_hip:
            print(f"error: legacy star {legacy} (HIP {hip}) is not on any constellation line")
            return 1
        aliases.append({"from": legacy, "to": f"hip_{hip}"})

    groups = []
    for group_id, name, description, kind, value, members in names.GROUPS:
        unknown = [m for m in members if m not in lines]
        if unknown:
            print(f"error: group {group_id} has unknown constellations {unknown}")
            return 1
        groups.append({
            "id": group_id,
            "name": name,
            "description": description,
            "constellationIds": [m.lower() for m in members],
            "effect": {"type": kind, "value": value},
        })

    write_json(GROUPS_PATH, {"ConstellationGroupTable": {"groups": groups}})
    write_json(STARS_PATH, {"StarTable": {"stars": list(star_by_hip.values()), "aliases": aliases}})
    write_json(CONSTELLATIONS_PATH, {"ConstellationTable": {"constellations": constellations}})
    named = sum(1 for s in star_by_hip.values() if s["nameEn"])
    board_counts = [len(c["board"]["stars"]) for c in constellations]
    print(f"wrote {len(star_by_hip)} stars ({named} named), {len(constellations)} constellations and {len(groups)} groups")
    print(f"board stars per constellation: min {min(board_counts)}, max {max(board_counts)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())

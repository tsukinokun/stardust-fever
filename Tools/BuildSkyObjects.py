"""ビッグバンで手に入る天体のデータ（StardustFever/Assets/Data/SkyObjects.json）を作る。

    python Tools/BuildSkyObjects.py

恒星の位置と等級は Tools/SkyData/hygdata_v41.csv から引く（無ければ
python Tools/BuildSkyCatalog.py --download で取ってくる）。
"""
import csv
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import SkyObjects as source  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
HYG_PATH = os.path.join(ROOT, "Tools", "SkyData", "hygdata_v41.csv")
TEXTURE_PATH = "StardustFever/Assets/Textures/SkyObjects/{}.png"
OUTPUT_PATH = os.path.join(ROOT, "StardustFever", "Assets", "Data", "SkyObjects.json")


def load_hyg():
    with open(HYG_PATH, encoding="utf-8") as stream:
        return {row["hip"]: row for row in csv.DictReader(stream) if row["hip"]}


def main():
    hyg = load_hyg()
    star_textures = {star: texture for texture, stars in source.STAR_TEXTURES.items() for star in stars}
    objects = []
    ids = set()
    for item in source.OBJECTS:
        if item["id"] in ids:
            raise SystemExit("duplicate id: " + item["id"])
        ids.add(item["id"])

        if "hip" in item:
            row = hyg[str(item["hip"])]
            ra, dec, magnitude = float(row["ra"]), float(row["dec"]), float(row["mag"])
        else:
            ra, dec, magnitude = item["ra"], item["dec"], item["magnitude"]

        objects.append({
            "id": item["id"],
            "name": item["name"],
            "nameEn": item["nameEn"],
            "kind": item["kind"],
            "ra": round(ra, 5),
            "dec": round(dec, 5),
            "magnitude": magnitude,
            "rarity": item["rarity"],
            "caption": item["caption"],
            "texture": TEXTURE_PATH.format(star_textures.get(item["id"], item["id"])),
            "size": source.OBJECT_SIZES.get(item["id"], source.STAR_SIZE),
            "effect": {"type": item["effect"], "value": source.EFFECT_VALUES[item["effect"]][item["rarity"]]},
        })

    with open(OUTPUT_PATH, "w", encoding="utf-8", newline="\n") as stream:
        json.dump({"SkyObjectTable": {"objects": objects}}, stream, ensure_ascii=False, indent=4)
        stream.write("\n")

    counts = [sum(1 for item in objects if item["rarity"] == rarity) for rarity in range(4)]
    print(f"wrote {len(objects)} objects {counts}")


if __name__ == "__main__":
    main()

"""盤面の上の半円の壁のモデル（StardustFever/Assets/Models/Arch.obj）を作る。

    python Tools/GenerateArchModel.py

玉の当たり判定は GenerateBoardLayout.py の短い箱の列のまま。箱をそのまま見せると継ぎ目の角が
外へ飛び出すので、見た目はこの滑らかな1枚のモデルに任せ、箱は描かない。
原点は半円の中心。内側の面は盤面の半径 30、外側の面は左右の外壁の外面にそろえる。
"""
import math
import os

from GenerateBoardLayout import ARC_CENTER, BOARD_HALF_WIDTH, ROOT, WALL_THICKNESS_HALF

OUTPUT_DIR = os.path.join(ROOT, "StardustFever", "Assets", "Models")
INNER_RADIUS = BOARD_HALF_WIDTH
OUTER_RADIUS = BOARD_HALF_WIDTH + WALL_THICKNESS_HALF * 2.0
DEPTH_HALF = 2.0  # Wall Prefab の奥行き
SEGMENTS = 192


def main():
    vertices, normals, faces = [], [], []

    def add(x, y, z, nx, ny, nz):
        vertices.append((x, y, z))
        normals.append((nx, ny, nz))
        return len(vertices)

    def strip(point_at, normal_at):
        """角度に沿った帯。各点は (前の端, 後の端) の2頂点になる。"""
        previous = None
        for i in range(SEGMENTS + 1):
            theta = math.pi * i / SEGMENTS
            near, far = point_at(theta)
            normal = normal_at(theta)
            pair = (add(*near, *normal), add(*far, *normal))
            if previous:
                faces.append((previous[0], pair[0], pair[1]))
                faces.append((previous[0], pair[1], previous[1]))
            previous = pair

    def ring(radius, theta, z):
        return (radius * math.cos(theta), radius * math.sin(theta), z)

    # 手前の面
    strip(lambda t: (ring(INNER_RADIUS, t, -DEPTH_HALF), ring(OUTER_RADIUS, t, -DEPTH_HALF)), lambda t: (0.0, 0.0, -1.0))
    # 内側の面と外側の面
    strip(lambda t: (ring(INNER_RADIUS, t, -DEPTH_HALF), ring(INNER_RADIUS, t, DEPTH_HALF)), lambda t: (-math.cos(t), -math.sin(t), 0.0))
    strip(lambda t: (ring(OUTER_RADIUS, t, -DEPTH_HALF), ring(OUTER_RADIUS, t, DEPTH_HALF)), lambda t: (math.cos(t), math.sin(t), 0.0))

    # 両端のふた（左右の外壁の上に乗る面）
    for x0, x1 in ((INNER_RADIUS, OUTER_RADIUS), (-OUTER_RADIUS, -INNER_RADIUS)):
        corners = [add(x, 0.0, z, 0.0, -1.0, 0.0) for x, z in ((x0, -DEPTH_HALF), (x1, -DEPTH_HALF), (x1, DEPTH_HALF), (x0, DEPTH_HALF))]
        faces.append((corners[0], corners[1], corners[2]))
        faces.append((corners[0], corners[2], corners[3]))

    with open(os.path.join(OUTPUT_DIR, "Arch.obj"), "w", encoding="utf-8", newline="\n") as f:
        f.write("mtllib Arch.mtl\no Arch\n")
        for v in vertices:
            f.write("v %.5f %.5f %.5f\n" % v)
        for n in normals:
            f.write("vn %.5f %.5f %.5f\n" % n)
        f.write("usemtl Arch\n")
        for a, b, c in faces:
            f.write(f"f {a}//{a} {b}//{b} {c}//{c}\n")
    with open(os.path.join(OUTPUT_DIR, "Arch.mtl"), "w", encoding="utf-8", newline="\n") as f:
        # 曲面は照明を一点で強く返すので、Wall より艶を落とす
        f.write("newmtl Arch\nKd 0.3 0.26 0.62\nKs 0 0 0\nKe 0.16 0.13 0.4\nNs 1\nd 1.0\n")
    print(f"wrote Arch.obj ({len(vertices)} vertices, {len(faces)} faces, centre {ARC_CENTER})")


if __name__ == "__main__":
    main()

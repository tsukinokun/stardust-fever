"""BGM を作る（StardustFever/Assets/Audio/Bgm/*.mp3）。

    python Tools/BuildMusic.py --download    元の音源を Tools/MusicData/ へ落としてから作る
    python Tools/BuildMusic.py               手元の音源から作り直す

エンジンは再生位置を指定できないので、場面で聴かせたい部分をここで切り出して別のファイルにする。
切り出しの秒数は、音量の推移を測って決めた（下の TRACKS）。元の音源はリポジトリに入れない。
権利と出典は Docs/Credits.md。ffmpeg が要る。
"""
import argparse
import array
import json
import math
import os
import re
import struct
import subprocess
import sys
import time
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from MusicSources import SOURCES

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DATA_DIR = os.path.join(ROOT, "Tools", "MusicData")
OUTPUT_DIR = os.path.join(ROOT, "StardustFever", "Assets", "Audio", "Bgm")
USER_AGENT = {"User-Agent": "StardustFeverBuildMusic/1.0 (game asset build)"}

# 出力名: (元の音源, [(開始秒, 終了秒), ...], 目標の音量 LUFS)
# 区間が複数なら、つなぎ目を CROSSFADE 秒で重ねる。静かな曲と盛り上がる曲の落差は目標の音量で付ける
TRACKS = {
    "Normal.mp3": ("Venus.ogg", [(0, 240)], -22.0),                 # 通常: ホルスト「金星」の冒頭
    "Map.mp3": ("ClairDeLune.mp3", [(0, 236)], -22.0),              # 星図: ドビュッシー「月の光」
    "Reach.mp3": ("Mars.ogg", [(78, 110)], -16.0),                  # リーチ: ホルスト「火星」の最も強い所から
    "Fever.mp3": ("Jupiter.ogg", [(248, 293), (410, 469)], -16.0),  # フィーバー: 「木星」の賛歌の頂点 → 終結部
}
CHANCE_TRACK = "Chance.mp3"    # 星ゲットチャンス: 「きらきら星変奏曲」のオルゴール
CHANCE_SECONDS = 120.0
CHANCE_LOUDNESS = -19.0
CROSSFADE = 3.0
EDGE_FADE = 1.5                # ループのつなぎ目を隠す
SAMPLE_RATE = 22050


def download():
    os.makedirs(DATA_DIR, exist_ok=True)
    for name, url in SOURCES.items():
        path = os.path.join(DATA_DIR, name)
        if os.path.exists(path):
            continue
        with urllib.request.urlopen(urllib.request.Request(url, headers=USER_AGENT), timeout=120) as response:
            data = response.read()
        with open(path, "wb") as file:
            file.write(data)
        print(f"downloaded {name} ({len(data)} bytes)")
        time.sleep(3)    # Wikimedia の問い合わせ制限にかからないように


def ffmpeg(*args):
    return subprocess.run(["ffmpeg", "-hide_banner", "-y", *args], capture_output=True, text=True, encoding="utf-8", errors="replace", check=True)


def measure_loudness(path):
    stderr = ffmpeg("-i", path, "-af", "loudnorm=print_format=json", "-f", "null", "-").stderr
    return float(json.loads(re.search(r"\{[^{}]*\}", stderr, re.S).group(0))["input_i"])


def finish(temporary, output, loudness):
    """音量を目標に合わせ（強い所は抑え）、両端をフェードして mp3 にする。"""
    gain = loudness - measure_loudness(temporary)
    duration = float(subprocess.run(["ffprobe", "-v", "error", "-show_entries", "format=duration", "-of", "csv=p=0", temporary],
                                    capture_output=True, text=True, check=True).stdout)
    filters = (f"volume={gain:.2f}dB,alimiter=limit=0.89:level=disabled,"
               f"afade=t=in:d={EDGE_FADE},afade=t=out:st={duration - EDGE_FADE:.3f}:d={EDGE_FADE}")
    ffmpeg("-i", temporary, "-af", filters, "-ar", "44100", "-ac", "2", "-b:a", "160k", output)
    print(f"wrote {os.path.basename(output)} ({duration:.0f} s, {gain:+.1f} dB)")


def build_cut(name, source, sections, loudness):
    inputs = []
    for start, end in sections:
        inputs += ["-ss", str(start), "-t", str(end - start), "-i", os.path.join(DATA_DIR, source)]
    temporary = os.path.join(DATA_DIR, "_" + name + ".wav")
    if len(sections) == 1:
        ffmpeg(*inputs, temporary)
    else:
        ffmpeg(*inputs, "-filter_complex", f"[0][1]acrossfade=d={CROSSFADE}", temporary)
    finish(temporary, os.path.join(OUTPUT_DIR, name), loudness)
    os.remove(temporary)


#-------------------------------------------------------------
# オルゴール: パブリックドメインの MIDI を鐘の音で鳴らす
#-------------------------------------------------------------
def read_varlen(data, position):
    value = 0
    while True:
        byte = data[position]
        position += 1
        value = (value << 7) | (byte & 0x7F)
        if not byte & 0x80:
            return value, position


def parse_midi(path):
    """音符を (秒, 音の高さ, 強さ) で返す。"""
    with open(path, "rb") as file:
        data = file.read()
    _, track_count, division = struct.unpack(">HHH", data[8:14])
    position, tempo_events, note_events = 14, [], []
    for _ in range(track_count):
        length = struct.unpack(">I", data[position + 4:position + 8])[0]
        position += 8
        end, tick, status = position + length, 0, 0
        while position < end:
            delta, position = read_varlen(data, position)
            tick += delta
            if data[position] & 0x80:
                status = data[position]
                position += 1
            kind = status & 0xF0
            if status == 0xFF:
                meta = data[position]
                size, position = read_varlen(data, position + 1)
                if meta == 0x51:
                    tempo_events.append((tick, int.from_bytes(data[position:position + 3], "big")))
                position += size
            elif status in (0xF0, 0xF7):
                size, position = read_varlen(data, position)
                position += size
            elif kind in (0xC0, 0xD0):
                position += 1
            else:
                if kind == 0x90 and data[position + 1] > 0:
                    note_events.append((tick, data[position], data[position + 1]))
                position += 2
    tempo_events.sort()
    notes = []
    for tick, pitch, velocity in sorted(note_events):
        seconds, last_tick, tempo = 0.0, 0, 500000
        for change_tick, change in tempo_events:
            if change_tick >= tick:
                break
            seconds += (change_tick - last_tick) * tempo / division / 1e6
            last_tick, tempo = change_tick, change
        notes.append((seconds + (tick - last_tick) * tempo / division / 1e6, pitch, velocity))
    return notes


def bell(pitch):
    """オルゴールの櫛の1音。高い倍音ほど早く消える。"""
    frequency = 440.0 * 2.0 ** ((pitch - 69) / 12.0)
    length = int(SAMPLE_RATE * 1.1)
    partials = [(1.0, 1.0, 3.2), (2.0, 0.35, 6.0), (4.07, 0.16, 11.0), (6.2, 0.06, 18.0)]
    samples = [0.0] * length
    for ratio, level, decay in partials:
        step = 2.0 * math.pi * frequency * ratio / SAMPLE_RATE
        falloff = math.exp(-decay / SAMPLE_RATE)
        amplitude = level
        for index in range(length):
            samples[index] += amplitude * math.sin(step * index)
            amplitude *= falloff
    return samples


def build_chance():
    notes = [note for note in parse_midi(os.path.join(DATA_DIR, "K265.mid")) if note[0] < CHANCE_SECONDS]
    mix = array.array("f", [0.0]) * int(SAMPLE_RATE * (CHANCE_SECONDS + 2.0))
    cache = {}
    for seconds, pitch, velocity in notes:
        pitch += 12    # オルゴールらしく1オクターブ上げる
        if pitch not in cache:
            cache[pitch] = bell(pitch)
        start = int(seconds * SAMPLE_RATE)
        level = 0.12 * velocity / 127.0
        for offset, sample in enumerate(cache[pitch]):
            mix[start + offset] += level * sample
    peak = max(abs(sample) for sample in mix) or 1.0
    pcm = array.array("h", (int(sample / peak * 30000) for sample in mix))
    raw = os.path.join(DATA_DIR, "_chance.pcm")
    temporary = os.path.join(DATA_DIR, "_chance.wav")
    with open(raw, "wb") as file:
        file.write(pcm.tobytes())
    # 小さな部屋の響きを足す
    ffmpeg("-f", "s16le", "-ar", str(SAMPLE_RATE), "-ac", "1", "-i", raw, "-af", "aecho=0.8:0.7:60|120:0.3|0.18", "-t", str(CHANCE_SECONDS), temporary)
    finish(temporary, os.path.join(OUTPUT_DIR, CHANCE_TRACK), CHANCE_LOUDNESS)
    os.remove(raw)
    os.remove(temporary)
    print(f"  {len(notes)} notes")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--download", action="store_true")
    args = parser.parse_args()
    if args.download:
        download()
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    for name, (source, sections, loudness) in TRACKS.items():
        build_cut(name, source, sections, loudness)
    build_chance()


if __name__ == "__main__":
    main()

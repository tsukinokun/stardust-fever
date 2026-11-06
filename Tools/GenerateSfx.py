"""効果音を合成する（StardustFever/Assets/Audio/Sfx/*.wav）。

    python Tools/GenerateSfx.py

星の雰囲気に合わせ、鐘とガラスのような澄んだ音を中心に作る。
エンジンはピッチを変えられないので、釘と星をつなぐ音は高さ違いのファイルにする。
乱数の種を固定しているので、作り直しても同じ音になる。
"""
import array
import math
import os
import random
import wave

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUTPUT_DIR = os.path.join(ROOT, "StardustFever", "Assets", "Audio", "Sfx")
RATE = 44100

BELL = [(1.0, 1.0, 1.0), (2.0, 0.45, 1.8), (3.01, 0.22, 2.8), (5.4, 0.1, 5.0)]    # (倍音, 強さ, 減衰の速さの倍率)
GLASS = [(1.0, 1.0, 1.0), (2.76, 0.4, 2.2), (5.4, 0.18, 4.0)]
SOFT = [(1.0, 1.0, 1.0), (2.0, 0.2, 2.0)]


def note(name):
    """"C5" のような音名を周波数にする。"""
    semitone = {"C": 0, "D": 2, "E": 4, "F": 5, "G": 7, "A": 9, "B": 11}[name[0]]
    octave = int(name[-1])
    if "#" in name:
        semitone += 1
    return 440.0 * 2.0 ** ((semitone + (octave + 1) * 12 - 69) / 12.0)


def silence(seconds):
    return [0.0] * int(seconds * RATE)


def mix(target, source, at=0.0, level=1.0):
    start = int(at * RATE)
    if start + len(source) > len(target):
        target.extend([0.0] * (start + len(source) - len(target)))
    for index, sample in enumerate(source):
        target[start + index] += sample * level
    return target


def tone(frequency, seconds, decay=6.0, partials=BELL, attack=0.002):
    """倍音ごとに減衰する音。"""
    length = int(seconds * RATE)
    samples = [0.0] * length
    for ratio, level, speed in partials:
        step = 2.0 * math.pi * frequency * ratio / RATE
        falloff = math.exp(-decay * speed / RATE)
        amplitude = level
        for index in range(length):
            samples[index] += amplitude * math.sin(step * index)
            amplitude *= falloff
    attack_length = max(1, int(attack * RATE))
    for index in range(min(attack_length, length)):
        samples[index] *= index / attack_length
    return samples


def sweep(start, end, seconds, level=1.0, curve=1.0):
    """高さが滑らかに変わる音。"""
    length = int(seconds * RATE)
    phase, samples = 0.0, []
    for index in range(length):
        progress = index / length
        frequency = start * (end / start) ** (progress ** curve)
        phase += 2.0 * math.pi * frequency / RATE
        envelope = min(1.0, index / (0.01 * RATE)) * min(1.0, (length - index) / (0.05 * RATE))
        samples.append(level * envelope * math.sin(phase))
    return samples


def noise(seconds, decay, cutoff, rng):
    """低域だけ残した雑音（一次のローパス）。"""
    length = int(seconds * RATE)
    coefficient = 1.0 - math.exp(-2.0 * math.pi * cutoff / RATE)
    value, samples = 0.0, []
    for index in range(length):
        value += coefficient * (rng.uniform(-1.0, 1.0) - value)
        samples.append(value * math.exp(-decay * index / RATE))
    return samples


def echo(samples, delay=0.11, feedback=0.35, repeats=4):
    result = list(samples)
    for repeat in range(1, repeats + 1):
        mix(result, samples, delay * repeat, feedback ** repeat)
    return result


def arpeggio(names, gap, seconds=0.7, decay=5.0, partials=BELL):
    result = []
    for index, name in enumerate(names):
        mix(result, tone(note(name), seconds, decay, partials), index * gap)
    return result


def save(name, samples, peak=0.8):
    top = max(abs(sample) for sample in samples) or 1.0
    fade = min(len(samples), int(0.01 * RATE))    # 終わりのプツッという音を防ぐ
    for index in range(fade):
        samples[len(samples) - 1 - index] *= index / fade
    pcm = array.array("h", (int(sample / top * peak * 32767) for sample in samples))
    with wave.open(os.path.join(OUTPUT_DIR, name + ".wav"), "wb") as file:
        file.setnchannels(1)
        file.setsampwidth(2)
        file.setframerate(RATE)
        file.writeframes(pcm.tobytes())


def main():
    os.makedirs(OUTPUT_DIR, exist_ok=True)
    rng = random.Random(11)

    # 釘: 短いガラスの音。五音音階の高さ違いにすると、連続しても濁らない
    for index, name in enumerate(["C6", "D6", "E6", "G6", "A6"]):
        save(f"Pin{index}", tone(note(name), 0.12, 38.0, GLASS), 0.5)

    # 発射: 下から上へ抜ける音
    launch = sweep(180.0, 900.0, 0.16, 0.7, 0.7)
    mix(launch, noise(0.12, 30.0, 2500.0, rng), 0.0, 0.5)
    save("Launch", launch, 0.5)

    # 入賞: 口ごとに響きを変える
    save("PocketStart", arpeggio(["E5", "B5"], 0.06, 0.5, 8.0), 0.7)
    save("PocketTulip", arpeggio(["G5", "D6"], 0.05, 0.4, 10.0, GLASS), 0.65)
    save("PocketGate", echo(arpeggio(["C6", "G6", "C7"], 0.05, 0.5, 8.0, GLASS)), 0.75)

    # リール
    save("ReelStop", tone(note("A4"), 0.12, 30.0, SOFT), 0.6)
    reach = sweep(300.0, 1200.0, 0.5, 0.6, 1.4)
    mix(reach, echo(arpeggio(["A5", "E6"], 0.1, 0.5, 7.0)), 0.4)
    save("Reach", reach, 0.8)
    save("Miss", arpeggio(["E5", "C5"], 0.1, 0.4, 9.0, SOFT), 0.45)

    # 当たり: 明るい上昇のファンファーレ
    win = arpeggio(["C5", "E5", "G5", "C6", "E6", "G6"], 0.07, 1.0, 3.5)
    mix(win, arpeggio(["C4", "G4", "C5"], 0.0, 1.4, 2.5), 0.42)
    mix(win, arpeggio(["C6", "E6", "G6", "C7"], 0.0, 1.5, 2.5, GLASS), 0.42, 0.6)
    save("Win", echo(win, 0.14, 0.3, 3), 0.9)
    save("FeverEnd", echo(arpeggio(["G5", "E5", "C5"], 0.13, 0.9, 4.0)), 0.55)

    # 星ゲットチャンス: つなぐたびに音階を上がる
    save("ChanceStart", echo(arpeggio(["G5", "C6", "E6", "G6", "C7"], 0.05, 0.8, 4.5, GLASS)), 0.8)
    for index, name in enumerate(["C6", "D6", "E6", "G6", "A6", "C7", "D7", "E7"]):
        save(f"StarLink{index}", echo(tone(note(name), 0.5, 9.0, GLASS), 0.09, 0.3, 2), 0.7)
    save("ChanceFail", echo(arpeggio(["D5", "A4"], 0.16, 0.8, 5.0, SOFT)), 0.5)

    # 星を手に入れる・星座がそろう
    save("StarGet", echo(arpeggio(["E6", "G6", "B6", "E7"], 0.08, 1.0, 4.0, GLASS)), 0.8)
    constellation = arpeggio(["C5", "G5", "C6", "E6", "G6", "C7"], 0.1, 1.6, 2.5)
    mix(constellation, arpeggio(["C4", "G4", "E5"], 0.0, 2.0, 1.8, SOFT), 0.5, 0.8)
    save("Constellation", echo(constellation, 0.16, 0.35, 3), 0.9)

    # ビッグバン: 低い地鳴りから光へ抜ける
    bang = noise(2.5, 1.6, 180.0, rng)
    mix(bang, tone(55.0, 2.5, 1.5, SOFT), 0.0, 0.8)
    mix(bang, sweep(200.0, 3000.0, 1.4, 0.25, 2.0), 0.2)
    mix(bang, echo(arpeggio(["C6", "G6", "C7", "G7"], 0.06, 1.4, 3.0, GLASS)), 1.4, 0.5)
    save("BigBang", bang, 0.9)

    # 操作の音
    save("Purchase", arpeggio(["G5", "C6"], 0.07, 0.4, 9.0), 0.6)
    save("Denied", arpeggio(["A3", "A3"], 0.1, 0.12, 25.0, SOFT), 0.5)
    save("MapOpen", sweep(500.0, 1400.0, 0.25, 0.5), 0.4)
    save("MapClose", sweep(1400.0, 500.0, 0.25, 0.5), 0.4)
    save("VolumeTick", tone(note("E6"), 0.08, 40.0, GLASS), 0.4)
    save("UiHover", tone(note("A6"), 0.05, 60.0, GLASS), 0.3)
    save("UiClick", arpeggio(["E6", "A6"], 0.03, 0.12, 30.0, GLASS), 0.5)

    print(f"wrote {len(os.listdir(OUTPUT_DIR))} files to {OUTPUT_DIR}")


if __name__ == "__main__":
    main()

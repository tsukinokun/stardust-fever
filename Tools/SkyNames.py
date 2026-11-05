"""BuildSkyCatalog.py が使う、日本語の名前と星座の効果の表。

星座の和名は学術用語（文部科学省『学術用語集 天文学編』）の表記。
星の固有名は IAU WGSN の名前を、日本でよく使われるカタカナ表記にしたもの。
表に無い固有名は英語のまま出す（BuildSkyCatalog.py が警告する）。
"""

# 星座の略符 → (和名, 英語名)
CONSTELLATIONS = {
    "And": ("アンドロメダ座", "Andromeda"),
    "Ant": ("ポンプ座", "Antlia"),
    "Aps": ("ふうちょう座", "Apus"),
    "Aql": ("わし座", "Aquila"),
    "Aqr": ("みずがめ座", "Aquarius"),
    "Ara": ("さいだん座", "Ara"),
    "Ari": ("おひつじ座", "Aries"),
    "Aur": ("ぎょしゃ座", "Auriga"),
    "Boo": ("うしかい座", "Boötes"),
    "Cae": ("ちょうこくぐ座", "Caelum"),
    "Cam": ("きりん座", "Camelopardalis"),
    "Cap": ("やぎ座", "Capricornus"),
    "Car": ("りゅうこつ座", "Carina"),
    "Cas": ("カシオペヤ座", "Cassiopeia"),
    "Cen": ("ケンタウルス座", "Centaurus"),
    "Cep": ("ケフェウス座", "Cepheus"),
    "Cet": ("くじら座", "Cetus"),
    "Cha": ("カメレオン座", "Chamaeleon"),
    "Cir": ("コンパス座", "Circinus"),
    "CMa": ("おおいぬ座", "Canis Major"),
    "CMi": ("こいぬ座", "Canis Minor"),
    "Cnc": ("かに座", "Cancer"),
    "Col": ("はと座", "Columba"),
    "Com": ("かみのけ座", "Coma Berenices"),
    "CrA": ("みなみのかんむり座", "Corona Australis"),
    "CrB": ("かんむり座", "Corona Borealis"),
    "Crt": ("コップ座", "Crater"),
    "Cru": ("みなみじゅうじ座", "Crux"),
    "Crv": ("からす座", "Corvus"),
    "CVn": ("りょうけん座", "Canes Venatici"),
    "Cyg": ("はくちょう座", "Cygnus"),
    "Del": ("いるか座", "Delphinus"),
    "Dor": ("かじき座", "Dorado"),
    "Dra": ("りゅう座", "Draco"),
    "Equ": ("こうま座", "Equuleus"),
    "Eri": ("エリダヌス座", "Eridanus"),
    "For": ("ろ座", "Fornax"),
    "Gem": ("ふたご座", "Gemini"),
    "Gru": ("つる座", "Grus"),
    "Her": ("ヘルクレス座", "Hercules"),
    "Hor": ("とけい座", "Horologium"),
    "Hya": ("うみへび座", "Hydra"),
    "Hyi": ("みずへび座", "Hydrus"),
    "Ind": ("インディアン座", "Indus"),
    "Lac": ("とかげ座", "Lacerta"),
    "Leo": ("しし座", "Leo"),
    "Lep": ("うさぎ座", "Lepus"),
    "Lib": ("てんびん座", "Libra"),
    "LMi": ("こじし座", "Leo Minor"),
    "Lup": ("おおかみ座", "Lupus"),
    "Lyn": ("やまねこ座", "Lynx"),
    "Lyr": ("こと座", "Lyra"),
    "Men": ("テーブルさん座", "Mensa"),
    "Mic": ("けんびきょう座", "Microscopium"),
    "Mon": ("いっかくじゅう座", "Monoceros"),
    "Mus": ("はえ座", "Musca"),
    "Nor": ("じょうぎ座", "Norma"),
    "Oct": ("はちぶんぎ座", "Octans"),
    "Oph": ("へびつかい座", "Ophiuchus"),
    "Ori": ("オリオン座", "Orion"),
    "Pav": ("くじゃく座", "Pavo"),
    "Peg": ("ペガスス座", "Pegasus"),
    "Per": ("ペルセウス座", "Perseus"),
    "Phe": ("ほうおう座", "Phoenix"),
    "Pic": ("がか座", "Pictor"),
    "PsA": ("みなみのうお座", "Piscis Austrinus"),
    "Psc": ("うお座", "Pisces"),
    "Pup": ("とも座", "Puppis"),
    "Pyx": ("らしんばん座", "Pyxis"),
    "Ret": ("レチクル座", "Reticulum"),
    "Scl": ("ちょうこくしつ座", "Sculptor"),
    "Sco": ("さそり座", "Scorpius"),
    "Sct": ("たて座", "Scutum"),
    "Ser": ("へび座", "Serpens"),
    "Sex": ("ろくぶんぎ座", "Sextans"),
    "Sge": ("や座", "Sagitta"),
    "Sgr": ("いて座", "Sagittarius"),
    "Tau": ("おうし座", "Taurus"),
    "Tel": ("ぼうえんきょう座", "Telescopium"),
    "TrA": ("みなみのさんかく座", "Triangulum Australe"),
    "Tri": ("さんかく座", "Triangulum"),
    "Tuc": ("きょしちょう座", "Tucana"),
    "UMa": ("おおぐま座", "Ursa Major"),
    "UMi": ("こぐま座", "Ursa Minor"),
    "Vel": ("ほ座", "Vela"),
    "Vir": ("おとめ座", "Virgo"),
    "Vol": ("とびうお座", "Volans"),
    "Vul": ("こぎつね座", "Vulpecula"),
}

# 星座の固有の効果。星座の星を全部そろえると発動する。
#   global        全体倍率 ×(1 + 値)
#   pinPayout     釘の星屑 ×(1 + 値)
#   pocketPayout  入賞の星屑 ×(1 + 値)
#   lotteryChance フィーバーの当選確率 + 値
#   starChance    星ゲットチャンスの当選確率 + 値
#   feverDuration フィーバーの時間 + 値（秒）
ZODIAC = {"Ari", "Tau", "Gem", "Cnc", "Leo", "Vir", "Lib", "Sco", "Sgr", "Cap", "Aqr", "Psc"}
WATER = {"Cet", "Del", "PsA", "Hya", "Hyi", "Dor", "Vol", "Car", "Pup", "Vel", "Eri"}
INSTRUMENTS = {"Ant", "Cae", "Cir", "For", "Hor", "Mic", "Men", "Nor", "Oct", "Pic", "Pyx", "Ret", "Scl", "Sct",
               "Sex", "Tel", "Lyr", "Crt", "Sge", "TrA", "Tri", "Ara", "CrA", "CrB"}
BIRDS = {"Aql", "Col", "Crv", "Cyg", "Gru", "Pav", "Phe", "Tuc", "Aps"}
PEOPLE = {"Ori", "Per", "Her", "Boo", "Oph", "Aur", "Cep", "Cas", "And", "Cen", "Ind", "Com", "Peg"}

EFFECT_BASE = {
    "lotteryChance": 0.004,
    "pocketPayout": 0.15,
    "pinPayout": 0.15,
    "starChance": 0.01,
    "feverDuration": 1.0,
    "global": 0.05,
}


def constellation_effect(abbr):
    if abbr in ZODIAC:
        kind = "lotteryChance"
    elif abbr in WATER:
        kind = "pocketPayout"
    elif abbr in INSTRUMENTS:
        kind = "pinPayout"
    elif abbr in BIRDS:
        kind = "starChance"
    elif abbr in PEOPLE:
        kind = "feverDuration"
    else:
        kind = "global"
    return kind, EFFECT_BASE[kind]


GREEK = {
    "Alp": "α", "Bet": "β", "Gam": "γ", "Del": "δ", "Eps": "ε", "Zet": "ζ", "Eta": "η", "The": "θ",
    "Iot": "ι", "Kap": "κ", "Lam": "λ", "Mu": "μ", "Nu": "ν", "Xi": "ξ", "Omi": "ο", "Pi": "π",
    "Rho": "ρ", "Sig": "σ", "Tau": "τ", "Ups": "υ", "Phi": "φ", "Chi": "χ", "Psi": "ψ", "Ome": "ω",
}

# IAU の固有名 → カタカナ
STAR_NAMES = {
    "Acamar": "アカマル", "Achernar": "アケルナル", "Acrab": "アクラブ", "Acrux": "アクルックス",
    "Acubens": "アクベンス", "Adhafera": "アダフェラ", "Adhara": "アダーラ", "Ain": "アイン",
    "Albali": "アルバリ", "Albireo": "アルビレオ", "Alchiba": "アルキバ", "Aldebaran": "アルデバラン",
    "Alderamin": "アルデラミン", "Aldhanab": "アルダナブ", "Aldhibah": "アルディバ", "Aldulfin": "アルドゥルフィン",
    "Alfirk": "アルフィルク", "Algedi": "アルゲディ", "Algenib": "アルゲニブ", "Algieba": "アルギエバ",
    "Algol": "アルゴル", "Algorab": "アルゴラブ", "Alhena": "アルヘナ", "Alioth": "アリオト",
    "Aljanah": "アルジャナー", "Alkaid": "アルカイド", "Alkaphrah": "アルカフラー", "Alkes": "アルケス",
    "Almach": "アルマク", "Alnair": "アルナイル", "Alnasl": "アルナスル", "Alnilam": "アルニラム",
    "Alnitak": "アルニタク", "Alphard": "アルファルド", "Alphecca": "アルフェッカ", "Alpheratz": "アルフェラッツ",
    "Alpherg": "アルフェルグ", "Alrescha": "アルレシャ", "Alsciaukat": "アルシャウカト", "Alsephina": "アルセフィナ",
    "Alshain": "アルシャイン", "Altair": "アルタイル", "Altais": "アルタイス", "Aludra": "アルドラ",
    "Alya": "アリヤ", "Alzirr": "アルジル", "Ancha": "アンカ", "Ankaa": "アンカア",
    "Anser": "アンセル", "Antares": "アンタレス", "Arcturus": "アークトゥルス", "Arkab Posterior": "アルカブ・ポステリオル",
    "Arneb": "アルネブ", "Ascella": "アスケラ", "Asellus Australis": "アセルス・アウストラリス",
    "Asellus Borealis": "アセルス・ボレアリス", "Ashlesha": "アシュレーシャ", "Aspidiske": "アスピディスケ",
    "Athebyne": "アテビネ", "Atik": "アティク", "Atlas": "アトラス", "Atria": "アトリア",
    "Avior": "アヴィオール", "Azha": "アザー", "Baten Kaitos": "バテン・カイトス", "Beemim": "ベエミム",
    "Bellatrix": "ベラトリックス", "Betelgeuse": "ベテルギウス", "Bharani": "バラニ", "Biham": "ビハム",
    "Brachium": "ブラキウム", "Canopus": "カノープス", "Capella": "カペラ", "Caph": "カフ",
    "Castor": "カストル", "Cebalrai": "ケバルライ", "Chamukuy": "チャムクイ", "Chara": "カラ",
    "Chertan": "シェルタン", "Cor Caroli": "コル・カロリ", "Cursa": "クルサ", "Dabih": "ダビー",
    "Dalim": "ダリム", "Deneb": "デネブ", "Deneb Algedi": "デネブ・アルゲディ", "Denebola": "デネボラ",
    "Diadem": "ディアデム", "Diphda": "ディフダ", "Dschubba": "ジュバ", "Dubhe": "ドゥーベ",
    "Edasich": "エダシク", "Elnath": "エルナト", "Eltanin": "エルタニン", "Enif": "エニフ",
    "Errai": "エライ", "Fang": "ファン", "Fawaris": "ファワリス", "Fomalhaut": "フォーマルハウト",
    "Furud": "フルド", "Gacrux": "ガクルックス", "Giausar": "ギアウサル", "Gienah": "ギェナー",
    "Gomeisa": "ゴメイサ", "Grumium": "グルミウム", "Gudja": "グジャ", "Hadar": "ハダル",
    "Hamal": "ハマル", "Hassaleh": "ハッサレー", "Heze": "ヘゼ", "Homam": "ホマム",
    "Imai": "イマイ", "Izar": "イザール", "Kaffaljidhma": "カッファルジドマ", "Kang": "カン",
    "Kaus Australis": "カウス・アウストラリス", "Kaus Borealis": "カウス・ボレアリス", "Kaus Media": "カウス・メディア",
    "Kitalpha": "キタルファ", "Kochab": "コカブ", "Kornephoros": "コルネフォロス", "Kraz": "クラズ",
    "Larawag": "ララワグ", "Maasym": "マアシム", "Mahasim": "マハシム", "Markab": "マルカブ",
    "Markeb": "マルケブ", "Matar": "マタル", "Mebsuta": "メブスタ", "Megrez": "メグレズ",
    "Meissa": "メイサ", "Mekbuda": "メクブダ", "Menkalinan": "メンカリナン", "Menkar": "メンカル",
    "Menkent": "メンケント", "Menkib": "メンキブ", "Merak": "メラク", "Meridiana": "メリディアナ",
    "Mesarthim": "メサルティム", "Miaplacidus": "ミアプラキドゥス", "Mimosa": "ミモザ", "Minchir": "ミンキル",
    "Minelauva": "ミネラウバ", "Mintaka": "ミンタカ", "Mira": "ミラ", "Mirach": "ミラク",
    "Miram": "ミラム", "Mirfak": "ミルファク", "Mirzam": "ミルザム", "Mizar": "ミザール",
    "Mothallah": "モサラー", "Muliphein": "ムリフェイン", "Muphrid": "ムフリッド", "Muscida": "ムシダ",
    "Naos": "ナオス", "Nashira": "ナシラ", "Nekkar": "ネッカル", "Nganurganity": "ンガヌルガニティ",
    "Nihal": "ニハル", "Nunki": "ヌンキ", "Nusakan": "ヌサカン", "Okab": "オカブ",
    "Paikauhale": "パイカウハレ", "Peacock": "ピーコック", "Phact": "ファクト", "Phecda": "フェクダ",
    "Pherkad": "フェルカド", "Polaris": "ポラリス", "Polis": "ポリス", "Pollux": "ポルックス",
    "Porrima": "ポリマ", "Praecipua": "プラエキプア", "Prima Hyadum": "プリマ・ヒアドゥム", "Procyon": "プロキオン",
    "Propus": "プロプス", "Ran": "ラン", "Rana": "ラナ", "Rasalas": "ラサラス",
    "Rasalhague": "ラスアルハゲ", "Rastaban": "ラスタバン", "Regulus": "レグルス", "Rigel": "リゲル",
    "Rigil Kentaurus": "リギル・ケンタウルス", "Rotanev": "ロタネフ", "Ruchbah": "ルクバー", "Rukbat": "ルクバト",
    "Sabik": "サビク", "Saclateni": "サクラテニ", "Sadachbia": "サダクビア", "Sadalbari": "サダルバリ",
    "Sadalmelik": "サダルメリク", "Sadalsuud": "サダルスウド", "Sadr": "サドル", "Saiph": "サイフ",
    "Sargas": "サルガス", "Sarin": "サリン", "Sceptrum": "スケプトルム", "Scheat": "シェアト",
    "Schedar": "シェダル", "Secunda Hyadum": "セクンダ・ヒアドゥム", "Segin": "セギン", "Seginus": "セギヌス",
    "Sham": "シャム", "Shaula": "シャウラ", "Sheliak": "シェリアク", "Sheratan": "シェラタン",
    "Sirius": "シリウス", "Skat": "スカト", "Spica": "スピカ", "Sualocin": "スアロキン",
    "Suhail": "スハイル", "Sulafat": "スラファト", "Syrma": "シルマ", "Tabit": "タビト",
    "Taiyangshou": "タイヤンショウ", "Talitha": "タリタ", "Tania Australis": "タニア・アウストラリス",
    "Tania Borealis": "タニア・ボレアリス", "Tarazed": "タラゼド", "Tarf": "タルフ", "Tejat": "テジャト",
    "Theemin": "テーミン", "Thuban": "トゥバン", "Tiaki": "ティアキ", "Tianguan": "ティエングアン",
    "Torcular": "トルクラリス", "Tureis": "トゥレイス", "Unukalhai": "ウヌクアルハイ", "Vega": "ベガ",
    "Vindemiatrix": "ビンデミアトリックス", "Wasat": "ワサト", "Wazn": "ワズン", "Wezen": "ウェズン",
    "Wurren": "ウレン", "Xamidimura": "ザミディムラ", "Yed Posterior": "イェド・ポステリオル",
    "Yed Prior": "イェド・プリオル", "Yildun": "イルドゥン", "Zaniah": "ザニア", "Zhang": "ジャン",
    "Zibal": "ジバル", "Zosma": "ゾスマ", "Zubenelgenubi": "ズベンエルゲヌビ", "Zubenelhakrabi": "ズベンエルハクラビ",
    "Zubeneschamali": "ズベンエスカマリ",
}

# 今のセーブに入っている星の id → HIP 番号（v1 のセーブを移し替えるため）
LEGACY_STAR_IDS = {
    "betelgeuse": 27989, "rigel": 24436, "bellatrix": 25336, "mintaka": 25930, "alnilam": 26311,
    "alnitak": 26727, "saiph": 27366, "castor": 36850, "pollux": 37826, "mebsuta": 32246,
    "tejat": 30343, "wasat": 35550, "alhena": 31681,
}

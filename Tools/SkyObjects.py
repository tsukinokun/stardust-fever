"""ビッグバンで手に入る天体の一覧。Tools/BuildSkyObjects.py が SkyObjects.json を作る。

星座の線に入っていない有名な星と、星団・星雲・銀河を集めた。
文章はゲームのために書き下ろしたもの。

hip があるものは、位置と等級を HYG Database から引く。無いものは ra（時）・dec（度）・
magnitude を手で書く。rarity は 0: コモン、1: レア、2: スーパーレア、3: レジェンド。
"""

# (効果の種類, レア度) → 効果の量。星座の効果（Constellations.json）を基準に、
# コモンは約1/3、レアは約2/3、スーパーレアは1.5倍、レジェンドは4倍にしてある
EFFECT_VALUES = {
    "pinPayout": (0.05, 0.1, 0.2, 0.6),
    "pocketPayout": (0.05, 0.1, 0.2, 0.6),
    "global": (0.02, 0.04, 0.08, 0.2),
    "lotteryChance": (0.001, 0.002, 0.005, 0.015),
    "starChance": (0.003, 0.006, 0.015, 0.04),
    "feverDuration": (0.5, 1.0, 2.0, 4.0),
}

# 恒星の絵（色）。無ければ天体の id と同じ名前の絵を使う
STAR_TEXTURES = {
    "star_red": ("lalande", "kapteyn", "ross128", "luyten", "wolf359", "proxima", "barnard", "gj581", "trappist1"),
    "star_orange": ("cyg61", "groombridge"),
    "star_yellow": ("helvetios", "delcep", "rcrb"),
    "star_white": ("alcor", "vanmaanen", "sigoct"),
    "star_blue": ("alcyone", "maia", "electra", "merope", "taygeta", "pcyg"),
}

# 星図での大きさ（UI のピクセル）。無ければ恒星の大きさ
STAR_SIZE = 44.0
OBJECT_SIZES = {
    "m42": 120.0, "m8": 100.0, "m1": 80.0, "m57": 70.0, "m27": 80.0, "coalsack": 170.0, "etacar": 90.0,
    "m45": 100.0, "hyades": 130.0, "m44": 90.0, "m7": 90.0, "doublecluster": 100.0, "m13": 80.0, "omegacen": 100.0,
    "m31": 190.0, "lmc": 170.0, "smc": 120.0, "sgra": 90.0, "cygx1": 90.0,
}

OBJECTS = [
    # コモン
    dict(id="alcor", hip=65477, name="アルコル", nameEn="Alcor", kind="star", rarity=0, effect="pinPayout",
         caption="北斗七星の柄にあるミザールのすぐそばに寄り添う4等星です。古代アラビアでは、この星が見えるかどうかで視力を試したと伝えられます。ミザールと一緒に、肉眼で見える二重星として親しまれてきました。"),
    dict(id="helvetios", hip=113357, name="ヘルウェティオス", nameEn="Helvetios", kind="star", rarity=0, effect="global",
         caption="ペガスス座51番星とも呼ばれる、太陽によく似た星です。1995年、太陽以外の普通の星を回る惑星がこの星で初めて見つかりました。発見者は2019年のノーベル物理学賞を受けています。"),
    dict(id="cyg61", hip=104214, name="はくちょう座61番星", nameEn="61 Cygni", kind="star", rarity=0, effect="feverDuration",
         caption="1838年、ベッセルが年周視差を測り、恒星までの距離が初めてわかった星です。空の上を動く速さが大きいことから「飛翔星」とも呼ばれました。約11光年先にある、橙色の二重星です。"),
    dict(id="lalande", hip=54035, name="ラランド21185", nameEn="Lalande 21185", kind="star", rarity=0, effect="pocketPayout",
         caption="おおぐま座にある赤色矮星で、太陽から約8光年の近さにあります。暗くて肉眼では見えませんが、北天で見える恒星の中では特に近い星です。フランスの天文学者ラランドの星表に名前が残ります。"),
    dict(id="kapteyn", hip=24186, name="カプタイン星", nameEn="Kapteyn's Star", kind="star", rarity=0, effect="starChance",
         caption="がか座にある赤色矮星で、空の上を動く速さが全天で2番目に大きい星です。銀河の回転とは逆向きに回っていて、別の小さな銀河から来た星だと考えられています。"),
    dict(id="groombridge", hip=57939, name="グルームブリッジ1830", nameEn="Groombridge 1830", kind="star", rarity=0, effect="lotteryChance",
         caption="おおぐま座にある古い星で、銀河の円盤を突き抜けるように高速で動いています。19世紀に見つかった当時は、空の上を動く速さが最も大きい星でした。金属の少ない、宇宙の古い世代の星です。"),
    dict(id="ross128", hip=57548, name="ロス128", nameEn="Ross 128", kind="star", rarity=0, effect="pinPayout",
         caption="おとめ座にある穏やかな赤色矮星です。約11光年先にあり、地球ほどの大きさの惑星が見つかっています。赤色矮星には珍しく激しい爆発が少なく、生命を探す候補として注目されています。"),
    dict(id="luyten", hip=36208, name="ライテン星", nameEn="Luyten's Star", kind="star", rarity=0, effect="pocketPayout",
         caption="こいぬ座にある赤色矮星で、約12光年先にあります。2017年に宇宙人へ向けて音楽などの信号が送られた星で、返事が届くとしても四半世紀ほど先のことになります。"),
    dict(id="vanmaanen", hip=3829, name="ファン・マーネン星", nameEn="Van Maanen's Star", kind="star", rarity=0, effect="global",
         caption="うお座にある白色矮星で、単独で輝く白色矮星としては太陽に最も近い星です。太陽ほどの重さが地球ほどの大きさに詰まっています。太陽も遠い未来にこのような姿になります。"),
    dict(id="delcep", hip=110991, name="ケフェウス座δ", nameEn="Delta Cephei", kind="star", rarity=0, effect="lotteryChance",
         caption="約5.4日の周期で明るさを変える変光星です。この仲間は周期から本当の明るさがわかるため、宇宙の距離を測るものさしになりました。遠い銀河までの距離もこの星の仲間で測られました。"),
    dict(id="sigoct", hip=104382, name="はちぶんぎ座σ", nameEn="Polaris Australis", kind="star", rarity=0, effect="starChance",
         caption="天の南極に最も近い肉眼の星で、南の北極星とも呼ばれます。ただし5等星とかなり暗く、北のポラリスのような目印にはなりません。南半球の空は、この星のまわりを回ります。"),
    dict(id="rcrb", hip=77442, name="かんむり座R", nameEn="R Coronae Borealis", kind="star", rarity=0, effect="feverDuration",
         caption="ふだんは6等星ですが、ある日突然暗くなる変わった星です。星のまわりに炭素のすすが固まって光を隠すためで、数か月かけて元に戻ります。明るくなるのではなく暗くなる、逆の新星です。"),
    dict(id="alcyone", hip=17702, name="アルキオネ", nameEn="Alcyone", kind="star", rarity=0, effect="pinPayout",
         caption="すばる（プレアデス星団）で一番明るい星です。名前はギリシャ神話のプレアデス七姉妹の一人に由来します。青白い巨星で、星団の中心近くに輝きます。"),
    dict(id="maia", hip=17573, name="マイア", nameEn="Maia", kind="star", rarity=0, effect="pocketPayout",
         caption="すばるの星の一つで、七姉妹の長女とされます。神々の使いヘルメスの母です。5月（May）の名前の由来ともいわれます。星のまわりを青い反射星雲が包んでいます。"),
    dict(id="electra", hip=17499, name="エレクトラ", nameEn="Electra", kind="star", rarity=0, effect="global",
         caption="すばるの星の一つです。七姉妹の中で一人だけ姿を消したとする伝説があり、物語によってはこの星がその一人とされます。肉眼で見えるすばるの星が6つほどなのは、この伝説と結び付けられてきました。"),
    dict(id="merope", hip=17608, name="メローペ", nameEn="Merope", kind="star", rarity=0, effect="feverDuration",
         caption="すばるの星の一つで、南側に広がる反射星雲をまとっています。七姉妹の中で一人だけ人間の王と結婚したため、恥じて光が弱いという神話があります。"),
    dict(id="taygeta", hip=17531, name="タイゲタ", nameEn="Taygeta", kind="star", rarity=0, effect="lotteryChance",
         caption="すばるの星の一つで、スパルタの近くのタイゲトス山の名の由来となった女神です。実際には互いのまわりを回る連星で、2つの星の光が合わさって見えています。"),
    dict(id="wolf359", ra=10.9414, dec=7.0147, magnitude=13.5, name="ウォルフ359", nameEn="Wolf 359", kind="star", rarity=0, effect="starChance",
         caption="しし座にある赤色矮星で、太陽から約8光年の近さにあります。太陽の1割ほどの重さしかなく、望遠鏡なしでは見えません。近い星として物語の舞台に選ばれることもあります。"),

    # レア
    dict(id="proxima", hip=70890, name="プロキシマ・ケンタウリ", nameEn="Proxima Centauri", kind="star", rarity=1, effect="global",
         caption="太陽に最も近い恒星で、約4.2光年先にあります。名前はラテン語で「最も近い」の意味です。暗い赤色矮星で肉眼では見えません。地球に似た大きさの惑星が回っていることがわかっています。"),
    dict(id="barnard", hip=87937, name="バーナード星", nameEn="Barnard's Star", kind="star", rarity=1, effect="pinPayout",
         caption="空の上を動く速さが全天で最も大きい星です。1年で月の直径の約200分の1ほど動き、肉眼には見えませんが、約6光年先で太陽系のすぐ近くを通り過ぎている最中です。"),
    dict(id="pcyg", hip=100044, name="はくちょう座P", nameEn="P Cygni", kind="star", rarity=1, effect="feverDuration",
         caption="1600年に突然3等星まで明るくなって記録された星です。太陽の数十万倍も明るい青色超巨星で、強い風でガスを噴き出しています。その光の特徴は、同じ仲間の星を調べる手本になりました。"),
    dict(id="gj581", hip=74995, name="グリーゼ581", nameEn="Gliese 581", kind="star", rarity=1, effect="starChance",
         caption="てんびん座にある赤色矮星で、複数の惑星が見つかっています。生命が住める領域の惑星の候補として話題になりました。地球からこの星へメッセージが送られたこともあります。"),
    dict(id="trappist1", ra=23.1065, dec=-5.0414, magnitude=18.8, name="トラピスト1", nameEn="TRAPPIST-1", kind="star", rarity=1, effect="lotteryChance",
         caption="みずがめ座にある小さな赤色矮星で、地球ほどの大きさの惑星が7つも回っています。そのうち数個は水が液体で存在できる範囲にあります。40光年ほど先にある、生命探しの注目の星です。"),
    dict(id="m45", ra=3.79, dec=24.12, magnitude=1.6, name="プレアデス星団", nameEn="Pleiades", kind="cluster", rarity=1, effect="pinPayout",
         caption="日本では「すばる」の名で親しまれる、おうし座の散開星団です。清少納言も「星はすばる」と書きました。生まれて1億年ほどの若い星が、青い光で輝いています。"),
    dict(id="hyades", ra=4.45, dec=15.87, magnitude=0.5, name="ヒアデス星団", nameEn="Hyades", kind="cluster", rarity=1, effect="pocketPayout",
         caption="おうし座の顔にあたるV字形の星団で、地球に最も近い散開星団の一つです。約150光年先にあります。距離が正確に測れるので、宇宙の距離の物差しの土台として使われてきました。"),
    dict(id="m44", ra=8.67, dec=19.67, magnitude=3.7, name="プレセペ星団", nameEn="Beehive Cluster", kind="cluster", rarity=1, effect="pocketPayout",
         caption="かに座の中央にある散開星団で、英語では「蜂の巣」と呼ばれます。肉眼ではぼんやりした雲に見え、古代から知られていました。望遠鏡を初めて向けたガリレオが、星の集まりだと確かめました。"),
    dict(id="doublecluster", ra=2.33, dec=57.13, magnitude=4.3, name="ペルセウス座の二重星団", nameEn="Double Cluster", kind="cluster", rarity=1, effect="global",
         caption="ペルセウス座とカシオペヤ座の間に、2つの星団が並んでいます。どちらも約7500光年先にあり、肉眼でも見えるほど明るく輝く、若い星の大きな集まりです。小さな双眼鏡で見ると見事です。"),
    dict(id="m7", ra=17.90, dec=-34.80, magnitude=3.3, name="トレミー星団", nameEn="Ptolemy Cluster", kind="cluster", rarity=1, effect="starChance",
         caption="さそり座の尾にある明るい散開星団です。西暦130年ごろに天文学者プトレマイオス（トレミー）が記録し、その名が付きました。肉眼で見える星団として、古くから知られていました。"),
    dict(id="m8", ra=18.06, dec=-24.38, magnitude=6.0, name="干潟星雲", nameEn="Lagoon Nebula", kind="nebula", rarity=1, effect="feverDuration",
         caption="いて座にある、星が生まれている巨大なガスの雲です。中央を横切る暗い帯が、潮が引いた干潟のように見えることからこの名があります。条件が良ければ肉眼でもぼんやり見えます。"),
    dict(id="m27", ra=19.99, dec=22.72, magnitude=7.5, name="亜鈴状星雲", nameEn="Dumbbell Nebula", kind="nebula", rarity=1, effect="lotteryChance",
         caption="こぎつね座にある惑星状星雲で、星の一生の終わりにガスが広がった姿です。鉄アレイのような形からこの名が付きました。1764年に初めて見つかった惑星状星雲です。"),
    dict(id="coalsack", ra=12.85, dec=-63.0, magnitude=9.0, name="コールサック", nameEn="Coalsack Nebula", kind="nebula", rarity=1, effect="global",
         caption="南十字星のすぐそばで、天の川にぽっかり穴が開いたように見える暗黒星雲です。名前は「石炭袋」の意味です。冷たいちりが後ろの星の光を隠し、南半球では肉眼ではっきり見えます。"),

    # スーパーレア
    dict(id="m42", ra=5.59, dec=-5.39, magnitude=4.0, name="オリオン大星雲", nameEn="Orion Nebula", kind="nebula", rarity=2, effect="feverDuration",
         caption="オリオン座の三つ星の下にある、肉眼でも見える星の誕生の現場です。約1300光年先の巨大なガスの雲の中で、今まさに星が生まれています。冬の空で一番有名な星雲です。"),
    dict(id="etacar", ra=10.7514, dec=-59.6844, magnitude=4.3, name="りゅうこつ座η", nameEn="Eta Carinae", kind="star", rarity=2, effect="global",
         caption="太陽の100倍を超える重さの星を含む、銀河でも指折りの巨星です。1840年代に大爆発を起こし、一時は全天で2番目に明るくなりました。いつか超新星になると考えられています。"),
    dict(id="cygx1", hip=98298, name="はくちょう座X-1", nameEn="Cygnus X-1", kind="other", rarity=2, effect="pocketPayout",
         caption="ブラックホールとして初めて広く認められた天体です。青い超巨星のガスを吸い込み、強いX線を放っています。ホーキングが「ブラックホールではない」方に賭け、後に負けを認めた話が有名です。"),
    dict(id="m1", ra=5.58, dec=22.01, magnitude=8.4, name="かに星雲", nameEn="Crab Nebula", kind="nebula", rarity=2, effect="pinPayout",
         caption="1054年に爆発した超新星の残骸です。藤原定家の『明月記』にもこの時の「客星」の記録が残ります。中心には1秒に約30回転する中性子星があり、今も星雲を光らせています。"),
    dict(id="m57", ra=18.89, dec=33.03, magnitude=8.8, name="環状星雲", nameEn="Ring Nebula", kind="nebula", rarity=2, effect="lotteryChance",
         caption="こと座にある、輪の形をした惑星状星雲です。太陽のような星が一生の終わりに外側のガスを放った姿で、輪の中心には白色矮星が残っています。小さな望遠鏡でも輪の形が見えます。"),
    dict(id="m13", ra=16.69, dec=36.46, magnitude=5.8, name="ヘルクレス座球状星団", nameEn="Hercules Cluster", kind="cluster", rarity=2, effect="starChance",
         caption="数十万の古い星が球のように集まった星団で、北半球で最も見事な球状星団です。1974年、この星団へ向けて電波のメッセージが送られました。届くのは約2万5千年後です。"),
    dict(id="omegacen", ra=13.45, dec=-47.48, magnitude=3.9, name="オメガ星団", nameEn="Omega Centauri", kind="cluster", rarity=2, effect="pinPayout",
         caption="銀河で最大の球状星団で、約1000万個の星が集まっています。プトレマイオスは星として記録しました。銀河に飲み込まれた小さな銀河の中心が残ったものとも考えられています。"),
    dict(id="smc", ra=0.88, dec=-72.8, magnitude=2.7, name="小マゼラン雲", nameEn="Small Magellanic Cloud", kind="galaxy", rarity=2, effect="global",
         caption="天の川銀河のそばを回る小さな銀河で、南半球では肉眼で雲のように見えます。名前はマゼランの世界一周の航海から付きました。この銀河の変光星が、宇宙の距離を測る鍵になりました。"),

    # レジェンド
    dict(id="m31", ra=0.71, dec=41.27, magnitude=3.4, name="アンドロメダ銀河", nameEn="Andromeda Galaxy", kind="galaxy", rarity=3, effect="global",
         caption="肉眼で見える最も遠い天体の一つで、約250万光年先の巨大な渦巻銀河です。1兆個ほどの星が集まっています。天の川銀河と近づき合っていて、数十億年後には一つになるといわれます。"),
    dict(id="lmc", ra=5.39, dec=-69.76, magnitude=0.9, name="大マゼラン雲", nameEn="Large Magellanic Cloud", kind="galaxy", rarity=3, effect="feverDuration",
         caption="天の川銀河の最大の伴銀河で、約16万光年先にあります。1987年にここで起きた超新星では、ニュートリノが日本のカミオカンデで捉えられ、小柴昌俊のノーベル賞につながりました。"),
    dict(id="sgra", ra=17.7611, dec=-29.0078, magnitude=30.0, name="いて座A*", nameEn="Sagittarius A*", kind="other", rarity=3, effect="pinPayout",
         caption="天の川銀河の中心にある、太陽の約400万倍の重さの巨大ブラックホールです。まわりの星の動きからその存在が確かめられ、2020年のノーベル物理学賞になりました。2022年にはその影の姿が写されました。"),
]

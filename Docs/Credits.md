# クレジット

## 天体データ

`StardustFever/Assets/Data/Stars.json` と `Constellations.json` は、次のデータから `Tools/BuildSkyCatalog.py` で作った。
元データ（`Tools/SkyData/`）はリポジトリに入れていない。`python Tools/BuildSkyCatalog.py --download` で取ってくる。

| データ | 使ったもの | 出典 | ライセンス |
|---|---|---|---|
| 星座の線（88星座） | Stellarium の modern sky culture（`index.json`） | https://github.com/Stellarium/stellarium/tree/master/skycultures/modern | CC BY-SA 4.0 |
| 星の位置・等級・スペクトル型・バイエル符号 | HYG Database v4.1（`hygdata_v41.csv`）、astronexus | https://github.com/astronexus/HYG-Database | CC BY-SA 4.0 |
| 星の固有名 | IAU WGSN の星名一覧（`IAU-CSN.txt`）、E. Mamajek | https://www.pas.rochester.edu/~emamajek/WGSN/IAU-CSN.txt | IAU の星名はパブリックドメイン |

- 2つの CC BY-SA 4.0 のデータから作ったので、`Stars.json` と `Constellations.json` も **CC BY-SA 4.0** で扱う（ゲームのコードには及ばない）
- 星座の和名は学術用語（文部科学省『学術用語集 天文学編』）、星の固有名のカタカナ表記は日本でよく使われる表記に合わせた（`Tools/SkyNames.py`）
- 盤面の位置（`board`）と星座の効果（`effect`）はゲームのために加えたもの

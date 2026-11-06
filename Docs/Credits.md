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

## 音楽

`StardustFever/Assets/Audio/Bgm/` の曲は、次の音源から `Tools/BuildMusic.py` で切り出した。
元の音源（`Tools/MusicData/`）はリポジトリに入れていない。`python Tools/BuildMusic.py --download` で取ってくる。

| ファイル | 曲 | 演奏 | 出典 | 権利の表示 |
|---|---|---|---|---|
| `Normal.mp3` | ホルスト「惑星」より「金星」 | U.S. Air Force Heritage of America Band（1998） | https://commons.wikimedia.org/wiki/File:Holst-_venus.ogg | 演奏: PD-USGov、作曲: PD |
| `Reach.mp3` | ホルスト「惑星」より「火星」 | U.S. Air Force Band（1998） | https://commons.wikimedia.org/wiki/File:Holst-_mars.ogg | 演奏: PD-USGov、作曲: PD |
| `Fever.mp3` | ホルスト「惑星」より「木星」 | U.S. Air Force Heritage of America Band（1998） | https://commons.wikimedia.org/wiki/File:Holst_The_Planets_Jupiter.ogg | 演奏: PD-USGov、作曲: PD |
| `Map.mp3` | ドビュッシー「月の光」 | U.S. Air Force Band of Flight, Wright Brass（1993） | https://commons.wikimedia.org/wiki/File:Clair_de_Lune_-_Wright_Brass_-_United_States_Air_Force_Band_of_Flight.mp3 | 演奏・録音: PD-USGov、作曲: PD |
| `Chance.mp3` | モーツァルト「きらきら星変奏曲」K.265 | MIDI を `Tools/BuildMusic.py` がオルゴールの音で鳴らした | https://commons.wikimedia.org/wiki/File:K265_(Ah_vous_dirai-je,_Maman).mid（Michael Bednarek 作の MIDI） | MIDI: PD-self、作曲: PD |

- **配布の前に確かめること:** 「惑星」の3曲は Merlin Patterson による吹奏楽への編曲で演奏されている。Commons の表示は演奏と作曲の権利だけで、編曲の権利には触れていない。販売や公開の前に、編曲の扱いを確かめるか、別の録音に差し替える
- 米軍の録音の「PD-USGov」は米国の連邦政府の著作物としての扱いで、他の国でも同じとは限らない

## 効果音

`StardustFever/Assets/Audio/Sfx/` は `Tools/GenerateSfx.py` で合成したもの。

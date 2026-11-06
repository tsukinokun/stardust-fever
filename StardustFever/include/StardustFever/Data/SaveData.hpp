//-------------------------------------------------------------
//! @file   SaveData.hpp
//! @brief  セーブファイル（Saves/save.json）の中身
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>
#include <StardustFever/Core/BigNumberSerialization.hpp>
#include <StardustFever/Data/StarData.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct SaveData
    //! @brief  1つのセーブに残す進行
    //! @note   ゲーム中はコンポーネントが正で、これは保存・読み込みの時だけ使う入れ物。
    //!         項目は全て省略可能にしてあり、項目を足した後でも古いセーブがそのまま読める
    //-------------------------------------------------------------
    struct SaveData {
        int                                  version = 1;                //!< セーブの形式の版
        Core::BigNumber                      stardust;                   //!< 所持している星屑
        std::unordered_map<std::string, int> upgradeLevels;              //!< 強化ID → レベル
        std::unordered_map<std::string, int> starLevels;                 //!< 星ID → レベル
        Core::BigNumber                      cosmicConstants;            //!< 宇宙定数
        int                                  bigBangCount = 0;           //!< ビッグバンの回数
        Core::BigNumber                      universeEarned;             //!< この宇宙で稼いだ星屑の累計
        bool                                 autoFireEnabled = false;    //!< 自動発射を ON にしていたか
        float                                bgmVolume       = 0.8f;     //!< BGM の音量
        float                                sfxVolume       = 0.6f;     //!< 効果音の音量
        bool                                 audioMuted      = false;    //!< 消音しているか
        bool                                 showKeyHints    = true;     //!< キー操作の表示を出すか
        std::vector<std::string>             unseenStars;                //!< 星図でまだ見ていない星
    };

    //! 今の形式の版。項目の意味を変えたときに上げ、読み込み側で古い版を変換する
    //-------------------------------------------------------------
    //! @brief  セーブの形式の版
    //! @note   2: 星の ID を "hip_" + ヒッパルコス番号にした。星座の完成回数は持たない（星がそろったかで決める）
    //-------------------------------------------------------------
    constexpr int kSaveDataVersion = 2;

    //-------------------------------------------------------------
    //! @brief  古い版のセーブを今の形式へ直す
    //! @param  data  [in,out] 読み込んだセーブ
    //! @param  stars [in] 古い星の ID の対応表を持つ星のデータ
    //! @note   古い ID を今の ID へ置き換える。同じ星が2つの ID で入っていたら、高い方のレベルを残す
    //-------------------------------------------------------------
    inline void MigrateSaveData(SaveData& data, const StarTable& stars) {
        if(data.version < 2) {
            std::unordered_map<std::string, int> migrated;
            for(const auto& [id, level] : data.starLevels) {
                int& slot = migrated[stars.ResolveAlias(id)];
                slot      = std::max(slot, level);
            }
            data.starLevels = std::move(migrated);
        }
        data.version = kSaveDataVersion;
    }

    template <class Archive>
    void serialize(Archive& archive, SaveData& data) {
        Tsukino::IO::LoadOptional(archive, "version", data.version);
        Tsukino::IO::LoadOptional(archive, "stardust", data.stardust);
        Tsukino::IO::LoadOptional(archive, "upgradeLevels", data.upgradeLevels);
        Tsukino::IO::LoadOptional(archive, "starLevels", data.starLevels);
        Tsukino::IO::LoadOptional(archive, "cosmicConstants", data.cosmicConstants);
        Tsukino::IO::LoadOptional(archive, "bigBangCount", data.bigBangCount);
        Tsukino::IO::LoadOptional(archive, "universeEarned", data.universeEarned);
        Tsukino::IO::LoadOptional(archive, "autoFireEnabled", data.autoFireEnabled);
        Tsukino::IO::LoadOptional(archive, "bgmVolume", data.bgmVolume);
        Tsukino::IO::LoadOptional(archive, "sfxVolume", data.sfxVolume);
        Tsukino::IO::LoadOptional(archive, "audioMuted", data.audioMuted);
        Tsukino::IO::LoadOptional(archive, "showKeyHints", data.showKeyHints);
        Tsukino::IO::LoadOptional(archive, "unseenStars", data.unseenStars);
    }

}    // namespace StardustFever::Data

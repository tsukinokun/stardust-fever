//-------------------------------------------------------------
//! @file   SaveData.hpp
//! @brief  セーブファイル（Saves/save.json）の中身
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>
#include <StardustFever/Core/BigNumberSerialization.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>

#include <string>
#include <unordered_map>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct SaveData
    //! @brief  1つのセーブに残す進行
    //! @note   ゲーム中はコンポーネントが正で、これは保存・読み込みの時だけ使う入れ物。
    //!         項目は全て省略可能にしてあり、項目を足した後でも古いセーブがそのまま読める
    //-------------------------------------------------------------
    struct SaveData {
        int                                  version = 1;                 //!< セーブの形式の版
        Core::BigNumber                      stardust;                    //!< 所持している星屑
        std::unordered_map<std::string, int> upgradeLevels;               //!< 強化ID → レベル
        std::unordered_map<std::string, int> starLevels;                  //!< 星ID → レベル
        std::unordered_map<std::string, int> constellationCompletions;    //!< 星座ID → 完成回数
        Core::BigNumber                      cosmicConstants;             //!< 宇宙定数
        int                                  bigBangCount = 0;            //!< ビッグバンの回数
        Core::BigNumber                      universeEarned;              //!< この宇宙で稼いだ星屑の累計
        bool                                 autoFireEnabled = false;     //!< 自動発射を ON にしていたか
    };

    //! 今の形式の版。項目の意味を変えたときに上げ、読み込み側で古い版を変換する
    constexpr int kSaveDataVersion = 1;

    template <class Archive>
    void serialize(Archive& archive, SaveData& data) {
        Tsukino::IO::LoadOptional(archive, "version", data.version);
        Tsukino::IO::LoadOptional(archive, "stardust", data.stardust);
        Tsukino::IO::LoadOptional(archive, "upgradeLevels", data.upgradeLevels);
        Tsukino::IO::LoadOptional(archive, "starLevels", data.starLevels);
        Tsukino::IO::LoadOptional(archive, "constellationCompletions", data.constellationCompletions);
        Tsukino::IO::LoadOptional(archive, "cosmicConstants", data.cosmicConstants);
        Tsukino::IO::LoadOptional(archive, "bigBangCount", data.bigBangCount);
        Tsukino::IO::LoadOptional(archive, "universeEarned", data.universeEarned);
        Tsukino::IO::LoadOptional(archive, "autoFireEnabled", data.autoFireEnabled);
    }

}    // namespace StardustFever::Data

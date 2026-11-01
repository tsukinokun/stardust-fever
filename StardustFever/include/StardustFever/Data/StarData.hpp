//-------------------------------------------------------------
//! @file   StarData.hpp
//! @brief  星のマスターデータ（Assets/Data/Stars.json）
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct StarDef
    //! @brief  星1つの定義
    //-------------------------------------------------------------
    struct StarDef {
        std::string id;                      //!< 参照用のID（例: "rigel"）
        std::string name;                    //!< 表示名（UTF-8。例: "リゲル"）
        double      magnitude    = 2.0;      //!< 見かけの等級。小さいほど明るく珍しい
        std::string spectralType = "A";      //!< スペクトル型（O/B/A/F/G/K/M）
        double      basePayout   = 100.0;    //!< つないだときの基本の星屑
        double      levelBonus   = 0.1;      //!< レベル1つごとに全体倍率へ足す量
    };

    //-------------------------------------------------------------
    //! @struct StarTable
    //! @brief  全ての星
    //-------------------------------------------------------------
    struct StarTable {
        std::vector<StarDef> stars;

        //-------------------------------------------------------------
        //! @brief  IDで星を探す
        //! @param  id [in] 星のID
        //! @return 見つかった星（無ければ nullptr）
        //-------------------------------------------------------------
        [[nodiscard]] const StarDef* Find(const std::string& id) const {
            for(const StarDef& star : stars) {
                if(star.id == id)
                    return &star;
            }
            return nullptr;
        }
    };

    template <class Archive>
    void serialize(Archive& archive, StarDef& star) {
        archive(cereal::make_nvp("id", star.id), cereal::make_nvp("name", star.name));
        Tsukino::IO::LoadOptional(archive, "magnitude", star.magnitude);
        Tsukino::IO::LoadOptional(archive, "spectralType", star.spectralType);
        Tsukino::IO::LoadOptional(archive, "basePayout", star.basePayout);
        Tsukino::IO::LoadOptional(archive, "levelBonus", star.levelBonus);
    }

    template <class Archive>
    void serialize(Archive& archive, StarTable& table) {
        archive(cereal::make_nvp("stars", table.stars));
    }

}    // namespace StardustFever::Data

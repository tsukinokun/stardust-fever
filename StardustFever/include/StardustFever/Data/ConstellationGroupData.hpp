//-------------------------------------------------------------
//! @file   ConstellationGroupData.hpp
//! @brief  共通点のある星座のグループのマスターデータ（Assets/Data/ConstellationGroups.json）
//! @note   Tools/BuildSkyCatalog.py が Tools/SkyNames.py の表から作る。手で書き換えない
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Data/ConstellationData.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct ConstellationGroupDef
    //! @brief  星座のグループ1つ（季節の星座・黄道12星座・旧アルゴ座など）
    //! @note   属する星座の星を全部そろえると effect が発動する
    //-------------------------------------------------------------
    struct ConstellationGroupDef {
        std::string              id;                  //!< 参照用のID（例: "winter"）
        std::string              name;                //!< 表示名（UTF-8）
        std::string              description;         //!< 共通点の説明（UTF-8）
        std::vector<std::string> constellationIds;    //!< 属する星座（ConstellationDef::id）
        ConstellationEffectDef   effect;              //!< そろえたときの効果（種類は星座と同じ）
    };

    //-------------------------------------------------------------
    //! @struct ConstellationGroupTable
    //! @brief  全てのグループ
    //-------------------------------------------------------------
    struct ConstellationGroupTable {
        std::vector<ConstellationGroupDef> groups;
    };

    template <class Archive>
    void serialize(Archive& archive, ConstellationGroupDef& group) {
        archive(cereal::make_nvp("id", group.id), cereal::make_nvp("name", group.name), cereal::make_nvp("constellationIds", group.constellationIds));
        Tsukino::IO::LoadOptional(archive, "description", group.description);
        Tsukino::IO::LoadOptional(archive, "effect", group.effect);
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationGroupTable& table) {
        archive(cereal::make_nvp("groups", table.groups));
    }

}    // namespace StardustFever::Data

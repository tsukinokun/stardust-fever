//-------------------------------------------------------------
//! @file   StarLinkConfigComponentSerialization.hpp
//! @brief  StarLinkConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/StarLinkConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const StarLinkConfigComponent& component) {
        archive(cereal::make_nvp("strokeBase", component.strokeBase));
        archive(cereal::make_nvp("perfectMultiplier", component.perfectMultiplier));
        archive(cereal::make_nvp("maxStarLevel", component.maxStarLevel));
        archive(cereal::make_nvp("linkLinePrefab", component.linkLinePrefab));
        archive(cereal::make_nvp("litStarModel", component.litStarModel));
        archive(cereal::make_nvp("lineThickness", component.lineThickness));
    }

    template <class Archive>
    void load(Archive& archive, StarLinkConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "strokeBase", component.strokeBase);
        Tsukino::IO::LoadOptional(archive, "perfectMultiplier", component.perfectMultiplier);
        Tsukino::IO::LoadOptional(archive, "maxStarLevel", component.maxStarLevel);
        Tsukino::IO::LoadOptional(archive, "linkLinePrefab", component.linkLinePrefab);
        Tsukino::IO::LoadOptional(archive, "litStarModel", component.litStarModel);
        Tsukino::IO::LoadOptional(archive, "lineThickness", component.lineThickness);
    }

}    // namespace StardustFever::ECS

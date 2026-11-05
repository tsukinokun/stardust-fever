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
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const StarLinkConfigComponent& component) {
        archive(cereal::make_nvp("linkLinePrefab", component.linkLinePrefab));
        archive(cereal::make_nvp("litStarModel", component.litStarModel));
        archive(cereal::make_nvp("lineThickness", component.lineThickness));
        archive(cereal::make_nvp("guideThickness", component.guideThickness));
        archive(cereal::make_nvp("litEmissive", component.litEmissive));
        archive(cereal::make_nvp("guideEmissive", component.guideEmissive));
    }

    template <class Archive>
    void load(Archive& archive, StarLinkConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "linkLinePrefab", component.linkLinePrefab);
        Tsukino::IO::LoadOptional(archive, "litStarModel", component.litStarModel);
        Tsukino::IO::LoadOptional(archive, "lineThickness", component.lineThickness);
        Tsukino::IO::LoadOptional(archive, "guideThickness", component.guideThickness);
        Tsukino::IO::LoadOptional(archive, "litEmissive", component.litEmissive);
        Tsukino::IO::LoadOptional(archive, "guideEmissive", component.guideEmissive);
    }

}    // namespace StardustFever::ECS

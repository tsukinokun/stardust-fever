//-------------------------------------------------------------
//! @file   PhysicsConfigComponentSerialization.hpp
//! @brief  PhysicsConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/PhysicsConfigComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const PhysicsConfigComponent& component) {
        archive(cereal::make_nvp("unitsPerMeter", component.unitsPerMeter));
        archive(cereal::make_nvp("substeps", component.substeps));
        archive(cereal::make_nvp("penetrationSlop", component.penetrationSlop));
        archive(cereal::make_nvp("speculativeContactDistance", component.speculativeContactDistance));
    }

    template <class Archive>
    void load(Archive& archive, PhysicsConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "unitsPerMeter", component.unitsPerMeter);
        Tsukino::IO::LoadOptional(archive, "substeps", component.substeps);
        Tsukino::IO::LoadOptional(archive, "penetrationSlop", component.penetrationSlop);
        Tsukino::IO::LoadOptional(archive, "speculativeContactDistance", component.speculativeContactDistance);
    }

}    // namespace StardustFever::ECS

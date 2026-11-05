//-------------------------------------------------------------
//! @file   HudPartComponentSerialization.hpp
//! @brief  HudPartComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudPartComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudPartComponent& component) {
        archive(cereal::make_nvp("name", component.name));
    }

    template <class Archive>
    void load(Archive& archive, HudPartComponent& component) {
        Tsukino::IO::LoadOptional(archive, "name", component.name);
    }

}    // namespace StardustFever::ECS

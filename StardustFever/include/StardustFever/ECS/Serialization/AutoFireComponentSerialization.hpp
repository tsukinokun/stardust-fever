//-------------------------------------------------------------
//! @file   AutoFireComponentSerialization.hpp
//! @brief  AutoFireComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/AutoFireComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const AutoFireComponent& component) {
        archive(cereal::make_nvp("unlocked", component.unlocked));
        archive(cereal::make_nvp("enabled", component.enabled));
    }

    template <class Archive>
    void load(Archive& archive, AutoFireComponent& component) {
        Tsukino::IO::LoadOptional(archive, "unlocked", component.unlocked);
        Tsukino::IO::LoadOptional(archive, "enabled", component.enabled);
    }

}    // namespace StardustFever::ECS

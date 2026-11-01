//-------------------------------------------------------------
//! @file   PocketComponentSerialization.hpp
//! @brief  PocketComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/PocketComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const PocketComponent& component) {
        archive(cereal::make_nvp("basePayout", component.basePayout));
        archive(cereal::make_nvp("isStartPocket", component.isStartPocket));
    }

    template <class Archive>
    void load(Archive& archive, PocketComponent& component) {
        Tsukino::IO::LoadOptional(archive, "basePayout", component.basePayout);
        Tsukino::IO::LoadOptional(archive, "isStartPocket", component.isStartPocket);
    }

}    // namespace StardustFever::ECS

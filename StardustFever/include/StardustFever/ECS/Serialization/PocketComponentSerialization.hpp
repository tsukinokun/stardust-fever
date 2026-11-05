//-------------------------------------------------------------
//! @file   PocketComponentSerialization.hpp
//! @brief  PocketComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         古い Prefab の isStartPocket: true は role "start" として読む
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/PocketComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const PocketComponent& component) {
        archive(cereal::make_nvp("basePayout", component.basePayout));
        archive(cereal::make_nvp("role", component.role));
    }

    template <class Archive>
    void load(Archive& archive, PocketComponent& component) {
        Tsukino::IO::LoadOptional(archive, "basePayout", component.basePayout);
        bool isStartPocket = false;
        Tsukino::IO::LoadOptional(archive, "isStartPocket", isStartPocket);
        if(isStartPocket)
            component.role = "start";
        Tsukino::IO::LoadOptional(archive, "role", component.role);
    }

}    // namespace StardustFever::ECS

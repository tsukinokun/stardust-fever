//-------------------------------------------------------------
//! @file   UpgradeShopConfigComponentSerialization.hpp
//! @brief  UpgradeShopConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/UpgradeShopConfigComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const UpgradeShopConfigComponent& component) {
        archive(cereal::make_nvp("repeatDelay", component.repeatDelay));
        archive(cereal::make_nvp("repeatInterval", component.repeatInterval));
        archive(cereal::make_nvp("repeatMinInterval", component.repeatMinInterval));
        archive(cereal::make_nvp("repeatAcceleration", component.repeatAcceleration));
    }

    template <class Archive>
    void load(Archive& archive, UpgradeShopConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "repeatDelay", component.repeatDelay);
        Tsukino::IO::LoadOptional(archive, "repeatInterval", component.repeatInterval);
        Tsukino::IO::LoadOptional(archive, "repeatMinInterval", component.repeatMinInterval);
        Tsukino::IO::LoadOptional(archive, "repeatAcceleration", component.repeatAcceleration);
    }

}    // namespace StardustFever::ECS

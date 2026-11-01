//-------------------------------------------------------------
//! @file   HudConfigComponentSerialization.hpp
//! @brief  HudConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudConfigComponent& component) {
        archive(cereal::make_nvp("messageDuration", component.messageDuration));
        archive(cereal::make_nvp("handleGaugeSegments", component.handleGaugeSegments));
        archive(cereal::make_nvp("helpText", component.helpText));
    }

    template <class Archive>
    void load(Archive& archive, HudConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "messageDuration", component.messageDuration);
        Tsukino::IO::LoadOptional(archive, "handleGaugeSegments", component.handleGaugeSegments);
        Tsukino::IO::LoadOptional(archive, "helpText", component.helpText);
    }

}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   HudGaugeComponentSerialization.hpp
//! @brief  HudGaugeComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudGaugeComponent.hpp>

#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudGaugeComponent& component) {
        archive(cereal::make_nvp("source", component.source));
        archive(cereal::make_nvp("colorLow", component.colorLow));
        archive(cereal::make_nvp("colorMid", component.colorMid));
        archive(cereal::make_nvp("colorHigh", component.colorHigh));
        archive(cereal::make_nvp("smoothing", component.smoothing));
        archive(cereal::make_nvp("pulseAmount", component.pulseAmount));
    }

    template <class Archive>
    void load(Archive& archive, HudGaugeComponent& component) {
        Tsukino::IO::LoadOptional(archive, "source", component.source);
        Tsukino::IO::LoadOptional(archive, "colorLow", component.colorLow);
        Tsukino::IO::LoadOptional(archive, "colorMid", component.colorMid);
        Tsukino::IO::LoadOptional(archive, "colorHigh", component.colorHigh);
        Tsukino::IO::LoadOptional(archive, "smoothing", component.smoothing);
        Tsukino::IO::LoadOptional(archive, "pulseAmount", component.pulseAmount);
    }

}    // namespace StardustFever::ECS

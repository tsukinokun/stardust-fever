//-------------------------------------------------------------
//! @file   TwinkleComponentSerialization.hpp
//! @brief  TwinkleComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。実行時の値は書かない
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/TwinkleComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const TwinkleComponent& component) {
        archive(cereal::make_nvp("baseEmissive", component.baseEmissive));
        archive(cereal::make_nvp("peakEmissive", component.peakEmissive));
        archive(cereal::make_nvp("flashEmissive", component.flashEmissive));
        archive(cereal::make_nvp("intervalMin", component.intervalMin));
        archive(cereal::make_nvp("intervalMax", component.intervalMax));
        archive(cereal::make_nvp("twinkleSeconds", component.twinkleSeconds));
        archive(cereal::make_nvp("flashDecay", component.flashDecay));
    }

    template <class Archive>
    void load(Archive& archive, TwinkleComponent& component) {
        Tsukino::IO::LoadOptional(archive, "baseEmissive", component.baseEmissive);
        Tsukino::IO::LoadOptional(archive, "peakEmissive", component.peakEmissive);
        Tsukino::IO::LoadOptional(archive, "flashEmissive", component.flashEmissive);
        Tsukino::IO::LoadOptional(archive, "intervalMin", component.intervalMin);
        Tsukino::IO::LoadOptional(archive, "intervalMax", component.intervalMax);
        Tsukino::IO::LoadOptional(archive, "twinkleSeconds", component.twinkleSeconds);
        Tsukino::IO::LoadOptional(archive, "flashDecay", component.flashDecay);
    }

}    // namespace StardustFever::ECS

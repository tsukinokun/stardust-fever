//-------------------------------------------------------------
//! @file   GlowHaloComponentSerialization.hpp
//! @brief  GlowHaloComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。実行時の値は書かない
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/GlowHaloComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const GlowHaloComponent& component) {
        archive(cereal::make_nvp("texture", component.texture));
        archive(cereal::make_nvp("texturePixels", component.texturePixels));
        archive(cereal::make_nvp("size", component.size));
        archive(cereal::make_nvp("color", component.color));
        archive(cereal::make_nvp("linkedColor", component.linkedColor));
        archive(cereal::make_nvp("offset", component.offset));
        archive(cereal::make_nvp("pulseAmplitude", component.pulseAmplitude));
        archive(cereal::make_nvp("periodMin", component.periodMin));
        archive(cereal::make_nvp("periodMax", component.periodMax));
        archive(cereal::make_nvp("lightColor", component.lightColor));
        archive(cereal::make_nvp("lightIntensity", component.lightIntensity));
        archive(cereal::make_nvp("linkedLightIntensity", component.linkedLightIntensity));
        archive(cereal::make_nvp("lightRange", component.lightRange));
    }

    template <class Archive>
    void load(Archive& archive, GlowHaloComponent& component) {
        Tsukino::IO::LoadOptional(archive, "texture", component.texture);
        Tsukino::IO::LoadOptional(archive, "texturePixels", component.texturePixels);
        Tsukino::IO::LoadOptional(archive, "size", component.size);
        Tsukino::IO::LoadOptional(archive, "color", component.color);
        Tsukino::IO::LoadOptional(archive, "linkedColor", component.linkedColor);
        Tsukino::IO::LoadOptional(archive, "offset", component.offset);
        Tsukino::IO::LoadOptional(archive, "pulseAmplitude", component.pulseAmplitude);
        Tsukino::IO::LoadOptional(archive, "periodMin", component.periodMin);
        Tsukino::IO::LoadOptional(archive, "periodMax", component.periodMax);
        Tsukino::IO::LoadOptional(archive, "lightColor", component.lightColor);
        Tsukino::IO::LoadOptional(archive, "lightIntensity", component.lightIntensity);
        Tsukino::IO::LoadOptional(archive, "linkedLightIntensity", component.linkedLightIntensity);
        Tsukino::IO::LoadOptional(archive, "lightRange", component.lightRange);
    }

}    // namespace StardustFever::ECS

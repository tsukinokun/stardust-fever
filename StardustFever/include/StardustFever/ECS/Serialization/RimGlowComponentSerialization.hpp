//-------------------------------------------------------------
//! @file   RimGlowComponentSerialization.hpp
//! @brief  エンジンの RimGlowComponent の cereal シリアライズ定義（ゲーム側で用意する）
//! @note   エンジンは RimGlowComponent を Prefab に登録していないので、玉・壁・入賞口の輪郭の光を
//!         JSON で調整できるようにここで定義する。全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）
//-------------------------------------------------------------
#pragma once
#include <Tsukino/BuiltIn/ECS/Component/RimGlowComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : Tsukino::BuiltIn::ECS（cereal が引数の型の名前空間から save / load を探すため）
namespace Tsukino::BuiltIn::ECS {

    template <class Archive>
    void save(Archive& archive, const RimGlowComponent& component) {
        archive(cereal::make_nvp("active", component.active));
        archive(cereal::make_nvp("rimColor", component.rimColor));
        archive(cereal::make_nvp("rimIntensity", component.rimIntensity));
        archive(cereal::make_nvp("rimPower", component.rimPower));
        archive(cereal::make_nvp("glow", component.glow));
    }

    template <class Archive>
    void load(Archive& archive, RimGlowComponent& component) {
        Tsukino::IO::LoadOptional(archive, "active", component.active);
        Tsukino::IO::LoadOptional(archive, "rimColor", component.rimColor);
        Tsukino::IO::LoadOptional(archive, "rimIntensity", component.rimIntensity);
        Tsukino::IO::LoadOptional(archive, "rimPower", component.rimPower);
        Tsukino::IO::LoadOptional(archive, "glow", component.glow);
    }

}    // namespace Tsukino::BuiltIn::ECS

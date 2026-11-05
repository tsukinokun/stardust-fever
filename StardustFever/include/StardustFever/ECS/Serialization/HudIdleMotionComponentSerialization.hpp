//-------------------------------------------------------------
//! @file   HudIdleMotionComponentSerialization.hpp
//! @brief  HudIdleMotionComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudIdleMotionComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudIdleMotionComponent& component) {
        archive(cereal::make_nvp("floatAmplitude", component.floatAmplitude));
        archive(cereal::make_nvp("scaleAmplitude", component.scaleAmplitude));
        archive(cereal::make_nvp("alphaAmplitude", component.alphaAmplitude));
        archive(cereal::make_nvp("periodMin", component.periodMin));
        archive(cereal::make_nvp("periodMax", component.periodMax));
    }

    template <class Archive>
    void load(Archive& archive, HudIdleMotionComponent& component) {
        Tsukino::IO::LoadOptional(archive, "floatAmplitude", component.floatAmplitude);
        Tsukino::IO::LoadOptional(archive, "scaleAmplitude", component.scaleAmplitude);
        Tsukino::IO::LoadOptional(archive, "alphaAmplitude", component.alphaAmplitude);
        Tsukino::IO::LoadOptional(archive, "periodMin", component.periodMin);
        Tsukino::IO::LoadOptional(archive, "periodMax", component.periodMax);
    }

}    // namespace StardustFever::ECS

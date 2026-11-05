//-------------------------------------------------------------
//! @file   HudLayoutConfigComponentSerialization.hpp
//! @brief  HudLayoutConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>

#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudLayoutConfigComponent& component) {
        archive(cereal::make_nvp("shopCardGroup", component.shopCardGroup));
        archive(cereal::make_nvp("shopX", component.shopX));
        archive(cereal::make_nvp("shopY", component.shopY));
        archive(cereal::make_nvp("shopSpacing", component.shopSpacing));
        archive(cereal::make_nvp("shopStagger", component.shopStagger));
        archive(cereal::make_nvp("chartHeaderGroup", component.chartHeaderGroup));
        archive(cereal::make_nvp("chartRowGroup", component.chartRowGroup));
        archive(cereal::make_nvp("chartX", component.chartX));
        archive(cereal::make_nvp("chartY", component.chartY));
        archive(cereal::make_nvp("chartRowSpacing", component.chartRowSpacing));
        archive(cereal::make_nvp("chartHeaderSpacing", component.chartHeaderSpacing));
        archive(cereal::make_nvp("chartIndent", component.chartIndent));
        archive(cereal::make_nvp("colorO", component.colorO));
        archive(cereal::make_nvp("colorB", component.colorB));
        archive(cereal::make_nvp("colorA", component.colorA));
        archive(cereal::make_nvp("colorF", component.colorF));
        archive(cereal::make_nvp("colorG", component.colorG));
        archive(cereal::make_nvp("colorK", component.colorK));
        archive(cereal::make_nvp("colorM", component.colorM));
        archive(cereal::make_nvp("undiscoveredColor", component.undiscoveredColor));
        archive(cereal::make_nvp("affordableColor", component.affordableColor));
        archive(cereal::make_nvp("unaffordableColor", component.unaffordableColor));
        archive(cereal::make_nvp("maxedColor", component.maxedColor));
        archive(cereal::make_nvp("unaffordableAlpha", component.unaffordableAlpha));
        archive(cereal::make_nvp("starIconTexture", component.starIconTexture));
        archive(cereal::make_nvp("starOutlineTexture", component.starOutlineTexture));
    }

    template <class Archive>
    void load(Archive& archive, HudLayoutConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "shopCardGroup", component.shopCardGroup);
        Tsukino::IO::LoadOptional(archive, "shopX", component.shopX);
        Tsukino::IO::LoadOptional(archive, "shopY", component.shopY);
        Tsukino::IO::LoadOptional(archive, "shopSpacing", component.shopSpacing);
        Tsukino::IO::LoadOptional(archive, "shopStagger", component.shopStagger);
        Tsukino::IO::LoadOptional(archive, "chartHeaderGroup", component.chartHeaderGroup);
        Tsukino::IO::LoadOptional(archive, "chartRowGroup", component.chartRowGroup);
        Tsukino::IO::LoadOptional(archive, "chartX", component.chartX);
        Tsukino::IO::LoadOptional(archive, "chartY", component.chartY);
        Tsukino::IO::LoadOptional(archive, "chartRowSpacing", component.chartRowSpacing);
        Tsukino::IO::LoadOptional(archive, "chartHeaderSpacing", component.chartHeaderSpacing);
        Tsukino::IO::LoadOptional(archive, "chartIndent", component.chartIndent);
        Tsukino::IO::LoadOptional(archive, "colorO", component.colorO);
        Tsukino::IO::LoadOptional(archive, "colorB", component.colorB);
        Tsukino::IO::LoadOptional(archive, "colorA", component.colorA);
        Tsukino::IO::LoadOptional(archive, "colorF", component.colorF);
        Tsukino::IO::LoadOptional(archive, "colorG", component.colorG);
        Tsukino::IO::LoadOptional(archive, "colorK", component.colorK);
        Tsukino::IO::LoadOptional(archive, "colorM", component.colorM);
        Tsukino::IO::LoadOptional(archive, "undiscoveredColor", component.undiscoveredColor);
        Tsukino::IO::LoadOptional(archive, "affordableColor", component.affordableColor);
        Tsukino::IO::LoadOptional(archive, "unaffordableColor", component.unaffordableColor);
        Tsukino::IO::LoadOptional(archive, "maxedColor", component.maxedColor);
        Tsukino::IO::LoadOptional(archive, "unaffordableAlpha", component.unaffordableAlpha);
        Tsukino::IO::LoadOptional(archive, "starIconTexture", component.starIconTexture);
        Tsukino::IO::LoadOptional(archive, "starOutlineTexture", component.starOutlineTexture);
    }

}    // namespace StardustFever::ECS

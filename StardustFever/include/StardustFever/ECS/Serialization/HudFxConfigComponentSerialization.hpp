//-------------------------------------------------------------
//! @file   HudFxConfigComponentSerialization.hpp
//! @brief  HudFxConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudFxConfigComponent.hpp>

#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudFxConfigComponent& component) {
        archive(cereal::make_nvp("toastGroup", component.toastGroup));
        archive(cereal::make_nvp("headlineGroup", component.headlineGroup));
        archive(cereal::make_nvp("popupGroup", component.popupGroup));
        archive(cereal::make_nvp("toastRight", component.toastRight));
        archive(cereal::make_nvp("toastBottom", component.toastBottom));
        archive(cereal::make_nvp("toastSpacing", component.toastSpacing));
        archive(cereal::make_nvp("toastSeconds", component.toastSeconds));
        archive(cereal::make_nvp("maxToasts", component.maxToasts));
        archive(cereal::make_nvp("headlineSeconds", component.headlineSeconds));
        archive(cereal::make_nvp("popupRise", component.popupRise));
        archive(cereal::make_nvp("popupSeconds", component.popupSeconds));
        archive(cereal::make_nvp("pinPopupChance", component.pinPopupChance));
        archive(cereal::make_nvp("particleTexture", component.particleTexture));
        archive(cereal::make_nvp("starBurstCount", component.starBurstCount));
        archive(cereal::make_nvp("completeBurstCount", component.completeBurstCount));
        archive(cereal::make_nvp("pinColor", component.pinColor));
        archive(cereal::make_nvp("pocketColor", component.pocketColor));
        archive(cereal::make_nvp("starColor", component.starColor));
        archive(cereal::make_nvp("bonusColor", component.bonusColor));
        archive(cereal::make_nvp("toneInfoColor", component.toneInfoColor));
        archive(cereal::make_nvp("tonePositiveColor", component.tonePositiveColor));
        archive(cereal::make_nvp("toneWarningColor", component.toneWarningColor));
        archive(cereal::make_nvp("headlineX", component.headlineX));
        archive(cereal::make_nvp("headlineY", component.headlineY));
        archive(cereal::make_nvp("maxPopups", component.maxPopups));
        archive(cereal::make_nvp("bannerTravel", component.bannerTravel));
        archive(cereal::make_nvp("timeWarningSeconds", component.timeWarningSeconds));
        archive(cereal::make_nvp("completeBurstX", component.completeBurstX));
        archive(cereal::make_nvp("completeBurstY", component.completeBurstY));
    }

    template <class Archive>
    void load(Archive& archive, HudFxConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "toastGroup", component.toastGroup);
        Tsukino::IO::LoadOptional(archive, "headlineGroup", component.headlineGroup);
        Tsukino::IO::LoadOptional(archive, "popupGroup", component.popupGroup);
        Tsukino::IO::LoadOptional(archive, "toastRight", component.toastRight);
        Tsukino::IO::LoadOptional(archive, "toastBottom", component.toastBottom);
        Tsukino::IO::LoadOptional(archive, "toastSpacing", component.toastSpacing);
        Tsukino::IO::LoadOptional(archive, "toastSeconds", component.toastSeconds);
        Tsukino::IO::LoadOptional(archive, "maxToasts", component.maxToasts);
        Tsukino::IO::LoadOptional(archive, "headlineSeconds", component.headlineSeconds);
        Tsukino::IO::LoadOptional(archive, "popupRise", component.popupRise);
        Tsukino::IO::LoadOptional(archive, "popupSeconds", component.popupSeconds);
        Tsukino::IO::LoadOptional(archive, "pinPopupChance", component.pinPopupChance);
        Tsukino::IO::LoadOptional(archive, "particleTexture", component.particleTexture);
        Tsukino::IO::LoadOptional(archive, "starBurstCount", component.starBurstCount);
        Tsukino::IO::LoadOptional(archive, "completeBurstCount", component.completeBurstCount);
        Tsukino::IO::LoadOptional(archive, "pinColor", component.pinColor);
        Tsukino::IO::LoadOptional(archive, "pocketColor", component.pocketColor);
        Tsukino::IO::LoadOptional(archive, "starColor", component.starColor);
        Tsukino::IO::LoadOptional(archive, "bonusColor", component.bonusColor);
        Tsukino::IO::LoadOptional(archive, "toneInfoColor", component.toneInfoColor);
        Tsukino::IO::LoadOptional(archive, "tonePositiveColor", component.tonePositiveColor);
        Tsukino::IO::LoadOptional(archive, "toneWarningColor", component.toneWarningColor);
        Tsukino::IO::LoadOptional(archive, "headlineX", component.headlineX);
        Tsukino::IO::LoadOptional(archive, "headlineY", component.headlineY);
        Tsukino::IO::LoadOptional(archive, "maxPopups", component.maxPopups);
        Tsukino::IO::LoadOptional(archive, "bannerTravel", component.bannerTravel);
        Tsukino::IO::LoadOptional(archive, "timeWarningSeconds", component.timeWarningSeconds);
        Tsukino::IO::LoadOptional(archive, "completeBurstX", component.completeBurstX);
        Tsukino::IO::LoadOptional(archive, "completeBurstY", component.completeBurstY);
    }

}    // namespace StardustFever::ECS

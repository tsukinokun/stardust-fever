//-------------------------------------------------------------
//! @file   SkyObjectConfigComponentSerialization.hpp
//! @brief  SkyObjectConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/SkyObjectConfigComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const SkyObjectConfigComponent& component) {
        archive(cereal::make_nvp("rarityNames", component.rarityNames));
        archive(cereal::make_nvp("baseWeights", component.baseWeights));
        archive(cereal::make_nvp("growths", component.growths));
        archive(cereal::make_nvp("rarityColors", component.rarityColors));
        archive(cereal::make_nvp("revealDelay", component.revealDelay));
        archive(cereal::make_nvp("cardWidth", component.cardWidth));
        archive(cereal::make_nvp("cardHeight", component.cardHeight));
        archive(cereal::make_nvp("captionColumns", component.captionColumns));
        archive(cereal::make_nvp("flashRarity", component.flashRarity));
    }

    template <class Archive>
    void load(Archive& archive, SkyObjectConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "rarityNames", component.rarityNames);
        Tsukino::IO::LoadOptional(archive, "baseWeights", component.baseWeights);
        Tsukino::IO::LoadOptional(archive, "growths", component.growths);
        Tsukino::IO::LoadOptional(archive, "rarityColors", component.rarityColors);
        Tsukino::IO::LoadOptional(archive, "revealDelay", component.revealDelay);
        Tsukino::IO::LoadOptional(archive, "cardWidth", component.cardWidth);
        Tsukino::IO::LoadOptional(archive, "cardHeight", component.cardHeight);
        Tsukino::IO::LoadOptional(archive, "captionColumns", component.captionColumns);
        Tsukino::IO::LoadOptional(archive, "flashRarity", component.flashRarity);
    }

}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   StarChanceConfigComponentSerialization.hpp
//! @brief  StarChanceConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const StarChanceConfigComponent& component) {
        archive(cereal::make_nvp("chanceProbability", component.chanceProbability));
        archive(cereal::make_nvp("duration", component.duration));
        archive(cereal::make_nvp("endDelayAfterComplete", component.endDelayAfterComplete));
        archive(cereal::make_nvp("starPinPrefab", component.starPinPrefab));
        archive(cereal::make_nvp("pinHideRadius", component.pinHideRadius));
        archive(cereal::make_nvp("rarityBase", component.rarityBase));
        archive(cereal::make_nvp("maxStarLevel", component.maxStarLevel));
        archive(cereal::make_nvp("duplicatePayoutScale", component.duplicatePayoutScale));
        archive(cereal::make_nvp("effectLevelBonus", component.effectLevelBonus));
        archive(cereal::make_nvp("allCompleteMultiplier", component.allCompleteMultiplier));
        archive(cereal::make_nvp("recentCount", component.recentCount));
        archive(cereal::make_nvp("missToastInterval", component.missToastInterval));
    }

    template <class Archive>
    void load(Archive& archive, StarChanceConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "chanceProbability", component.chanceProbability);
        Tsukino::IO::LoadOptional(archive, "duration", component.duration);
        Tsukino::IO::LoadOptional(archive, "endDelayAfterComplete", component.endDelayAfterComplete);
        Tsukino::IO::LoadOptional(archive, "starPinPrefab", component.starPinPrefab);
        Tsukino::IO::LoadOptional(archive, "pinHideRadius", component.pinHideRadius);
        Tsukino::IO::LoadOptional(archive, "rarityBase", component.rarityBase);
        Tsukino::IO::LoadOptional(archive, "maxStarLevel", component.maxStarLevel);
        Tsukino::IO::LoadOptional(archive, "duplicatePayoutScale", component.duplicatePayoutScale);
        Tsukino::IO::LoadOptional(archive, "effectLevelBonus", component.effectLevelBonus);
        Tsukino::IO::LoadOptional(archive, "allCompleteMultiplier", component.allCompleteMultiplier);
        Tsukino::IO::LoadOptional(archive, "recentCount", component.recentCount);
        Tsukino::IO::LoadOptional(archive, "missToastInterval", component.missToastInterval);
    }

}    // namespace StardustFever::ECS

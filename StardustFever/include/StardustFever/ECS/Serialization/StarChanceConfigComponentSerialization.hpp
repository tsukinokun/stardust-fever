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
        archive(cereal::make_nvp("rollSpinSeconds", component.rollSpinSeconds));
        archive(cereal::make_nvp("rollHangSeconds", component.rollHangSeconds));
        archive(cereal::make_nvp("rollFakeSeconds", component.rollFakeSeconds));
        archive(cereal::make_nvp("rollSteps", component.rollSteps));
        archive(cereal::make_nvp("rollComebackRate", component.rollComebackRate));
        archive(cereal::make_nvp("rollNearMissRate", component.rollNearMissRate));
        archive(cereal::make_nvp("goldOnWin", component.goldOnWin));
        archive(cereal::make_nvp("goldOnMiss", component.goldOnMiss));
        archive(cereal::make_nvp("rollFigureX", component.rollFigureX));
        archive(cereal::make_nvp("rollFigureY", component.rollFigureY));
        archive(cereal::make_nvp("rollFigureWidth", component.rollFigureWidth));
        archive(cereal::make_nvp("rollFigureHeight", component.rollFigureHeight));
        archive(cereal::make_nvp("rollVeilAlpha", component.rollVeilAlpha));
        archive(cereal::make_nvp("rollStarTexture", component.rollStarTexture));
        archive(cereal::make_nvp("rollNebulaTexture", component.rollNebulaTexture));
        archive(cereal::make_nvp("rollDustTexture", component.rollDustTexture));
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
        Tsukino::IO::LoadOptional(archive, "rollSpinSeconds", component.rollSpinSeconds);
        Tsukino::IO::LoadOptional(archive, "rollHangSeconds", component.rollHangSeconds);
        Tsukino::IO::LoadOptional(archive, "rollFakeSeconds", component.rollFakeSeconds);
        Tsukino::IO::LoadOptional(archive, "rollSteps", component.rollSteps);
        Tsukino::IO::LoadOptional(archive, "rollComebackRate", component.rollComebackRate);
        Tsukino::IO::LoadOptional(archive, "rollNearMissRate", component.rollNearMissRate);
        Tsukino::IO::LoadOptional(archive, "goldOnWin", component.goldOnWin);
        Tsukino::IO::LoadOptional(archive, "goldOnMiss", component.goldOnMiss);
        Tsukino::IO::LoadOptional(archive, "rollFigureX", component.rollFigureX);
        Tsukino::IO::LoadOptional(archive, "rollFigureY", component.rollFigureY);
        Tsukino::IO::LoadOptional(archive, "rollFigureWidth", component.rollFigureWidth);
        Tsukino::IO::LoadOptional(archive, "rollFigureHeight", component.rollFigureHeight);
        Tsukino::IO::LoadOptional(archive, "rollVeilAlpha", component.rollVeilAlpha);
        Tsukino::IO::LoadOptional(archive, "rollStarTexture", component.rollStarTexture);
        Tsukino::IO::LoadOptional(archive, "rollNebulaTexture", component.rollNebulaTexture);
        Tsukino::IO::LoadOptional(archive, "rollDustTexture", component.rollDustTexture);
    }

}    // namespace StardustFever::ECS

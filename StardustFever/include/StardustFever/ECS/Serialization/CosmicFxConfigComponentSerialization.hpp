//-------------------------------------------------------------
//! @file   CosmicFxConfigComponentSerialization.hpp
//! @brief  CosmicFxConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/CosmicFxConfigComponent.hpp>

#include <Tsukino/BuiltIn/ECS/Serialization/BurstParamsSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const CosmicFxConfigComponent& component) {
        archive(cereal::make_nvp("sparkTexture", component.sparkTexture));
        archive(cereal::make_nvp("pinSparkChance", component.pinSparkChance));
        archive(cereal::make_nvp("pinSpark", component.pinSpark));
        archive(cereal::make_nvp("streakTexture", component.streakTexture));
        archive(cereal::make_nvp("streakPixels", component.streakPixels));
        archive(cereal::make_nvp("shootingIntervalMin", component.shootingIntervalMin));
        archive(cereal::make_nvp("shootingIntervalMax", component.shootingIntervalMax));
        archive(cereal::make_nvp("feverIntervalScale", component.feverIntervalScale));
        archive(cereal::make_nvp("shootingSpawnMin", component.shootingSpawnMin));
        archive(cereal::make_nvp("shootingSpawnMax", component.shootingSpawnMax));
        archive(cereal::make_nvp("shootingDirection", component.shootingDirection));
        archive(cereal::make_nvp("shootingSpeedMin", component.shootingSpeedMin));
        archive(cereal::make_nvp("shootingSpeedMax", component.shootingSpeedMax));
        archive(cereal::make_nvp("shootingSizeMin", component.shootingSizeMin));
        archive(cereal::make_nvp("shootingSizeMax", component.shootingSizeMax));
        archive(cereal::make_nvp("shootingLife", component.shootingLife));
        archive(cereal::make_nvp("shootingColor", component.shootingColor));
    }

    template <class Archive>
    void load(Archive& archive, CosmicFxConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "sparkTexture", component.sparkTexture);
        Tsukino::IO::LoadOptional(archive, "pinSparkChance", component.pinSparkChance);
        Tsukino::IO::LoadOptional(archive, "pinSpark", component.pinSpark);
        Tsukino::IO::LoadOptional(archive, "streakTexture", component.streakTexture);
        Tsukino::IO::LoadOptional(archive, "streakPixels", component.streakPixels);
        Tsukino::IO::LoadOptional(archive, "shootingIntervalMin", component.shootingIntervalMin);
        Tsukino::IO::LoadOptional(archive, "shootingIntervalMax", component.shootingIntervalMax);
        Tsukino::IO::LoadOptional(archive, "feverIntervalScale", component.feverIntervalScale);
        Tsukino::IO::LoadOptional(archive, "shootingSpawnMin", component.shootingSpawnMin);
        Tsukino::IO::LoadOptional(archive, "shootingSpawnMax", component.shootingSpawnMax);
        Tsukino::IO::LoadOptional(archive, "shootingDirection", component.shootingDirection);
        Tsukino::IO::LoadOptional(archive, "shootingSpeedMin", component.shootingSpeedMin);
        Tsukino::IO::LoadOptional(archive, "shootingSpeedMax", component.shootingSpeedMax);
        Tsukino::IO::LoadOptional(archive, "shootingSizeMin", component.shootingSizeMin);
        Tsukino::IO::LoadOptional(archive, "shootingSizeMax", component.shootingSizeMax);
        Tsukino::IO::LoadOptional(archive, "shootingLife", component.shootingLife);
        Tsukino::IO::LoadOptional(archive, "shootingColor", component.shootingColor);
    }

}    // namespace StardustFever::ECS

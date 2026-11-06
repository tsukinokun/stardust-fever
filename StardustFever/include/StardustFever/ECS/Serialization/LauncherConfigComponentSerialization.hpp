//-------------------------------------------------------------
//! @file   LauncherConfigComponentSerialization.hpp
//! @brief  LauncherConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const LauncherConfigComponent& component) {
        archive(cereal::make_nvp("minSpeed", component.minSpeed));
        archive(cereal::make_nvp("maxSpeed", component.maxSpeed));
        archive(cereal::make_nvp("fireInterval", component.fireInterval));
        archive(cereal::make_nvp("chargeCycleSeconds", component.chargeCycleSeconds));
        archive(cereal::make_nvp("autoFireJitter", component.autoFireJitter));
        archive(cereal::make_nvp("maxBallsOnBoard", component.maxBallsOnBoard));
        archive(cereal::make_nvp("killPlaneY", component.killPlaneY));
        archive(cereal::make_nvp("stuckSeconds", component.stuckSeconds));
        archive(cereal::make_nvp("stuckDistance", component.stuckDistance));
        archive(cereal::make_nvp("stuckNudge", component.stuckNudge));
        archive(cereal::make_nvp("stuckMaxNudges", component.stuckMaxNudges));
        archive(cereal::make_nvp("ballPrefab", component.ballPrefab));
    }

    template <class Archive>
    void load(Archive& archive, LauncherConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "minSpeed", component.minSpeed);
        Tsukino::IO::LoadOptional(archive, "maxSpeed", component.maxSpeed);
        Tsukino::IO::LoadOptional(archive, "fireInterval", component.fireInterval);
        Tsukino::IO::LoadOptional(archive, "chargeCycleSeconds", component.chargeCycleSeconds);
        Tsukino::IO::LoadOptional(archive, "autoFireJitter", component.autoFireJitter);
        Tsukino::IO::LoadOptional(archive, "maxBallsOnBoard", component.maxBallsOnBoard);
        Tsukino::IO::LoadOptional(archive, "killPlaneY", component.killPlaneY);
        Tsukino::IO::LoadOptional(archive, "stuckSeconds", component.stuckSeconds);
        Tsukino::IO::LoadOptional(archive, "stuckDistance", component.stuckDistance);
        Tsukino::IO::LoadOptional(archive, "stuckNudge", component.stuckNudge);
        Tsukino::IO::LoadOptional(archive, "stuckMaxNudges", component.stuckMaxNudges);
        Tsukino::IO::LoadOptional(archive, "ballPrefab", component.ballPrefab);
    }

}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   FeverConfigComponentSerialization.hpp
//! @brief  FeverConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/FeverConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const FeverConfigComponent& component) {
        archive(cereal::make_nvp("duration", component.duration));
        archive(cereal::make_nvp("endDelayAfterComplete", component.endDelayAfterComplete));
        archive(cereal::make_nvp("starPinPrefab", component.starPinPrefab));
        archive(cereal::make_nvp("minStarToPocketGap", component.minStarToPocketGap));
    }

    template <class Archive>
    void load(Archive& archive, FeverConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "duration", component.duration);
        Tsukino::IO::LoadOptional(archive, "endDelayAfterComplete", component.endDelayAfterComplete);
        Tsukino::IO::LoadOptional(archive, "starPinPrefab", component.starPinPrefab);
        Tsukino::IO::LoadOptional(archive, "minStarToPocketGap", component.minStarToPocketGap);
    }

}    // namespace StardustFever::ECS

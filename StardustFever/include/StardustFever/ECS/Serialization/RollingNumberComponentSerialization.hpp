//-------------------------------------------------------------
//! @file   RollingNumberComponentSerialization.hpp
//! @brief  RollingNumberComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/RollingNumberComponent.hpp>

#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const RollingNumberComponent& component) {
        archive(cereal::make_nvp("catchUpRate", component.catchUpRate));
        archive(cereal::make_nvp("punchRatio", component.punchRatio));
        archive(cereal::make_nvp("punchAmount", component.punchAmount));
        archive(cereal::make_nvp("punchCooldown", component.punchCooldown));
        archive(cereal::make_nvp("spendColor", component.spendColor));
        archive(cereal::make_nvp("spendFlashSeconds", component.spendFlashSeconds));
    }

    template <class Archive>
    void load(Archive& archive, RollingNumberComponent& component) {
        Tsukino::IO::LoadOptional(archive, "catchUpRate", component.catchUpRate);
        Tsukino::IO::LoadOptional(archive, "punchRatio", component.punchRatio);
        Tsukino::IO::LoadOptional(archive, "punchAmount", component.punchAmount);
        Tsukino::IO::LoadOptional(archive, "punchCooldown", component.punchCooldown);
        Tsukino::IO::LoadOptional(archive, "spendColor", component.spendColor);
        Tsukino::IO::LoadOptional(archive, "spendFlashSeconds", component.spendFlashSeconds);
    }

}    // namespace StardustFever::ECS

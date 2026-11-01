//-------------------------------------------------------------
//! @file   PrestigeConfigComponentSerialization.hpp
//! @brief  PrestigeConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const PrestigeConfigComponent& component) {
        archive(cereal::make_nvp("threshold", component.threshold));
        archive(cereal::make_nvp("exponent", component.exponent));
        archive(cereal::make_nvp("bonusPerConstant", component.bonusPerConstant));
        archive(cereal::make_nvp("confirmWindow", component.confirmWindow));
    }

    template <class Archive>
    void load(Archive& archive, PrestigeConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "threshold", component.threshold);
        Tsukino::IO::LoadOptional(archive, "exponent", component.exponent);
        Tsukino::IO::LoadOptional(archive, "bonusPerConstant", component.bonusPerConstant);
        Tsukino::IO::LoadOptional(archive, "confirmWindow", component.confirmWindow);
    }

}    // namespace StardustFever::ECS

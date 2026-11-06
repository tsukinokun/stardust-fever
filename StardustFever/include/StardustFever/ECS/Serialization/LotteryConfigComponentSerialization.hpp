//-------------------------------------------------------------
//! @file   LotteryConfigComponentSerialization.hpp
//! @brief  LotteryConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/LotteryConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const LotteryConfigComponent& component) {
        archive(cereal::make_nvp("winProbability", component.winProbability));
        archive(cereal::make_nvp("maxPending", component.maxPending));
        archive(cereal::make_nvp("spinDuration", component.spinDuration));
        archive(cereal::make_nvp("symbolInterval", component.symbolInterval));
        archive(cereal::make_nvp("reachExtraDuration", component.reachExtraDuration));
        archive(cereal::make_nvp("symbols", component.symbols));
    }

    template <class Archive>
    void load(Archive& archive, LotteryConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "winProbability", component.winProbability);
        Tsukino::IO::LoadOptional(archive, "maxPending", component.maxPending);
        Tsukino::IO::LoadOptional(archive, "spinDuration", component.spinDuration);
        Tsukino::IO::LoadOptional(archive, "symbolInterval", component.symbolInterval);
        Tsukino::IO::LoadOptional(archive, "reachExtraDuration", component.reachExtraDuration);
        Tsukino::IO::LoadOptional(archive, "symbols", component.symbols);
    }

}    // namespace StardustFever::ECS

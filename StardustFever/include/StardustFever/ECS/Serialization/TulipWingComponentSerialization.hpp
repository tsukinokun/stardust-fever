//-------------------------------------------------------------
//! @file   TulipWingComponentSerialization.hpp
//! @brief  TulipWingComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。実行時だけの値は書かない
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/TulipWingComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const TulipWingComponent& component) {
        archive(cereal::make_nvp("halfLength", component.halfLength));
        archive(cereal::make_nvp("closedDeg", component.closedDeg));
        archive(cereal::make_nvp("openDeg", component.openDeg));
        archive(cereal::make_nvp("moveSeconds", component.moveSeconds));
    }

    template <class Archive>
    void load(Archive& archive, TulipWingComponent& component) {
        Tsukino::IO::LoadOptional(archive, "halfLength", component.halfLength);
        Tsukino::IO::LoadOptional(archive, "closedDeg", component.closedDeg);
        Tsukino::IO::LoadOptional(archive, "openDeg", component.openDeg);
        Tsukino::IO::LoadOptional(archive, "moveSeconds", component.moveSeconds);
    }

}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   SparkleTrailComponentSerialization.hpp
//! @brief  SparkleTrailComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。実行時の値は書かない
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/SparkleTrailComponent.hpp>

#include <Tsukino/BuiltIn/ECS/Serialization/BurstParamsSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const SparkleTrailComponent& component) {
        archive(cereal::make_nvp("texture", component.texture));
        archive(cereal::make_nvp("interval", component.interval));
        archive(cereal::make_nvp("minSpeed", component.minSpeed));
        archive(cereal::make_nvp("burst", component.burst));
        archive(cereal::make_nvp("feverColorA", component.feverColorA));
        archive(cereal::make_nvp("feverColorB", component.feverColorB));
    }

    template <class Archive>
    void load(Archive& archive, SparkleTrailComponent& component) {
        Tsukino::IO::LoadOptional(archive, "texture", component.texture);
        Tsukino::IO::LoadOptional(archive, "interval", component.interval);
        Tsukino::IO::LoadOptional(archive, "minSpeed", component.minSpeed);
        Tsukino::IO::LoadOptional(archive, "burst", component.burst);
        Tsukino::IO::LoadOptional(archive, "feverColorA", component.feverColorA);
        Tsukino::IO::LoadOptional(archive, "feverColorB", component.feverColorB);
    }

}    // namespace StardustFever::ECS

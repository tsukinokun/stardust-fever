//-------------------------------------------------------------
//! @file   PocketLabelComponentSerialization.hpp
//! @brief  PocketLabelComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。実行時だけの値は書かない
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/PocketLabelComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const PocketLabelComponent& component) {
        archive(cereal::make_nvp("text", component.text));
        archive(cereal::make_nvp("color", component.color));
        archive(cereal::make_nvp("worldOffsetY", component.worldOffsetY));
        archive(cereal::make_nvp("scale", component.scale));
    }

    template <class Archive>
    void load(Archive& archive, PocketLabelComponent& component) {
        Tsukino::IO::LoadOptional(archive, "text", component.text);
        Tsukino::IO::LoadOptional(archive, "color", component.color);
        Tsukino::IO::LoadOptional(archive, "worldOffsetY", component.worldOffsetY);
        Tsukino::IO::LoadOptional(archive, "scale", component.scale);
    }

}    // namespace StardustFever::ECS

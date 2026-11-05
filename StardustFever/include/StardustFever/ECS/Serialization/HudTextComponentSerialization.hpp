//-------------------------------------------------------------
//! @file   HudTextComponentSerialization.hpp
//! @brief  HudTextComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudTextComponent.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const HudTextComponent& component) {
        archive(cereal::make_nvp("kind", static_cast<int>(component.kind)));
        archive(cereal::make_nvp("index", component.index));
        archive(cereal::make_nvp("text", component.text));
    }

    template <class Archive>
    void load(Archive& archive, HudTextComponent& component) {
        // 一時変数は今の値から始める（InstantiateGroup の2回目の通過で 0 に戻らないように）
        int kind = static_cast<int>(component.kind);
        if(Tsukino::IO::LoadOptional(archive, "kind", kind))
            component.kind = static_cast<HudTextKind>(kind);
        Tsukino::IO::LoadOptional(archive, "index", component.index);
        Tsukino::IO::LoadOptional(archive, "text", component.text);
    }

}    // namespace StardustFever::ECS

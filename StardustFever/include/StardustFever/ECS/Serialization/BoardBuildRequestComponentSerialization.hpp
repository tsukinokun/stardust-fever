//-------------------------------------------------------------
//! @file   BoardBuildRequestComponentSerialization.hpp
//! @brief  BoardBuildRequestComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/BoardBuildRequestComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const BoardBuildRequestComponent& component) {
        archive(cereal::make_nvp("layoutPath", component.layoutPath));
    }

    template <class Archive>
    void load(Archive& archive, BoardBuildRequestComponent& component) {
        Tsukino::IO::LoadOptional(archive, "layoutPath", component.layoutPath);
    }

}    // namespace StardustFever::ECS

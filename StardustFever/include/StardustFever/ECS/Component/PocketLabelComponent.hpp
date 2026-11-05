//-------------------------------------------------------------
//! @file   PocketLabelComponent.hpp
//! @brief  PocketLabelComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <entt/entt.hpp>
#include <hlsl++.h>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PocketLabelComponent
    //! @brief  入賞口の上に出す名札（「スタート」「星の門」など）
    //! @note   入賞口の Prefab に付ける。PocketLabelSystem が画面の文字を作り、入賞口の上へ貼り付ける
    //-------------------------------------------------------------
    struct PocketLabelComponent {
        std::string    text;                                                     //!< 名札の文字（UTF-8）
        hlslpp::float4 color        = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);    //!< 文字の色（入賞口の色に合わせる）
        float          worldOffsetY = 4.0f;                                      //!< 入賞口からどれだけ上に出すか（ワールド単位）
        float          scale        = 0.42f;                                     //!< 文字の大きさ

        // 実行時の値（JSON には書かない）
        entt::entity label = entt::null;    //!< 作った名札
    };
}    // namespace StardustFever::ECS

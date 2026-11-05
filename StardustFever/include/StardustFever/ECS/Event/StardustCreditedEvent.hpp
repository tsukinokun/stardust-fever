//-------------------------------------------------------------
//! @file   StardustCreditedEvent.hpp
//! @brief  StardustCreditedEvent の宣言
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   StardustSource
    //! @brief  星屑の出どころ（浮かぶ数字の色分けに使う）
    //-------------------------------------------------------------
    enum class StardustSource {
        Pin,       //!< 釘
        Pocket,    //!< 入賞口
        Star,      //!< 星つなぎ
        Bonus,     //!< 星座の完成ボーナスなど
    };

    //-------------------------------------------------------------
    //! @struct StardustCreditedEvent
    //! @brief  星屑が財布へ入った（全体倍率を掛けた後の、実際に入った量）
    //! @note   PayoutSystem だけが出す。演出はこれを見て、表示と実際の額がずれないようにする
    //-------------------------------------------------------------
    struct StardustCreditedEvent {
        StardustFever::Core::BigNumber amount;                                            //!< 入った量
        StardustSource                 source      = StardustSource::Bonus;               //!< 出どころ
        hlslpp::float3                 position    = hlslpp::float3(0.0f, 0.0f, 0.0f);    //!< 得た場所（盤面のワールド座標）
        bool                           hasPosition = false;                               //!< position が意味を持つか
    };
}    // namespace StardustFever::ECS

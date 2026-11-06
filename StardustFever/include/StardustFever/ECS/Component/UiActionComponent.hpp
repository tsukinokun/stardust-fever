//-------------------------------------------------------------
//! @file   UiActionComponent.hpp
//! @brief  UiActionComponent の宣言
//-------------------------------------------------------------
#pragma once
#include <entt/entt.hpp>

#include <hlsl++.h>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @enum   UiAction
    //! @brief  押せる部品を押したときの意味
    //-------------------------------------------------------------
    enum class UiAction : int {
        None,
        BuyUpgrade,         //!< 強化を買う（index は強化の並び順）
        ToggleAutoFire,     //!< 自動発射の入切
        OpenMap,            //!< 星図を開く
        OpenGroups,         //!< グループの一覧を開く
        BigBang,            //!< ビッグバンの確認を開く
        BigBangConfirm,     //!< ビッグバンを実行する
        BigBangCancel,      //!< ビッグバンをやめる
        MapPrevious,        //!< 前の星座・グループへ
        MapNext,            //!< 次の星座・グループへ
        MapToggleGroups,    //!< 星座とグループの一覧を切り替える
        MapClose,           //!< 星図を閉じる
        MapSelectGroup,     //!< グループを選ぶ（index はグループの並び順）
    };

    //-------------------------------------------------------------
    //! @struct UiActionComponent
    //! @brief  押せる部品の意味と色
    //! @note   PointerTargetComponent と一緒に付ける。UiButtonSystem が押されたことを UiActionEvent にする
    //-------------------------------------------------------------
    struct UiActionComponent {
        UiAction       action     = UiAction::None;                            //!< 押したときの意味
        int            index      = 0;                                         //!< 意味ごとの番号
        bool           tinted     = false;                                     //!< 色を UiButtonSystem に任せるか
        hlslpp::float4 baseColor  = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);    //!< ふだんの色
        hlslpp::float4 hoverColor = hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);    //!< マウスが乗ったときの色
        entt::entity   label      = entt::null;                                //!< ボタンの文字（無ければ null）
    };
}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   UpgradeShopSystem.hpp
//! @brief  星屑を払って強化を買う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/UiActionEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <string>

namespace Tsukino::ECS {
    class EventBus;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  UpgradeShopSystem
    //! @brief  数字キー 1〜9 とカードで、Upgrades.json の並び順の強化を1レベル買う。長押しで続けて買う
    //! @note   買うのはレベルを上げるところまで。効果は UpgradeEffectSystem が計算する
    //-------------------------------------------------------------
    class UpgradeShopSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 購入イベントとメッセージの発行先
        //-------------------------------------------------------------
        explicit UpgradeShopSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  購入の入力を処理する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  強化を1レベル買う
        //! @param  registry [in] レジストリ
        //! @param  index    [in] Upgrades.json の添字
        //! @param  repeated [in] 長押しの2回目以降か。買えなくても知らせず、通知もまとめる
        //! @return 買えたか
        //-------------------------------------------------------------
        bool Buy(Tsukino::ECS::Registry& registry, int index, bool repeated);

        //-------------------------------------------------------------
        //! @brief  長押しを終える
        //-------------------------------------------------------------
        void EndHold();

        Tsukino::ECS::EventBus*   m_eventBus = nullptr;
        EventQueue<UiActionEvent> m_actions;
        int                       m_holdIndex    = -1;      //!< 長押ししている強化（無ければ -1）
        float                     m_holdTimer    = 0.0f;    //!< 次に買うまでの秒数
        float                     m_holdInterval = 0.0f;    //!< 今の買う間隔（秒）
        int                       m_holdCount    = 0;       //!< 長押しで続けて買った回数
        std::string               m_holdName;               //!< 最後に買った強化の名前
        int                       m_holdLevel = 0;          //!< 最後に買ったレベル
    };
}    // namespace StardustFever::ECS

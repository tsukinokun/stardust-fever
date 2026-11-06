//-------------------------------------------------------------
//! @file   BigBangSystem.hpp
//! @brief  ビッグバン（転生）を行う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/UiActionEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

namespace Tsukino::ECS {
    class EventBus;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  BigBangSystem
    //! @brief  B キーを2回押すと、星屑と強化を捨てて宇宙定数を得る
    //! @note   B かボタンで確認画面を開き、もう一度 B か実行のボタンで行う
    //!         2回目で実行する（押し間違いで進行が消えないようにするため）。
    //!         フィーバー中は星釘が残るのでできない。星のレベルと星座の完成回数は残す
    //-------------------------------------------------------------
    class BigBangSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] ビッグバンのイベントとメッセージの発行先
        //-------------------------------------------------------------
        explicit BigBangSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  入力を見てビッグバンを行う
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  ビッグバンを行う
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Execute(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::EventBus*   m_eventBus = nullptr;
        EventQueue<UiActionEvent> m_actions;
    };
}    // namespace StardustFever::ECS

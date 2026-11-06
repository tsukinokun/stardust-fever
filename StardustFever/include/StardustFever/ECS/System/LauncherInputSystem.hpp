//-------------------------------------------------------------
//! @file   LauncherInputSystem.hpp
//! @brief  ハンドル操作を発射台へ反映する System
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
    //! @class  LauncherInputSystem
    //! @brief  Space 長押しで強さを往復させ、離した瞬間の強さで1球打つ。A で自動発射の切り替え
    //! @note   入力を読むのはここだけにし、発射そのものは LaunchSystem が行う
    //-------------------------------------------------------------
    class LauncherInputSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] メッセージ（HudMessageEvent）の発行先
        //-------------------------------------------------------------
        explicit LauncherInputSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  入力を読んで発射台を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::EventBus*   m_eventBus = nullptr;
        EventQueue<UiActionEvent> m_actions;
        bool                      m_mouseCharging = false;    //!< 盤面の長押しで溜めているか
    };
}    // namespace StardustFever::ECS

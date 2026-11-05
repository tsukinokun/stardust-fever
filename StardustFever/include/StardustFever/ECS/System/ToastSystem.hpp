//-------------------------------------------------------------
//! @file   ToastSystem.hpp
//! @brief  通知（右下のトーストと画面中央の大見出し）を出す System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  ToastSystem
    //! @brief  HudMessageEvent を受けて、よくある出来事は右下へ積むトーストに、
    //!         大きな出来事は画面中央で弾む大見出しにする
    //! @note   トーストは新しいものが下に入り、古いものはなめらかに上へ押し上がる。
    //!         縁の色は HudMessageTone で変え、傾きを少しずつばらつかせる
    //-------------------------------------------------------------
    class ToastSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] メッセージの購読元
        //-------------------------------------------------------------
        explicit ToastSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  通知を出し、動かし、消す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  通知を消し始める（消えるトゥイーンを掛け、少し後に破棄する）
        //-------------------------------------------------------------
        void Leave(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity);

        EventQueue<HudMessageEvent> m_messages;
        std::mt19937                m_random;
    };
}    // namespace StardustFever::ECS

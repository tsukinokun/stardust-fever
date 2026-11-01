//-------------------------------------------------------------
//! @file   HudSystem.hpp
//! @brief  ゲームの状態をHUDの文字へ書き出す System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  HudSystem
    //! @brief  HudTextComponent の種類ごとに、表示する文字を組み立てて FontComponent へ入れる
    //! @note   表示はここだけで組み立てる。他の System は状態を書くか HudMessageEvent を出すだけ
    //-------------------------------------------------------------
    class HudSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] メッセージの購読元
        //-------------------------------------------------------------
        explicit HudSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  HUDの文字を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        EventQueue<HudMessageEvent> m_messages;
    };
}    // namespace StardustFever::ECS

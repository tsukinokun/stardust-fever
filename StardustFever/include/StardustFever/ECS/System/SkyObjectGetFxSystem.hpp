//-------------------------------------------------------------
//! @file   SkyObjectGetFxSystem.hpp
//! @brief  ビッグバンで手に入れた天体を見せる System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/SkyObjectDrawnEvent.hpp>
#include <StardustFever/ECS/Event/UiActionEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>

#include <string>

namespace Tsukino::ECS {
    class EventBus;
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  SkyObjectGetFxSystem
    //! @brief  ビッグバンの光が晴れたころに、手に入れた天体のカードを出す
    //! @note   カードを出している間は PrestigeStateComponent::rewardOpen で遊びを止める
    //-------------------------------------------------------------
    class SkyObjectGetFxSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene    [in] 部品を作るシーン
        //! @param  eventBus [in] 手に入れた知らせの受け取りと、効果音のきっかけの発行先
        //-------------------------------------------------------------
        SkyObjectGetFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  カードを出し・片付ける
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  カードを作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene*            m_scene    = nullptr;
        Tsukino::ECS::EventBus*         m_eventBus = nullptr;
        EventQueue<SkyObjectDrawnEvent> m_draws;
        EventQueue<UiActionEvent>       m_actions;
        std::string                     m_objectId;              //!< 見せる天体（無ければ空）
        float                           m_delay = 0.0f;          //!< カードを出すまでの残り秒数
        entt::entity                    m_root  = entt::null;    //!< カード（出している間だけ）
    };
}    // namespace StardustFever::ECS

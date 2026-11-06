//-------------------------------------------------------------
//! @file   StarGateFxSystem.hpp
//! @brief  星の門の抽選の演出を行う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/StarGateRollEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>

#include <vector>

// 名前空間 : Tsukino::ECS
namespace Tsukino::ECS {
    class Scene;
}    // namespace Tsukino::ECS

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarGateFxSystem
    //! @brief  星の輪を光が回り、だんだん遅くなって当たり外れを見せる
    //! @note   結果は StarChanceSystem が決める。ここは見せるだけ。輪の色が金なら当たりやすい
    //-------------------------------------------------------------
    class StarGateFxSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene    [in] 部品を作るシーン
        //! @param  eventBus [in] 抽選の知らせの購読元であり、刻みの発行先
        //-------------------------------------------------------------
        StarGateFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  演出を進める
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  輪を作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  輪を片付ける
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Clear(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene*          m_scene    = nullptr;
        Tsukino::ECS::EventBus*       m_eventBus = nullptr;
        EventQueue<StarGateRollEvent> m_events;
        entt::entity                  m_root    = entt::null;    //!< 輪
        entt::entity                  m_center  = entt::null;    //!< 真ん中の文字
        entt::entity                  m_caption = entt::null;    //!< 結果の文字
        std::vector<entt::entity>     m_stars;                   //!< 輪の星
        int                           m_litStar    = -1;         //!< 光っている星
        int                           m_result     = 0;          //!< 0: 抽選中、1: 当たり、2: 外れ
        float                         m_resultTime = 0.0f;       //!< 結果を見せている時間（秒）
    };
}    // namespace StardustFever::ECS

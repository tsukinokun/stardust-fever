//-------------------------------------------------------------
//! @file   TwinkleSystem.hpp
//! @brief  釘を星のように瞬かせ、当たると閃かせる System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallHitPinEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

namespace Tsukino::ECS {
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  TwinkleSystem
    //! @brief  TwinkleComponent を持つモデルの自己発光を、時々ふっと強め、玉が当たったら一瞬強く光らせる
    //! @note   瞬く間隔は実体ごとに乱数で引き直すので、盤面の釘はばらばらに瞬く。
    //!         当たった釘には CosmicFxConfigComponent の割合で小さな火花も出す
    //-------------------------------------------------------------
    class TwinkleSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene    [in] 火花の生成先（この System より長生きすること）
        //! @param  eventBus [in] 釘に当たったイベントの購読元
        //-------------------------------------------------------------
        TwinkleSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  自己発光を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::Scene*        m_scene = nullptr;
        EventQueue<BallHitPinEvent> m_hits;
        std::mt19937                m_random;
    };
}    // namespace StardustFever::ECS

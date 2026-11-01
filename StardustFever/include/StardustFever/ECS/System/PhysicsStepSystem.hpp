//-------------------------------------------------------------
//! @file   PhysicsStepSystem.hpp
//! @brief  エンジンの PhysicsSystem を1フレームに複数回進める System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <memory>

namespace Tsukino::ECS {
    class EventBus;
}
namespace Tsukino::BuiltIn::ECS {
    class PhysicsSystem;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  PhysicsStepSystem
    //! @brief  PhysicsConfigComponent に従って物理を分割して進める
    //! @note   パチンコ玉は速く、釘は細い。エンジンの PhysicsSystem は1フレーム1ステップで
    //!         連続衝突判定も無いため、そのままでは玉が釘をすり抜ける。
    //!         エンジンには手を入れず、同じ System を短い時間で複数回呼んで解決する
    //-------------------------------------------------------------
    class PhysicsStepSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 衝突イベント（CollisionEnterEvent）の発行先
        //-------------------------------------------------------------
        explicit PhysicsStepSystem(Tsukino::ECS::EventBus& eventBus);
        ~PhysicsStepSystem() override;

        //-------------------------------------------------------------
        //! @brief  物理を進める
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        std::unique_ptr<Tsukino::BuiltIn::ECS::PhysicsSystem> m_physics;
        bool                                                  m_unitsApplied               = false;    //!< unitsPerMeter を反映したか（ボディ生成前に1度だけ）
        float                                                 m_appliedPenetrationSlop     = -1.0f;    //!< 最後に反映した値（変わったときだけ設定し直す）
        float                                                 m_appliedSpeculativeDistance = -1.0f;    //!< 最後に反映した値
    };
}    // namespace StardustFever::ECS

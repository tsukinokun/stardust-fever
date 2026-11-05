//-------------------------------------------------------------
//! @file   PhysicsStepSystem.cpp
//! @brief  PhysicsStepSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/PhysicsStepSystem.hpp>

#include <StardustFever/ECS/Component/PhysicsConfigComponent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/EngineIntegration/ECS/System/PhysicsSystem.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    PhysicsStepSystem::PhysicsStepSystem(Tsukino::ECS::EventBus& eventBus)
        : m_physics(std::make_unique<Tsukino::BuiltIn::ECS::PhysicsSystem>(eventBus)) {
    }

    PhysicsStepSystem::~PhysicsStepSystem() = default;

    //-------------------------------------------------------------
    // 物理を分割して進める
    //-------------------------------------------------------------
    void PhysicsStepSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        int substeps = 1;
        if(const PhysicsConfigComponent* config = FindSingleton<PhysicsConfigComponent>(registry)) {
            //-------------------------------------------------------------
            // 長さの単位はボディの生成前に決めておく必要があるため、最初の1回だけ反映する。
            // ボディは m_physics->Update の頭で作られるので、ここが間に合う最後の機会
            //-------------------------------------------------------------
            if(!m_unitsApplied) {
                m_physics->SetUnitsPerMeter(config->unitsPerMeter);
                m_unitsApplied = true;
            }
            if(config->penetrationSlop != m_appliedPenetrationSlop || config->speculativeContactDistance != m_appliedSpeculativeDistance) {
                m_physics->SetContactTolerances(config->penetrationSlop, config->speculativeContactDistance);
                m_appliedPenetrationSlop     = config->penetrationSlop;
                m_appliedSpeculativeDistance = config->speculativeContactDistance;
            }
            substeps = std::max(1, config->substeps);
        }

        const float stepTime = deltaTime / static_cast<float>(substeps);
        for(int i = 0; i < substeps; ++i) {
            m_physics->Update(registry, stepTime);
        }
    }

}    // namespace StardustFever::ECS

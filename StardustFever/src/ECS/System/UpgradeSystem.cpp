//-------------------------------------------------------------
//! @file   UpgradeSystem.cpp
//! @brief  UpgradeSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/UpgradeSystem.hpp>

#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeConfigComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>
#include <Tsukino/Core/Input/KeyCodes.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    UpgradeSystem::UpgradeSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // 購入の入力を処理する
    //-------------------------------------------------------------
    void UpgradeSystem::Update(Tsukino::ECS::Registry& registry, float) {
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->inputSystem || !context->inputSystem->IsKeyPressed(Tsukino::Input::KeyCode::U))
            return;

        WalletComponent*              wallet   = FindSingleton<WalletComponent>(registry);
        AutoFireComponent*            autoFire = FindSingleton<AutoFireComponent>(registry);
        const UpgradeConfigComponent* config   = FindSingleton<UpgradeConfigComponent>(registry);
        if(!wallet || !autoFire || !config)
            return;

        if(autoFire->unlocked) {
            m_eventBus->Publish(HudMessageEvent{"自動発射は解禁済みです（A で切り替え）"});
            return;
        }

        const Core::BigNumber cost(config->autoFireUnlockCost);
        if(wallet->stardust < cost) {
            m_eventBus->Publish(HudMessageEvent{"星屑が足りません  あと " + (cost - wallet->stardust).ToJapaneseString()});
            return;
        }

        wallet->stardust   -= cost;
        autoFire->unlocked  = true;
        m_eventBus->Publish(HudMessageEvent{"自動発射を解禁！  A で切り替え（強さが揺らぎます）"});
    }

}    // namespace StardustFever::ECS

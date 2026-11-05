//-------------------------------------------------------------
//! @file   UpgradeShopSystem.cpp
//! @brief  UpgradeShopSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/UpgradeShopSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Event/UpgradePurchasedEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>
#include <Tsukino/Core/Input/KeyCodes.hpp>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        // 購入に使えるキーの数（1〜9）
        constexpr int kShopKeyCount = 9;
    }    // namespace

    UpgradeShopSystem::UpgradeShopSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // 購入の入力を処理する
    //-------------------------------------------------------------
    void UpgradeShopSystem::Update(Tsukino::ECS::Registry& registry, float) {
        // 星図の画面を開いている間は止める
        if(IsPlayPaused(registry))
            return;
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->inputSystem)
            return;

        //-------------------------------------------------------------
        // 押された数字キーを探す。D1〜D9 は仮想キーコードが連番（'1'〜'9'）
        //-------------------------------------------------------------
        using Tsukino::Input::KeyCode;
        int pressedIndex = -1;
        for(int i = 0; i < kShopKeyCount; ++i) {
            if(context->inputSystem->IsKeyPressed(static_cast<KeyCode>(static_cast<int>(KeyCode::D1) + i))) {
                pressedIndex = i;
                break;
            }
        }
        if(pressedIndex < 0)
            return;

        const Data::UpgradeTable& table = Data::GetUpgradeTable();
        if(pressedIndex >= static_cast<int>(table.upgrades.size()))
            return;

        WalletComponent*        wallet = FindSingleton<WalletComponent>(registry);
        UpgradeLevelsComponent* levels = FindSingleton<UpgradeLevelsComponent>(registry);
        if(!wallet || !levels)
            return;

        const Data::UpgradeDef& upgrade = table.upgrades[static_cast<size_t>(pressedIndex)];
        int&                    level   = levels->levels[upgrade.id];
        if(Core::UpgradeFormula::IsMaxed(level, upgrade.maxLevel)) {
            m_eventBus->Publish(HudMessageEvent{upgrade.name + " は最大レベルです", HudMessageImportance::Toast, HudMessageTone::Warning});
            return;
        }

        const Core::BigNumber cost = upgrade.CostAt(level);
        if(wallet->stardust < cost) {
            m_eventBus->Publish(HudMessageEvent{upgrade.name + "  星屑が足りません  あと " + (cost - wallet->stardust).ToJapaneseString(),
                                                HudMessageImportance::Toast,
                                                HudMessageTone::Warning});
            return;
        }

        wallet->stardust -= cost;
        ++level;
        m_eventBus->Publish(UpgradePurchasedEvent{upgrade.id, level});
        m_eventBus->Publish(HudMessageEvent{upgrade.name + " Lv" + std::to_string(level) + " ！", HudMessageImportance::Toast, HudMessageTone::Positive});
    }

}    // namespace StardustFever::ECS

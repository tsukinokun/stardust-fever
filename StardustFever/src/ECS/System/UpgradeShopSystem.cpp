//-------------------------------------------------------------
//! @file   UpgradeShopSystem.cpp
//! @brief  UpgradeShopSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/UpgradeShopSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/UiActionComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeShopConfigComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Event/UpgradeDeniedEvent.hpp>
#include <StardustFever/ECS/Event/UpgradePurchasedEvent.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/Core/ECS/Event/EventBus.hpp>
#include <Tsukino/Core/Input/InputSystem.hpp>
#include <Tsukino/Core/Input/KeyCodes.hpp>

#include <algorithm>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        // 購入に使えるキーの数（1〜9）
        constexpr int kShopKeyCount = 9;
        // 1フレームで続けて買う上限。止まった後のフレームで一度に買いすぎないように
        constexpr int kMaxRepeatsPerFrame = 4;
    }    // namespace

    UpgradeShopSystem::UpgradeShopSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_actions(eventBus) {
    }

    //-------------------------------------------------------------
    // 購入の入力を処理する
    //-------------------------------------------------------------
    void UpgradeShopSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto actions = m_actions.Drain();
        // 星図や設定の画面を開いている間は止める
        if(IsPlayPaused(registry)) {
            EndHold();
            return;
        }
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
        // カードのクリックも数字キーと同じ処理へつなぐ
        for(const UiActionEvent& action : actions) {
            if(action.action == UiAction::BuyUpgrade)
                pressedIndex = action.index;
        }

        const UpgradeShopConfigComponent* config = FindSingleton<UpgradeShopConfigComponent>(registry);

        //-------------------------------------------------------------
        // 押した瞬間は必ず1回買う。買えないときの知らせもここだけ
        //-------------------------------------------------------------
        if(pressedIndex >= 0) {
            EndHold();
            Buy(registry, pressedIndex, false);
            m_holdIndex    = pressedIndex;
            m_holdTimer    = config ? config->repeatDelay : 0.0f;
            m_holdInterval = config ? config->repeatInterval : 0.0f;
            return;
        }
        if(m_holdIndex < 0)
            return;

        //-------------------------------------------------------------
        // 押し続けているか。カードは、外へ出ると止まる
        //-------------------------------------------------------------
        bool held = m_holdIndex < kShopKeyCount && context->inputSystem->IsKeyDown(static_cast<KeyCode>(static_cast<int>(KeyCode::D1) + m_holdIndex));
        using Tsukino::BuiltIn::ECS::PointerTargetComponent;
        for(const auto entity : registry.View<UiActionComponent, PointerTargetComponent>()) {
            const UiActionComponent& action = registry.GetComponent<UiActionComponent>(entity);
            if(action.action == UiAction::BuyUpgrade && action.index == m_holdIndex)
                held = held || registry.GetComponent<PointerTargetComponent>(entity).pressed;
        }
        if(!held || !config) {
            EndHold();
            return;
        }

        //-------------------------------------------------------------
        // 続けて買う。だんだん速くなる。星屑が足りなければ、貯まるまで待つ
        //-------------------------------------------------------------
        m_holdTimer -= deltaTime;
        for(int i = 0; i < kMaxRepeatsPerFrame && m_holdTimer <= 0.0f; ++i) {
            m_holdTimer += m_holdInterval;
            if(!Buy(registry, m_holdIndex, true)) {
                m_holdTimer = std::max(m_holdTimer, 0.0f);
                break;
            }
            m_holdInterval = std::max(m_holdInterval * config->repeatAcceleration, config->repeatMinInterval);
        }
        m_holdTimer = std::max(m_holdTimer, -m_holdInterval);
    }

    //-------------------------------------------------------------
    // 長押しを終える
    //-------------------------------------------------------------
    void UpgradeShopSystem::EndHold() {
        if(m_holdCount > 0) {
            m_eventBus->Publish(UpgradeHoldEndedEvent{});
            m_eventBus->Publish(HudMessageEvent{m_holdName + " Lv" + std::to_string(m_holdLevel) + " ！  +" + std::to_string(m_holdCount),
                                                HudMessageImportance::Toast,
                                                HudMessageTone::Positive});
        }
        m_holdIndex = -1;
        m_holdCount = 0;
    }

    //-------------------------------------------------------------
    // 強化を1レベル買う
    //-------------------------------------------------------------
    bool UpgradeShopSystem::Buy(Tsukino::ECS::Registry& registry, int index, bool repeated) {
        const Data::UpgradeTable& table = Data::GetUpgradeTable();
        if(index >= static_cast<int>(table.upgrades.size()))
            return false;

        WalletComponent*        wallet = FindSingleton<WalletComponent>(registry);
        UpgradeLevelsComponent* levels = FindSingleton<UpgradeLevelsComponent>(registry);
        if(!wallet || !levels)
            return false;

        const Data::UpgradeDef& upgrade = table.upgrades[static_cast<size_t>(index)];
        int&                    level   = levels->levels[upgrade.id];
        if(Core::UpgradeFormula::IsMaxed(level, upgrade.maxLevel)) {
            if(repeated)
                return false;
            m_eventBus->Publish(UpgradeDeniedEvent{});
            m_eventBus->Publish(HudMessageEvent{upgrade.name + " は最大レベルです", HudMessageImportance::Toast, HudMessageTone::Warning});
            return false;
        }

        const Core::BigNumber cost = upgrade.CostAt(level);
        if(wallet->stardust < cost) {
            if(repeated)
                return false;
            m_eventBus->Publish(UpgradeDeniedEvent{});
            m_eventBus->Publish(HudMessageEvent{upgrade.name + "  星屑が足りません  あと " + (cost - wallet->stardust).ToJapaneseString(),
                                                HudMessageImportance::Toast,
                                                HudMessageTone::Warning});
            return false;
        }

        wallet->stardust -= cost;
        ++level;
        m_eventBus->Publish(UpgradePurchasedEvent{upgrade.id, level, repeated});
        if(repeated) {
            // 通知は積もるので、長押しが終わったときにまとめて出す
            ++m_holdCount;
            m_holdName  = upgrade.name;
            m_holdLevel = level;
            return true;
        }
        m_eventBus->Publish(HudMessageEvent{upgrade.name + " Lv" + std::to_string(level) + " ！", HudMessageImportance::Toast, HudMessageTone::Positive});
        return true;
    }

}    // namespace StardustFever::ECS

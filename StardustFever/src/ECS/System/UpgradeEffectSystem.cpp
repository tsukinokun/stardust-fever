//-------------------------------------------------------------
//! @file   UpgradeEffectSystem.cpp
//! @brief  UpgradeEffectSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/UpgradeEffectSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/Core/Log.hpp>

#include <string>
#include <unordered_set>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    // 効果を計算し直す
    //-------------------------------------------------------------
    void UpgradeEffectSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const UpgradeLevelsComponent* levels    = FindSingleton<UpgradeLevelsComponent>(registry);
        UpgradeModifiersComponent*    modifiers = FindSingleton<UpgradeModifiersComponent>(registry);
        if(!levels || !modifiers)
            return;

        UpgradeModifiersComponent result;
        for(const Data::UpgradeDef& upgrade : Data::GetUpgradeTable().upgrades) {
            const auto found = levels->levels.find(upgrade.id);
            const int  level = found == levels->levels.end() ? 0 : found->second;
            if(level <= 0)
                continue;

            const double value = upgrade.EffectAt(level);
            if(upgrade.effect == "pinPayout") {
                result.pinPayoutMultiplier *= value;
            } else if(upgrade.effect == "pocketPayout") {
                result.pocketPayoutMultiplier *= value;
            } else if(upgrade.effect == "lotteryChance") {
                result.lotteryChanceBonus += value;
            } else if(upgrade.effect == "feverDuration") {
                result.feverDurationBonus += static_cast<float>(value);
            } else if(upgrade.effect == "strokeBase") {
                result.strokeBaseBonus += value;
            } else if(upgrade.effect == "autoFireJitter") {
                result.autoFireJitterScale *= static_cast<float>(value);
            } else if(upgrade.effect == "autoFireUnlock") {
                result.autoFireUnlocked = true;
            } else {
                // 毎フレーム呼ばれるので、同じ書き間違いは1回だけ知らせる
                static std::unordered_set<std::string> s_ReportedEffects;
                if(s_ReportedEffects.insert(upgrade.effect).second)
                    Tsukino::Core::Log::Warn("Unknown upgrade effect '" + upgrade.effect + "' in Upgrades.json (upgrade '" + upgrade.id
                                             + "'). It has no effect.");
            }
        }
        *modifiers = result;

        for(const auto entity : registry.View<AutoFireComponent>()) {
            AutoFireComponent& autoFire = registry.GetComponent<AutoFireComponent>(entity);
            autoFire.unlocked           = result.autoFireUnlocked;
            // 解禁を取り消したとき（データの書き換えなど）に、自動発射だけ動き続けないようにする
            if(!autoFire.unlocked)
                autoFire.enabled = false;
        }
    }

}    // namespace StardustFever::ECS

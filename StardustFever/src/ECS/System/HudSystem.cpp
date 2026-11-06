//-------------------------------------------------------------
//! @file   HudSystem.cpp
//! @brief  HudSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/HudSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/FeverConfigComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HudTextComponent.hpp>
#include <StardustFever/ECS/Component/LauncherComponent.hpp>
#include <StardustFever/ECS/Component/LotteryConfigComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/RollingNumberComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        //-------------------------------------------------------------
        //! @brief  倍率を表示用にする
        //! @param  value [in] 倍率
        //! @return 1万未満は小数2桁（"×1.35"）、それ以上は和風の単位（"×12.3万"）
        //! @note   BigNumber::ToJapaneseString は1万未満を整数にするため、倍率の小数が消えてしまう
        //-------------------------------------------------------------
        std::string FormatMultiplier(const Core::BigNumber& value) {
            if(value.Exponent() >= 4)
                return "×" + value.ToJapaneseString();
            char buffer[32];
            std::snprintf(buffer, sizeof(buffer), "×%.2f", value.ToDouble());
            return buffer;
        }

        std::string BuildPrestigeHint(Tsukino::ECS::Registry& registry) {
            const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
            const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
            if(!prestige || !config)
                return {};

            const Core::BigNumber gain = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
            if(gain.IsZero()) {
                const Core::BigNumber needed = Core::PrestigeFormula::EarnedForNextConstant(gain, config->threshold, config->exponent);
                return "次の宇宙定数まで あと " + (needed - prestige->universeEarned).ToJapaneseString();
            }
            return "ビッグバンで Λ+" + gain.ToJapaneseString() + "  （B を2回）";
        }

        std::string BuildReel(Tsukino::ECS::Registry& registry, int index) {
            const LotteryStateComponent*  state  = FindSingleton<LotteryStateComponent>(registry);
            const LotteryConfigComponent* config = FindSingleton<LotteryConfigComponent>(registry);
            if(!state || !config || config->symbols.empty() || index < 0 || index >= static_cast<int>(state->symbols.size()))
                return {};
            const int symbolCount = static_cast<int>(config->symbols.size());
            return config->symbols[static_cast<size_t>(std::clamp(state->symbols[static_cast<size_t>(index)], 0, symbolCount - 1))];
        }

        std::string BuildAutoFire(Tsukino::ECS::Registry& registry) {
            const AutoFireComponent* autoFire = FindSingleton<AutoFireComponent>(registry);
            if(!autoFire)
                return {};
            if(!autoFire->unlocked) {
                // 解禁する強化の購入キーは Upgrades.json の並び順で決まるので、データから探す
                const auto& upgrades = Data::GetUpgradeTable().upgrades;
                for(size_t i = 0; i < upgrades.size(); ++i) {
                    if(upgrades[i].effect == "autoFire")
                        return "自動発射は強化 [" + std::to_string(i + 1) + "] で解禁";
                }
                return "自動発射 未解禁";
            }
            return autoFire->enabled ? "自動発射 ON ・ 強さが揺らぐ" : "自動発射 OFF";
        }

        //-------------------------------------------------------------
        //! @brief  1つの文字の中身を作る
        //-------------------------------------------------------------
        std::string BuildText(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, const HudTextComponent& hudText) {
            switch(hudText.kind) {
            case HudTextKind::Static:
                return hudText.text;
            case HudTextKind::Stardust:
                {
                    // 転がる数字が付いていれば、その表示中の値を出す
                    if(const RollingNumberComponent* rolling = registry.try_get<RollingNumberComponent>(entity); rolling && rolling->initialized)
                        return rolling->displayed.ToJapaneseString();
                    const WalletComponent* wallet = FindSingleton<WalletComponent>(registry);
                    return wallet ? wallet->stardust.ToJapaneseString() : std::string("-");
                }
            case HudTextKind::Multiplier:
                {
                    const ScoreMultiplierComponent* multiplier = FindSingleton<ScoreMultiplierComponent>(registry);
                    return multiplier ? FormatMultiplier(multiplier->total) : std::string("-");
                }
            case HudTextKind::Constants:
                {
                    const PrestigeStateComponent* prestige = FindSingleton<PrestigeStateComponent>(registry);
                    return "Λ" + (prestige ? prestige->cosmicConstants.ToJapaneseString() : std::string("0"));
                }
            case HudTextKind::ConstantsBonus:
                {
                    const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
                    const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
                    if(!prestige || !config)
                        return {};
                    return FormatMultiplier(Core::PrestigeFormula::Multiplier(prestige->cosmicConstants, config->bonusPerConstant));
                }
            case HudTextKind::PrestigeHint:
                return BuildPrestigeHint(registry);
            case HudTextKind::Reel:
                return BuildReel(registry, hudText.index);
            case HudTextKind::LaunchState:
                {
                    const LauncherComponent* launcher = FindSingleton<LauncherComponent>(registry);
                    return launcher && launcher->charging ? "溜め中" : "強さ";
                }
            case HudTextKind::LaunchPercent:
                {
                    const LauncherComponent* launcher = FindSingleton<LauncherComponent>(registry);
                    return launcher ? std::to_string(static_cast<int>(std::lround(launcher->handlePower * 100.0f))) + "%" : std::string();
                }
            case HudTextKind::AutoFire:
                return BuildAutoFire(registry);
            //-------------------------------------------------------------
            // 盤面の上の帯は、星ゲットチャンスとフィーバーで使い回す（同時には起きない）
            //-------------------------------------------------------------
            case HudTextKind::BannerHeading:
                {
                    const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry);
                    return chance && chance->active ? "★ 星ゲットチャンス ★" : "★ 星詠みタイム ★";
                }
            case HudTextKind::FeverTitle:
                {
                    if(const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry); chance && chance->active) {
                        const Data::ConstellationDef* constellation = Data::GetConstellationTable().Find(chance->constellationId);
                        return constellation ? constellation->name : chance->constellationId;
                    }
                    const FeverConfigComponent* config = FindSingleton<FeverConfigComponent>(registry);
                    if(!config)
                        return {};
                    char buffer[48];
                    std::snprintf(buffer, sizeof(buffer), "星屑 ×%g", config->pinPayoutMultiplier);
                    return buffer;
                }
            case HudTextKind::FeverProgress:
                {
                    const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry);
                    return chance && chance->active ? std::to_string(chance->starsLinked) + " / " + std::to_string(chance->starsTotal) : std::string();
                }
            case HudTextKind::FeverTime:
                {
                    float remaining = -1.0f;
                    if(const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry); chance && chance->active)
                        remaining = chance->timeRemaining;
                    else if(const FeverStateComponent* fever = FindSingleton<FeverStateComponent>(registry); fever && fever->active)
                        remaining = fever->timeRemaining;
                    return remaining >= 0.0f ? std::to_string(std::max(0, static_cast<int>(std::ceil(remaining)))) + "秒" : std::string();
                }
            case HudTextKind::FeverChain:
                {
                    const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry);
                    if(chance && chance->active)
                        return chance->completed ? "星を引いています…" : "全部つないで星をゲット";
                    return "チューリップが開く";
                }
            }
            return {};
        }
    }    // namespace

    //-------------------------------------------------------------
    // HUDの文字を更新する
    //-------------------------------------------------------------
    void HudSystem::Update(Tsukino::ECS::Registry& registry, float) {
        for(const auto entity : registry.View<HudTextComponent>()) {
            HudUi::SetText(registry, entity, BuildText(registry, entity, registry.GetComponent<HudTextComponent>(entity)));
        }
    }

}    // namespace StardustFever::ECS

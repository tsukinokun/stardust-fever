//-------------------------------------------------------------
//! @file   HudSystem.cpp
//! @brief  HudSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/HudSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/Core/TextUtility.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/HudConfigComponent.hpp>
#include <StardustFever/ECS/Component/HudStateComponent.hpp>
#include <StardustFever/ECS/Component/HudTextComponent.hpp>
#include <StardustFever/ECS/Component/LauncherComponent.hpp>
#include <StardustFever/ECS/Component/LotteryConfigComponent.hpp>
#include <StardustFever/ECS/Component/LotteryStateComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>

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

        std::string BuildStardustText(Tsukino::ECS::Registry& registry) {
            const WalletComponent* wallet = FindSingleton<WalletComponent>(registry);
            return "星屑  " + (wallet ? wallet->stardust.ToJapaneseString() : std::string("-"));
        }

        std::string BuildMultiplierText(Tsukino::ECS::Registry& registry) {
            const ScoreMultiplierComponent* multiplier = FindSingleton<ScoreMultiplierComponent>(registry);
            return "全体倍率  " + (multiplier ? FormatMultiplier(multiplier->total) : std::string("-"));
        }

        std::string BuildPrestigeText(Tsukino::ECS::Registry& registry) {
            const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
            const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
            if(!prestige || !config)
                return {};

            const Core::BigNumber multiplier = Core::PrestigeFormula::Multiplier(prestige->cosmicConstants, config->bonusPerConstant);
            std::string           text       = "宇宙定数 Λ" + prestige->cosmicConstants.ToJapaneseString() + "（" + FormatMultiplier(multiplier) + "）";

            const Core::BigNumber gain = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
            if(gain.IsZero()) {
                const Core::BigNumber needed  = Core::PrestigeFormula::EarnedForNextConstant(gain, config->threshold, config->exponent);
                text                         += "  次の宇宙定数まで あと " + (needed - prestige->universeEarned).ToJapaneseString();
            } else {
                text += "  ビッグバンで +" + gain.ToJapaneseString() + "（B を2回）";
            }
            return text;
        }

        std::string BuildHandleText(Tsukino::ECS::Registry& registry, const HudConfigComponent& config) {
            const LauncherComponent* launcher = FindSingleton<LauncherComponent>(registry);
            if(!launcher)
                return {};

            const int   segments = std::max(1, config.handleGaugeSegments);
            const int   filled   = static_cast<int>(std::lround(launcher->handlePower * static_cast<float>(segments)));
            std::string gauge;
            for(int i = 0; i < segments; ++i) {
                gauge += (i < filled) ? "■" : "□";
            }
            // 溜めている間は今の強さ、離した後は最後に打った強さ（自動発射の基準）を見せる
            const std::string label = launcher->charging ? "溜め中  " : "強さ    ";
            return label + gauge + "  " + std::to_string(static_cast<int>(std::lround(launcher->handlePower * 100.0f))) + "%";
        }

        std::string BuildAutoFireText(Tsukino::ECS::Registry& registry) {
            const AutoFireComponent* autoFire = FindSingleton<AutoFireComponent>(registry);
            if(!autoFire)
                return {};
            if(!autoFire->unlocked) {
                // 解禁する強化の購入キーは Upgrades.json の並び順で決まるので、データから探す
                const auto& upgrades = Data::GetUpgradeTable().upgrades;
                for(size_t i = 0; i < upgrades.size(); ++i) {
                    if(upgrades[i].effect == "autoFireUnlock")
                        return "自動発射  未解禁（強化 [" + std::to_string(i + 1) + "] で解禁）";
                }
                return "自動発射  未解禁";
            }
            return autoFire->enabled ? "自動発射  ON（強さが揺らぐ）  A: 切替" : "自動発射  OFF  A: 切替";
        }

        //-------------------------------------------------------------
        //! @brief  強化の効果量を表示用にする
        //! @param  upgrade [in] 強化の定義
        //! @param  level   [in] レベル
        //! @return 掛け算は "×1.95"、足し算は displayUnit に合わせて "+3%" / "+10秒" / "+0.5"
        //-------------------------------------------------------------
        std::string FormatUpgradeEffect(const Data::UpgradeDef& upgrade, int level) {
            const double value = upgrade.EffectAt(level);
            char         buffer[48];
            if(upgrade.Stacking() == Core::UpgradeFormula::Stacking::Multiply)
                std::snprintf(buffer, sizeof(buffer), "×%.2f", value);
            else if(upgrade.displayUnit == "%")
                std::snprintf(buffer, sizeof(buffer), "+%g%%", value * 100.0);
            else
                std::snprintf(buffer, sizeof(buffer), "+%g%s", value, upgrade.displayUnit.c_str());
            return buffer;
        }

        std::string BuildUpgradesText(Tsukino::ECS::Registry& registry) {
            const UpgradeLevelsComponent* levels = FindSingleton<UpgradeLevelsComponent>(registry);
            const WalletComponent*        wallet = FindSingleton<WalletComponent>(registry);
            if(!levels || !wallet)
                return {};

            std::string text     = "強化（数字キーで購入）";
            const auto& upgrades = Data::GetUpgradeTable().upgrades;
            for(size_t i = 0; i < upgrades.size(); ++i) {
                const Data::UpgradeDef& upgrade = upgrades[i];
                const auto              found   = levels->levels.find(upgrade.id);
                const int               level   = found == levels->levels.end() ? 0 : found->second;
                const bool              maxed   = Core::UpgradeFormula::IsMaxed(level, upgrade.maxLevel);
                const Core::BigNumber   cost    = upgrade.CostAt(level);

                // 買える行に印を付け、星屑が貯まったことにすぐ気づけるようにする
                const std::string mark  = maxed ? "　" : (wallet->stardust >= cost ? "◆" : "　");
                text                   += "\n" + mark + "[" + std::to_string(i + 1) + "] " + upgrade.name + " Lv" + std::to_string(level);
                text                   += maxed ? "  MAX" : "  星屑 " + cost.ToJapaneseString();
                if(level > 0 && upgrade.effect != "autoFireUnlock")
                    text += "  " + upgrade.description + " " + FormatUpgradeEffect(upgrade, level);
            }
            return text;
        }

        std::string BuildLotteryText(Tsukino::ECS::Registry& registry) {
            const LotteryStateComponent*  state  = FindSingleton<LotteryStateComponent>(registry);
            const LotteryConfigComponent* config = FindSingleton<LotteryConfigComponent>(registry);
            if(!state || !config || config->symbols.empty())
                return {};

            const int   symbolCount = static_cast<int>(config->symbols.size());
            std::string text;
            for(const int symbol : state->symbols) {
                text += "【" + config->symbols[static_cast<size_t>(std::clamp(symbol, 0, symbolCount - 1))] + "】";
            }
            text += "\n保留 ";
            for(int i = 0; i < config->maxPending; ++i) {
                text += (i < state->pending) ? "●" : "○";
            }
            return text;
        }

        std::string BuildFeverText(Tsukino::ECS::Registry& registry) {
            const FeverStateComponent* fever = FindSingleton<FeverStateComponent>(registry);
            if(!fever || !fever->active)
                return {};

            const Data::ConstellationDef* constellation  = Data::GetConstellationTable().Find(fever->constellationId);
            std::string                   text           = "★ 星詠みタイム ★  " + (constellation ? constellation->name : fever->constellationId);
            text                                        += "  " + std::to_string(fever->starsLinked) + " / " + std::to_string(fever->starsTotal);
            text                                        += "  残り " + std::to_string(std::max(0, static_cast<int>(std::ceil(fever->timeRemaining)))) + "秒";
            if(fever->bestChain >= 2)
                text += "\n一筆書き 最大 " + std::to_string(fever->bestChain) + "連";
            if(fever->completed && fever->perfect)
                text += "  パーフェクト！";
            return text;
        }

        std::string BuildCollectionText(Tsukino::ECS::Registry& registry) {
            const StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry);
            if(!collection)
                return {};

            const Data::StarTable& stars = Data::GetStarTable();
            std::string            text  = "星図";
            for(const Data::ConstellationDef& constellation : Data::GetConstellationTable().constellations) {
                const auto completions  = collection->constellationCompletions.find(constellation.id);
                const int  count        = completions == collection->constellationCompletions.end() ? 0 : completions->second;
                text                   += "\n" + constellation.name + "  完成 " + std::to_string(count) + "回";
                for(const Data::ConstellationStarDef& member : constellation.stars) {
                    const Data::StarDef* star   = stars.Find(member.starId);
                    const auto           level  = collection->starLevels.find(member.starId);
                    text                       += "\n  " + (star ? star->name : member.starId) + "  ";
                    text                       += level == collection->starLevels.end() ? "未発見" : "★" + std::to_string(level->second);
                }
            }
            return text;
        }
    }    // namespace

    HudSystem::HudSystem(Tsukino::ECS::EventBus& eventBus)
        : m_messages(eventBus) {
    }

    //-------------------------------------------------------------
    // HUDの文字を更新する
    //-------------------------------------------------------------
    void HudSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                messages = m_messages.Drain();
        HudStateComponent*        state    = FindSingleton<HudStateComponent>(registry);
        const HudConfigComponent* config   = FindSingleton<HudConfigComponent>(registry);
        if(!state || !config)
            return;

        //-------------------------------------------------------------
        // 同じフレームに届いたメッセージ（星の獲得と星座の完成など）は全て見せたいので改行でつなぐ
        //-------------------------------------------------------------
        if(!messages.empty()) {
            std::string combined;
            for(const HudMessageEvent& message : messages) {
                if(!combined.empty())
                    combined += "\n";
                combined += message.text;
            }
            state->message      = combined;
            state->messageTimer = config->messageDuration;
        }
        state->messageTimer = std::max(0.0f, state->messageTimer - deltaTime);
        if(state->messageTimer <= 0.0f)
            state->message.clear();

        using Tsukino::BuiltIn::ECS::FontComponent;
        for(const auto entity : registry.View<HudTextComponent, FontComponent>()) {
            const HudTextKind kind = registry.GetComponent<HudTextComponent>(entity).kind;
            std::string       text;
            switch(kind) {
            case HudTextKind::Stardust:
                text = BuildStardustText(registry);
                break;
            case HudTextKind::Multiplier:
                text = BuildMultiplierText(registry);
                break;
            case HudTextKind::Handle:
                text = BuildHandleText(registry, *config);
                break;
            case HudTextKind::AutoFire:
                text = BuildAutoFireText(registry);
                break;
            case HudTextKind::Lottery:
                text = BuildLotteryText(registry);
                break;
            case HudTextKind::Fever:
                text = BuildFeverText(registry);
                break;
            case HudTextKind::Message:
                text = state->message;
                break;
            case HudTextKind::Help:
                text = config->helpText;
                break;
            case HudTextKind::Collection:
                text = BuildCollectionText(registry);
                break;
            case HudTextKind::Prestige:
                text = BuildPrestigeText(registry);
                break;
            case HudTextKind::Upgrades:
                text = BuildUpgradesText(registry);
                break;
            }
            registry.GetComponent<FontComponent>(entity).text = Core::Utf8ToWide(text);
        }
    }

}    // namespace StardustFever::ECS

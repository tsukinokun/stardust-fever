//-------------------------------------------------------------
//! @file   UpgradeTooltipSystem.cpp
//! @brief  UpgradeTooltipSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/UpgradeTooltipSystem.hpp>

#include <StardustFever/Core/UpgradeFormula.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/LauncherConfigComponent.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/ShopCardComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeLevelsComponent.hpp>
#include <StardustFever/ECS/Component/WalletComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>
#include <StardustFever/ECS/Utility/UpgradeText.hpp>

#include <Tsukino/BuiltIn/ECS/Component/UIVisibilityComponent.hpp>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    UpgradeTooltipSystem::UpgradeTooltipSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 吹き出しを作る
    //-------------------------------------------------------------
    void UpgradeTooltipSystem::Build(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             style   = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!style || !context || !context->assetManager)
            return;

        const int order = style->sortOrder - 30;
        m_root          = UiParts::MakeNode(*m_scene, registry, entt::null, 0.0f, 0.0f);
        UiParts::AddCard(*m_scene, registry, *context->assetManager, *style, m_root, style->tooltipWidth, style->tooltipHeight, order);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(m_root).visible = false;

        const float left = -style->tooltipWidth * 0.5f + 18.0f;
        const float top  = -style->tooltipHeight * 0.5f;
        for(int i = 0; i < 3; ++i) {
            m_text[i] = UiParts::MakeNode(*m_scene, registry, m_root, left, top + 24.0f + 28.0f * static_cast<float>(i));
            UiParts::AddLabel(
                registry, m_text[i], "", style->textScale * (i == 0 ? 0.95f : 0.8f), order + 2, i == 0 ? style->titleColor : style->textColor, false);
            registry.GetComponent<Tsukino::BuiltIn::ECS::FontComponent>(m_text[i]).maxWidth = style->tooltipWidth - 36.0f;
        }
    }

    //-------------------------------------------------------------
    // 吹き出しを出し入れする
    //-------------------------------------------------------------
    void UpgradeTooltipSystem::Update(Tsukino::ECS::Registry& registry, float) {
        if(m_root == entt::null)
            Build(registry);
        const SettingsConfigComponent* style  = FindSingleton<SettingsConfigComponent>(registry);
        const UpgradeLevelsComponent*  levels = FindSingleton<UpgradeLevelsComponent>(registry);
        const WalletComponent*         wallet = FindSingleton<WalletComponent>(registry);
        const LauncherConfigComponent* launch = FindSingleton<LauncherConfigComponent>(registry);
        if(m_root == entt::null || !style || !levels || !wallet || !launch)
            return;

        using Tsukino::BuiltIn::ECS::PointerTargetComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;
        entt::entity hovered = entt::null;
        for(const auto entity : registry.View<ShopCardComponent, PointerTargetComponent>()) {
            if(registry.GetComponent<PointerTargetComponent>(entity).hovered)
                hovered = entity;
        }
        registry.GetComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(m_root).visible = hovered != entt::null;
        if(hovered == entt::null)
            return;

        const auto& upgrades = Data::GetUpgradeTable().upgrades;
        const int   index    = registry.GetComponent<ShopCardComponent>(hovered).index;
        if(index < 0 || index >= static_cast<int>(upgrades.size()))
            return;
        const Data::UpgradeDef& upgrade = upgrades[static_cast<size_t>(index)];
        const auto              found   = levels->levels.find(upgrade.id);
        const int               level   = found == levels->levels.end() ? 0 : found->second;
        const bool              maxed   = Core::UpgradeFormula::IsMaxed(level, upgrade.maxLevel);

        //-------------------------------------------------------------
        // カードの右へ置く
        //-------------------------------------------------------------
        TransformComponent& transform = registry.GetComponent<TransformComponent>(m_root);
        const float         cardY     = registry.GetComponent<TransformComponent>(hovered).position.y;
        transform.position            = hlslpp::float3(style->tooltipX, cardY, 0.0f);
        transform.dirty               = true;

        //-------------------------------------------------------------
        // 今 → 次
        //-------------------------------------------------------------
        std::string change;
        if(upgrade.effect == "autoFire") {
            change = "発射間隔  " + (level > 0 ? UpgradeText::Interval(upgrade, level, launch->autoFireInterval) : std::string("なし"));
            if(!maxed)
                change += " → " + UpgradeText::Interval(upgrade, level + 1, launch->autoFireInterval);
        } else {
            change = upgrade.description + "  " + (level > 0 ? UpgradeText::Effect(upgrade, level) : std::string("なし"));
            if(!maxed)
                change += " → " + UpgradeText::Effect(upgrade, level + 1);
        }

        std::string price = "これ以上は上がりません";
        if(!maxed) {
            const Core::BigNumber cost = upgrade.CostAt(level);
            price                      = "値段 " + cost.ToJapaneseString();
            if(wallet->stardust < cost)
                price += "  あと " + (cost - wallet->stardust).ToJapaneseString();
        }

        HudUi::SetText(registry,
                       m_text[0],
                       upgrade.name + "  Lv" + std::to_string(level) + (upgrade.maxLevel > 0 ? " / " + std::to_string(upgrade.maxLevel) : std::string()));
        HudUi::SetText(registry, m_text[1], change);
        HudUi::SetText(registry, m_text[2], price);
    }

}    // namespace StardustFever::ECS

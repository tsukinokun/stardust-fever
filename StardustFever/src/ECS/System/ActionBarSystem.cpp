//-------------------------------------------------------------
//! @file   ActionBarSystem.cpp
//! @brief  ActionBarSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/ActionBarSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/ECS/Component/AutoFireComponent.hpp>
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    ActionBarSystem::ActionBarSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // ボタンを作る
    //-------------------------------------------------------------
    void ActionBarSystem::Build(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             style   = FindSingleton<SettingsConfigComponent>(registry);
        const HudLayoutConfigComponent*            layout  = FindSingleton<HudLayoutConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!style || !layout || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager& assets = *context->assetManager;

        const int   order = style->sortOrder - 40;
        const float width = style->actionButtonWidth;
        const float y     = style->actionBarY;
        const float x     = style->actionBarX;
        const float gap   = width + style->actionButtonGap;
        m_autoFire        = UiParts::MakeButton(
            *m_scene, registry, assets, *style, entt::null, x, y, width, style->actionButtonHeight, order, "自動発射", UiAction::ToggleAutoFire);
        UiParts::MakeButton(*m_scene, registry, assets, *style, entt::null, x + gap, y, width, style->actionButtonHeight, order, "星図", UiAction::OpenMap);
        m_bigBang = UiParts::MakeButton(
            *m_scene, registry, assets, *style, entt::null, x + gap * 2.0f, y, width, style->actionButtonHeight, order, "ビッグバン", UiAction::BigBang);

        // 右のパネルの「そろったグループ」の行。行の Y は文字の中心
        const entt::entity row =
            UiParts::MakeNode(*m_scene, registry, entt::null, layout->chartX + layout->chartWidth * 0.5f, layout->chartY + layout->chartHeaderSpacing);
        UiParts::AddImage(registry,
                          row,
                          assets.Load(Tsukino::Core::Path(style->whiteTexture)),
                          layout->chartWidth,
                          layout->chartHeaderSpacing,
                          order,
                          hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f));
        UiParts::MakeClickable(registry, row, UiAction::OpenGroups);
    }

    //-------------------------------------------------------------
    // ボタンを作り、今の状態に合わせる
    //-------------------------------------------------------------
    void ActionBarSystem::Update(Tsukino::ECS::Registry& registry, float) {
        if(m_autoFire == entt::null)
            Build(registry);
        const SettingsConfigComponent* style = FindSingleton<SettingsConfigComponent>(registry);
        if(m_autoFire == entt::null || !style)
            return;

        //-------------------------------------------------------------
        // 自動発射: 入なら金色、未解禁なら沈める
        //-------------------------------------------------------------
        if(const AutoFireComponent* autoFire = FindSingleton<AutoFireComponent>(registry)) {
            UiActionComponent& button = registry.GetComponent<UiActionComponent>(m_autoFire);
            const bool         on     = autoFire->unlocked && autoFire->enabled;
            button.baseColor          = on ? style->onColor : style->buttonColor;
            HudUi::SetText(registry, button.label, !autoFire->unlocked ? "自動発射" : (on ? "自動 ON" : "自動 OFF"));
            HudUi::SetTextColor(registry, button.label, autoFire->unlocked ? style->textColor : style->textColor * hlslpp::float4(1.0f, 1.0f, 1.0f, 0.45f));
        }

        //-------------------------------------------------------------
        // ビッグバン: できるときだけ金色にする
        //-------------------------------------------------------------
        const PrestigeStateComponent*  prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent* config   = FindSingleton<PrestigeConfigComponent>(registry);
        if(prestige && config) {
            UiActionComponent& button = registry.GetComponent<UiActionComponent>(m_bigBang);
            const bool         ready  = !Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent).IsZero();
            button.baseColor          = ready ? style->onColor : style->buttonColor;
            HudUi::SetTextColor(registry, button.label, ready ? style->textColor : style->textColor * hlslpp::float4(1.0f, 1.0f, 1.0f, 0.45f));
        }
    }

}    // namespace StardustFever::ECS

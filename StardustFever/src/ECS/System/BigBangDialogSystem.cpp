//-------------------------------------------------------------
//! @file   BigBangDialogSystem.cpp
//! @brief  BigBangDialogSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BigBangDialogSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    BigBangDialogSystem::BigBangDialogSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 確認画面を作り・片付ける
    //-------------------------------------------------------------
    void BigBangDialogSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const PrestigeStateComponent* prestige = FindSingleton<PrestigeStateComponent>(registry);
        if(!prestige)
            return;
        if(prestige->confirmOpen && m_root == entt::null)
            Build(registry);
        if(!prestige->confirmOpen && m_root != entt::null) {
            if(registry.IsValid(m_root))
                Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
            m_root = entt::null;
        }
    }

    //-------------------------------------------------------------
    // 確認画面を作る
    //-------------------------------------------------------------
    void BigBangDialogSystem::Build(Tsukino::ECS::Registry& registry) {
        const SettingsConfigComponent*             style    = FindSingleton<SettingsConfigComponent>(registry);
        const PrestigeStateComponent*              prestige = FindSingleton<PrestigeStateComponent>(registry);
        const PrestigeConfigComponent*             config   = FindSingleton<PrestigeConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context  = GetEngineContext(registry);
        if(!style || !prestige || !config || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const int                     order  = style->sortOrder;

        m_root = UiParts::MakeNode(*m_scene, registry, entt::null, 0.0f, 0.0f);

        // 後ろの幕。後ろの部品を押せないよう、当たりも受ける
        UiParts::AddImage(registry,
                          UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f),
                          assets.Load(Tsukino::Core::Path(style->whiteTexture)),
                          1600.0f,
                          1000.0f,
                          order,
                          style->dimColor)
            .raycastTarget = true;

        const float        width  = 620.0f;
        const float        height = 360.0f;
        const entt::entity card   = UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f);
        UiParts::AddCard(*m_scene, registry, assets, *style, card, width, height, order + 2);

        const float left = -width * 0.5f + 44.0f;
        const float top  = -height * 0.5f;
        const auto  line = [&](const std::string& text, float y, float scale, const hlslpp::float4& color) {
            UiParts::AddLabel(registry, UiParts::MakeNode(*m_scene, registry, card, left, top + y), text, scale, order + 6, color, false);
        };

        const Core::BigNumber gain = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
        line("ビッグバン", 52.0f, style->titleScale, style->titleColor);
        line("宇宙定数 +" + gain.ToJapaneseString(), 112.0f, style->textScale * 1.2f, style->titleColor);
        line("消えるもの  星屑・強化・盤面の玉", 166.0f, style->textScale, style->textColor);
        line("残るもの  集めた星と星座・宇宙定数", 204.0f, style->textScale, style->textColor);

        const float        buttonY = height * 0.5f - 52.0f;
        const float        buttonW = style->closeButtonWidth;
        const float        buttonH = style->closeButtonHeight;
        const entt::entity run =
            UiParts::MakeButton(*m_scene, registry, assets, *style, card, -110.0f, buttonY, buttonW, buttonH, order + 5, "実行する", UiAction::BigBangConfirm);
        registry.GetComponent<UiActionComponent>(run).baseColor = style->onColor;
        UiParts::MakeButton(*m_scene, registry, assets, *style, card, 110.0f, buttonY, buttonW, buttonH, order + 5, "やめる", UiAction::BigBangCancel);

        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, card, 0.06f, 0.35f);
    }

}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   BigBangDialogSystem.cpp
//! @brief  BigBangDialogSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BigBangDialogSystem.hpp>

#include <StardustFever/Core/PrestigeFormula.hpp>
#include <StardustFever/ECS/Component/PrestigeConfigComponent.hpp>
#include <StardustFever/ECS/Component/PrestigeStateComponent.hpp>
#include <StardustFever/ECS/Component/ScoreMultiplierComponent.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Utility/CollectionText.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/SkyObjectOdds.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <algorithm>
#include <cstdio>
#include <string>

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

        const float        width  = 700.0f;
        const float        height = 640.0f;
        const entt::entity card   = UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f);
        UiParts::AddCard(*m_scene, registry, assets, *style, card, width, height, order + 2);

        const float left = -width * 0.5f + 44.0f;
        const float top  = -height * 0.5f;
        const auto  text = [&](const std::string& value, float x, float y, float scale, const hlslpp::float4& color) {
            UiParts::AddLabel(registry, UiParts::MakeNode(*m_scene, registry, card, left + x, top + y), value, scale, order + 6, color, false);
        };

        //-------------------------------------------------------------
        // 宇宙定数の説明
        //-------------------------------------------------------------
        const Core::BigNumber gain = Core::PrestigeFormula::GainOnBigBang(prestige->universeEarned, config->threshold, config->exponent);
        text("ビッグバン", 0.0f, 52.0f, style->titleScale, style->titleColor);
        std::string headline = "宇宙定数 +" + gain.ToJapaneseString();
        if(const ScoreMultiplierComponent* multiplier = FindSingleton<ScoreMultiplierComponent>(registry)) {
            const Core::BigNumber next  = Core::PrestigeFormula::MultiplierAfter(multiplier->total, prestige->cosmicConstants, gain, config->bonusPerConstant);
            headline                   += "   全体倍率 " + CollectionText::Multiplier(multiplier->total) + " → " + CollectionText::Multiplier(next);
        }
        text(headline, 0.0f, 106.0f, style->textScale * 1.15f, style->titleColor);

        char bonus[128];
        std::snprintf(bonus, sizeof(bonus), "1つにつき全体倍率 +%.1f。ビッグバンをしても消えません。", config->bonusPerConstant);
        text("宇宙定数は、この宇宙で稼いだ分だけ得られる永続の力です。", 0.0f, 148.0f, style->textScale * 0.9f, style->textColor);
        text(bonus, 0.0f, 178.0f, style->textScale * 0.9f, style->textColor);
        text("消えるもの  星屑・強化・盤面の玉", 0.0f, 224.0f, style->textScale * 0.9f, style->textColor);
        text("残るもの  集めた星と星座・宇宙定数・天体", 0.0f, 254.0f, style->textScale * 0.9f, style->textColor);

        //-------------------------------------------------------------
        // 手に入る天体。確率は実際の抽選と同じ計算
        //-------------------------------------------------------------
        const SkyObjectConfigComponent* objects    = FindSingleton<SkyObjectConfigComponent>(registry);
        const StarCollectionComponent*  collection = FindSingleton<StarCollectionComponent>(registry);
        if(objects && collection) {
            const SkyObjectOdds odds = ComputeSkyObjectOdds(*objects, *collection, prestige->universeEarned, config->threshold);
            if(std::all_of(odds.rates.begin(), odds.rates.end(), [](double rate) { return rate <= 0.0; })) {
                text("天体はすべて集めました", 0.0f, 304.0f, style->textScale, style->titleColor);
            } else {
                text("天体を1つ必ず手に入れます", 0.0f, 304.0f, style->textScale, style->titleColor);
                for(size_t i = 0; i < odds.rates.size(); ++i) {
                    const float          y     = 340.0f + static_cast<float>(i) * 30.0f;
                    const hlslpp::float4 color = i < objects->rarityColors.size() ? objects->rarityColors[i] : style->textColor;
                    char                 rate[32];
                    std::snprintf(rate, sizeof(rate), "%.1f%%", odds.rates[i] * 100.0);
                    text(i < objects->rarityNames.size() ? objects->rarityNames[i] : std::string(), 0.0f, y, style->textScale * 0.9f, color);
                    text(rate, 220.0f, y, style->textScale * 0.9f, color);
                    text("残り " + std::to_string(odds.remaining[i]), 340.0f, y, style->textScale * 0.9f, color);
                }
                text("この宇宙で稼ぐほど、高いレア度が出やすくなります", 0.0f, 472.0f, style->textScale * 0.8f, style->textColor);
            }
        }

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

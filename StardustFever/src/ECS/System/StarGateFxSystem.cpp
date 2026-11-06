//-------------------------------------------------------------
//! @file   StarGateFxSystem.cpp
//! @brief  StarGateFxSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarGateFxSystem.hpp>

#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Component/UICanvasGroupComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        constexpr int   kStarCount     = 8;
        constexpr float kPi            = 3.14159265f;
        constexpr float kResultSeconds = 0.9f;    //!< 結果を見せる時間（秒）
        constexpr int   kSortOrder     = 56;      //!< HUD より手前、星の演出より奥

        const hlslpp::float4 kBlue(0.45f, 0.7f, 1.0f, 1.0f);
        const hlslpp::float4 kGold(1.0f, 0.82f, 0.35f, 1.0f);
        const hlslpp::float4 kDim(0.25f, 0.28f, 0.42f, 0.8f);
    }    // namespace

    StarGateFxSystem::StarGateFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus)
        : m_scene(&scene)
        , m_eventBus(&eventBus)
        , m_events(eventBus) {
    }

    //-------------------------------------------------------------
    // 輪を作る
    //-------------------------------------------------------------
    void StarGateFxSystem::Build(Tsukino::ECS::Registry& registry) {
        const StarChanceConfigComponent*           config  = FindSingleton<StarChanceConfigComponent>(registry);
        const SettingsConfigComponent*             style   = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !style || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const auto                    star   = assets.Load(Tsukino::Core::Path(style->starTexture));
        const float                   size   = (config->rollRingRadius + config->rollStarSize) * 2.0f;

        m_root = UiParts::MakeNode(*m_scene, registry, entt::null, config->rollRingX, config->rollRingY);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_root);

        // 盤面の釘の上でも星が見えるよう、丸い濃紺の面を敷く
        UiParts::SetSlice(
            UiParts::AddImage(
                registry, m_root, assets.Load(Tsukino::Core::Path(style->pillTexture)), size, size, kSortOrder, hlslpp::float4(0.012f, 0.015f, 0.05f, 0.92f)),
            UiParts::kPillBorder,
            size / (UiParts::kPillBorder * 2.0f));

        m_stars.clear();
        for(int i = 0; i < kStarCount; ++i) {
            const float        angle = -kPi * 0.5f + kPi * 2.0f * static_cast<float>(i) / static_cast<float>(kStarCount);
            const entt::entity node =
                UiParts::MakeNode(*m_scene, registry, m_root, std::cos(angle) * config->rollRingRadius, std::sin(angle) * config->rollRingRadius);
            UiParts::AddImage(registry, node, star, config->rollStarSize, config->rollStarSize, kSortOrder + 1, kDim);
            m_stars.push_back(node);
        }

        m_center = UiParts::MakeNode(*m_scene, registry, m_root, 0.0f, 0.0f);
        UiParts::AddLabel(registry, m_center, "？", style->titleScale * 1.2f, kSortOrder + 2, style->titleColor, true);
        m_caption = UiParts::MakeNode(*m_scene, registry, m_root, 0.0f, -config->rollRingRadius - config->rollStarSize - 24.0f);
        UiParts::AddLabel(registry, m_caption, "星の門", style->textScale, kSortOrder + 2, style->textColor, true);

        m_litStar = -1;
        m_result  = 0;
        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, m_root, 0.15f, 0.3f);
    }

    //-------------------------------------------------------------
    // 輪を片付ける
    //-------------------------------------------------------------
    void StarGateFxSystem::Clear(Tsukino::ECS::Registry& registry) {
        if(m_root != entt::null && registry.IsValid(m_root))
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
        m_root = entt::null;
        m_stars.clear();
    }

    //-------------------------------------------------------------
    // 演出を進める
    //-------------------------------------------------------------
    void StarGateFxSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const auto                       events = m_events.Drain();
        const StarChanceStateComponent*  state  = FindSingleton<StarChanceStateComponent>(registry);
        const StarChanceConfigComponent* config = FindSingleton<StarChanceConfigComponent>(registry);
        if(!state || !config)
            return;

        for(const StarGateRollEvent& event : events) {
            if(event.stage == StarGateRollStage::Start) {
                Clear(registry);
                Build(registry);
            } else if(m_root != entt::null && (event.stage == StarGateRollStage::Win || event.stage == StarGateRollStage::Miss)) {
                const bool win = event.stage == StarGateRollStage::Win;
                m_result       = win ? 1 : 2;
                m_resultTime   = 0.0f;
                HudUi::SetText(registry, m_center, win ? "！" : "…");
                HudUi::SetText(registry, m_caption, win ? "星ゲットチャンス！" : "星は応えなかった");
                if(win)
                    Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, m_root, 0.35f, 0.45f);
            }
        }
        if(m_root == entt::null)
            return;

        const hlslpp::float4& color = state->rollGold ? kGold : kBlue;

        //-------------------------------------------------------------
        // 抽選中: 光が回る。残り時間の2.5乗で進めるので、だんだん遅くなる
        //-------------------------------------------------------------
        if(m_result == 0) {
            const float progress = 1.0f - std::clamp(state->rollTimer / std::max(state->rollTotal, 0.01f), 0.0f, 1.0f);
            const float travel   = (1.0f - std::pow(1.0f - progress, 2.5f)) * config->rollRevolutions;
            const int   lit      = static_cast<int>(travel * kStarCount) % kStarCount;
            if(lit != m_litStar) {
                m_litStar = lit;
                m_eventBus->Publish(StarGateRollEvent{StarGateRollStage::Tick});
            }
            for(int i = 0; i < kStarCount; ++i) {
                // 光の後ろの星は少し残す（尾を引いて回って見える）
                const int           behind    = (lit - i + kStarCount) % kStarCount;
                const float         glow      = behind == 0 ? 1.0f : (behind == 1 ? 0.55f : (behind == 2 ? 0.25f : 0.0f));
                const float         scale     = 1.0f + 0.5f * glow;
                TransformComponent& transform = registry.GetComponent<TransformComponent>(m_stars[static_cast<size_t>(i)]);
                transform.scale               = hlslpp::float3(scale, scale, 1.0f);
                transform.dirty               = true;
                registry.GetComponent<SpriteComponent>(m_stars[static_cast<size_t>(i)]).tintColor = kDim + (color - kDim) * glow;
            }
            return;
        }

        //-------------------------------------------------------------
        // 結果: 当たりは全部の星が光って外へ広がり、外れは暗くなって消える
        //-------------------------------------------------------------
        m_resultTime  += deltaTime;
        const float t  = std::clamp(m_resultTime / kResultSeconds, 0.0f, 1.0f);
        for(int i = 0; i < kStarCount; ++i) {
            const entt::entity  star                               = m_stars[static_cast<size_t>(i)];
            const float         scale                              = m_result == 1 ? 1.5f + 0.5f * std::sin(t * kPi) : 1.0f;
            TransformComponent& transform                          = registry.GetComponent<TransformComponent>(star);
            transform.scale                                        = hlslpp::float3(scale, scale, 1.0f);
            transform.dirty                                        = true;
            registry.GetComponent<SpriteComponent>(star).tintColor = m_result == 1 ? kGold : kDim;
        }
        registry.GetComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_root).alpha = t < 0.6f ? 1.0f : (1.0f - t) / 0.4f;
        if(t >= 1.0f)
            Clear(registry);
    }

}    // namespace StardustFever::ECS

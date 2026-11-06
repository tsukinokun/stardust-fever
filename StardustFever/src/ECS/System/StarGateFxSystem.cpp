//-------------------------------------------------------------
//! @file   StarGateFxSystem.cpp
//! @brief  StarGateFxSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarGateFxSystem.hpp>

#include <StardustFever/Core/StarGateRoll.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/StarMapGeometry.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Component/UICanvasGroupComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIVisibilityComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Core/ECS/Event/EventBus.hpp>

#include <algorithm>
#include <cmath>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::SpriteBlendMode;
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;
        namespace Roll = Core::StarGateRoll;

        constexpr float kResultSeconds = 1.0f;      //!< 結果を見せる時間（秒）
        constexpr int   kSortOrder     = 56;        //!< HUD より手前、星の演出より奥
        constexpr int   kDustCount     = 36;        //!< 金色の予告の星屑の数
        constexpr float kDotSize       = 44.0f;     //!< 星の光の大きさ
        constexpr float kScatter       = 170.0f;    //!< 線が散る距離

        const hlslpp::float4 kBlue(0.5f, 0.75f, 1.0f, 1.0f);
        const hlslpp::float4 kGold(1.0f, 0.82f, 0.35f, 1.0f);
        const hlslpp::float4 kDim(0.3f, 0.34f, 0.5f, 0.55f);

        //-------------------------------------------------------------
        //! @brief  部品の位置と大きさを書く
        //-------------------------------------------------------------
        void Move(Tsukino::ECS::Registry& registry, entt::entity entity, float x, float y, float scale) {
            TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
            transform.position            = hlslpp::float3(x, y, 0.0f);
            transform.scale               = hlslpp::float3(scale, scale, 1.0f);
            transform.dirty               = true;
        }
    }    // namespace

    StarGateFxSystem::StarGateFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus)
        : m_scene(&scene)
        , m_eventBus(&eventBus)
        , m_events(eventBus)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 画面を作る
    //-------------------------------------------------------------
    void StarGateFxSystem::Build(Tsukino::ECS::Registry& registry) {
        const StarChanceConfigComponent*           config  = FindSingleton<StarChanceConfigComponent>(registry);
        const SettingsConfigComponent*             style   = FindSingleton<SettingsConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !style || !context || !context->assetManager)
            return;
        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const float                   x      = config->rollFigureX;
        const float                   y      = config->rollFigureY;

        m_root = UiParts::MakeNode(*m_scene, registry, entt::null, 0.0f, 0.0f);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_root);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(m_root);

        // 画面全体を暗くする。当たりは受けないので、玉はそのまま打てる
        UiParts::AddImage(registry,
                          UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f),
                          assets.Load(Tsukino::Core::Path(style->whiteTexture)),
                          1600.0f,
                          1000.0f,
                          kSortOrder,
                          hlslpp::float4(0.0f, 0.0f, 0.03f, config->rollVeilAlpha));

        m_nebula = UiParts::MakeNode(*m_scene, registry, m_root, x, y);
        UiParts::AddImage(registry,
                          m_nebula,
                          assets.Load(Tsukino::Core::Path(config->rollNebulaTexture)),
                          900.0f,
                          900.0f,
                          kSortOrder + 1,
                          hlslpp::float4(1.0f, 1.0f, 1.0f, 0.7f))
            .blendMode = SpriteBlendMode::Additive;
        m_glow         = UiParts::MakeNode(*m_scene, registry, m_root, x, y);
        UiParts::AddImage(registry, m_glow, assets.Load(Tsukino::Core::Path(style->glowTexture)), 760.0f, 760.0f, kSortOrder + 1, kBlue).blendMode =
            SpriteBlendMode::Additive;

        m_dust.clear();
        const auto dustImage = assets.Load(Tsukino::Core::Path(config->rollDustTexture));
        for(int i = 0; i < kDustCount; ++i) {
            Dust dust;
            dust.entity = UiParts::MakeNode(*m_scene, registry, m_root, 0.0f, 0.0f);
            UiParts::AddImage(registry, dust.entity, dustImage, 26.0f, 26.0f, kSortOrder + 2, hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f)).blendMode =
                SpriteBlendMode::Additive;
            std::uniform_real_distribution<float> screenX(120.0f, 1480.0f);
            std::uniform_real_distribution<float> screenY(0.0f, 1000.0f);
            std::uniform_real_distribution<float> fall(70.0f, 210.0f);
            std::uniform_real_distribution<float> drift(-40.0f, 40.0f);
            dust.position = hlslpp::float2(screenX(m_random), screenY(m_random));
            dust.velocity = hlslpp::float2(drift(m_random), fall(m_random));
            m_dust.push_back(dust);
        }

        m_caption = UiParts::MakeNode(*m_scene, registry, m_root, x, y - config->rollFigureHeight * 0.5f - 66.0f);
        UiParts::AddLabel(registry, m_caption, "星の門", style->titleScale, kSortOrder + 6, style->titleColor, true);
        m_name = UiParts::MakeNode(*m_scene, registry, m_root, x, y + config->rollFigureHeight * 0.5f + 62.0f);
        UiParts::AddLabel(registry, m_name, "", style->titleScale * 1.15f, kSortOrder + 6, style->textColor, true);

        m_figure = entt::null;
        m_step   = -1;
        m_lit    = -1;
        m_phase  = static_cast<int>(Roll::Phase::Spin);
        m_result = 0;
        m_time   = 0.0f;
        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, m_glow, 0.3f, 0.4f);
    }

    //-------------------------------------------------------------
    // 画面を片付ける
    //-------------------------------------------------------------
    void StarGateFxSystem::Clear(Tsukino::ECS::Registry& registry) {
        if(m_root != entt::null && registry.IsValid(m_root))
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
        m_root   = entt::null;
        m_figure = entt::null;
        m_lines.clear();
        m_dots.clear();
        m_dust.clear();
    }

    //-------------------------------------------------------------
    // 線画を作り直す
    //-------------------------------------------------------------
    void StarGateFxSystem::ShowConstellation(Tsukino::ECS::Registry& registry, const std::string& id) {
        const StarChanceConfigComponent*           config        = FindSingleton<StarChanceConfigComponent>(registry);
        const SettingsConfigComponent*             style         = FindSingleton<SettingsConfigComponent>(registry);
        const Data::ConstellationDef*              constellation = Data::GetConstellationTable().Find(id);
        Tsukino::EngineIntegration::EngineContext* context       = GetEngineContext(registry);
        if(!config || !style || !constellation || !context || !context->assetManager)
            return;

        if(m_figure != entt::null && registry.IsValid(m_figure))
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_figure);
        m_figure = UiParts::MakeNode(*m_scene, registry, m_root, config->rollFigureX, config->rollFigureY);
        m_lines.clear();
        m_dots.clear();
        HudUi::SetText(registry, m_name, constellation->name);

        StarMapGeometry::Figure figure;
        if(!StarMapGeometry::FitFigure(*constellation, Data::GetStarTable(), config->rollFigureWidth, config->rollFigureHeight, figure))
            return;

        //-------------------------------------------------------------
        // 灯る順は線をたどる順にする。星が線に沿って順につながって見える
        //-------------------------------------------------------------
        const int        count = static_cast<int>(figure.points.size());
        std::vector<int> order(static_cast<size_t>(count), -1);
        int              next  = 0;
        const auto       visit = [&](int index) {
            if(index >= 0 && index < count && order[static_cast<size_t>(index)] < 0 && figure.defs[static_cast<size_t>(index)])
                order[static_cast<size_t>(index)] = next++;
        };
        for(const Data::ConstellationLineDef& line : constellation->lines) {
            visit(line.from);
            visit(line.to);
        }
        for(int i = 0; i < count; ++i) {
            visit(i);
        }

        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const auto                    white  = assets.Load(Tsukino::Core::Path(style->whiteTexture));
        const auto                    image  = assets.Load(Tsukino::Core::Path(config->rollStarTexture));
        for(const Data::ConstellationLineDef& line : constellation->lines) {
            if(line.from < 0 || line.from >= count || line.to < 0 || line.to >= count || !figure.defs[static_cast<size_t>(line.from)]
               || !figure.defs[static_cast<size_t>(line.to)])
                continue;
            const float        x0     = static_cast<float>(figure.points[static_cast<size_t>(line.from)].x);
            const float        y0     = static_cast<float>(figure.points[static_cast<size_t>(line.from)].y);
            const float        x1     = static_cast<float>(figure.points[static_cast<size_t>(line.to)].x);
            const float        y1     = static_cast<float>(figure.points[static_cast<size_t>(line.to)].y);
            const entt::entity entity = UiParts::MakeNode(*m_scene, registry, m_figure, (x0 + x1) * 0.5f, (y0 + y1) * 0.5f);
            UiParts::AddImage(registry, entity, white, std::hypot(x1 - x0, y1 - y0), 3.0f, kSortOrder + 3, kDim);
            registry.GetComponent<TransformComponent>(entity).rotation = hlslpp::quaternion::rotation_z(std::atan2(y1 - y0, x1 - x0));
            m_lines.push_back(
                {entity, order[static_cast<size_t>(line.from)], order[static_cast<size_t>(line.to)], hlslpp::float2((x0 + x1) * 0.5f, (y0 + y1) * 0.5f)});
        }
        for(int i = 0; i < count; ++i) {
            if(!figure.defs[static_cast<size_t>(i)])
                continue;
            const float        x      = static_cast<float>(figure.points[static_cast<size_t>(i)].x);
            const float        y      = static_cast<float>(figure.points[static_cast<size_t>(i)].y);
            const entt::entity entity = UiParts::MakeNode(*m_scene, registry, m_figure, x, y);
            UiParts::AddImage(registry, entity, image, kDotSize, kDotSize, kSortOrder + 4, kDim).blendMode = SpriteBlendMode::Additive;
            m_dots.push_back({entity, order[static_cast<size_t>(i)], hlslpp::float2(x, y)});
        }
    }

    //-------------------------------------------------------------
    // 線画の灯りを決める
    //-------------------------------------------------------------
    void StarGateFxSystem::ApplyLit(Tsukino::ECS::Registry& registry, int lit, const hlslpp::float4& color) {
        for(const FigureLine& line : m_lines) {
            registry.GetComponent<SpriteComponent>(line.entity).tintColor = line.from < lit && line.to < lit ? color : kDim;
        }
        for(const FigureDot& dot : m_dots) {
            const bool on                                                = dot.order < lit;
            registry.GetComponent<SpriteComponent>(dot.entity).tintColor = on ? color : kDim;
            Move(registry, dot.entity, dot.base.x, dot.base.y, on ? 1.5f : 0.8f);
        }
    }

    //-------------------------------------------------------------
    // 線をほどいて散らす
    //-------------------------------------------------------------
    void StarGateFxSystem::Scatter(Tsukino::ECS::Registry& registry, float t) {
        const float eased = 1.0f - (1.0f - t) * (1.0f - t);
        const auto  away  = [&](const hlslpp::float2& base) {
            const float length = std::max(std::hypot(static_cast<float>(base.x), static_cast<float>(base.y)), 1.0f);
            return hlslpp::float2(static_cast<float>(base.x) + static_cast<float>(base.x) / length * kScatter * eased,
                                  static_cast<float>(base.y) + static_cast<float>(base.y) / length * kScatter * eased);
        };
        hlslpp::float4 faded = kDim;
        faded.w              = static_cast<float>(kDim.w) * (1.0f - t);
        for(const FigureLine& line : m_lines) {
            const hlslpp::float2 position = away(line.base);
            Move(registry, line.entity, position.x, position.y, 1.0f);
            registry.GetComponent<SpriteComponent>(line.entity).tintColor = faded;
        }
        for(const FigureDot& dot : m_dots) {
            const hlslpp::float2 position = away(dot.base);
            Move(registry, dot.entity, position.x, position.y, 0.8f);
            registry.GetComponent<SpriteComponent>(dot.entity).tintColor = faded;
        }
    }

    //-------------------------------------------------------------
    // 後ろの光と星屑を動かす
    //-------------------------------------------------------------
    void StarGateFxSystem::AnimateBackdrop(Tsukino::ECS::Registry& registry, bool gold, float deltaTime) {
        const StarChanceConfigComponent* config = FindSingleton<StarChanceConfigComponent>(registry);
        if(!config)
            return;
        m_time += deltaTime;

        // 金色は大きく速く脈打つ
        const float pulse = 1.0f + (gold ? 0.22f : 0.06f) * std::sin(m_time * (gold ? 8.0f : 3.0f));
        Move(registry, m_glow, config->rollFigureX, config->rollFigureY, pulse);
        hlslpp::float4 glow                                      = gold ? kGold : kBlue;
        glow.w                                                   = gold ? 0.5f : 0.3f;
        registry.GetComponent<SpriteComponent>(m_glow).tintColor = glow;

        TransformComponent& nebula = registry.GetComponent<TransformComponent>(m_nebula);
        nebula.rotation            = hlslpp::quaternion::rotation_z(m_time * 0.25f);
        nebula.dirty               = true;

        for(Dust& dust : m_dust) {
            dust.position.y += dust.velocity.y * deltaTime;
            dust.position.x += dust.velocity.x * deltaTime;
            if(static_cast<float>(dust.position.y) > 1020.0f)
                dust.position.y = -20.0f;
            Move(registry, dust.entity, dust.position.x, dust.position.y, 1.0f);
            registry.GetComponent<SpriteComponent>(dust.entity).tintColor = gold ? kGold : hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f);
        }
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

        const hlslpp::float4& color = state->rollGold ? kGold : kBlue;
        for(const StarGateRollEvent& event : events) {
            if(event.stage == StarGateRollStage::Start) {
                Clear(registry);
                Build(registry);
            } else if(m_root != entt::null && (event.stage == StarGateRollStage::Win || event.stage == StarGateRollStage::Miss)) {
                const bool win = event.stage == StarGateRollStage::Win;
                m_result       = win ? 1 : 2;
                m_resultTime   = 0.0f;
                HudUi::SetText(registry, m_caption, win ? "星ゲットチャンス！" : "星は応えなかった");
                if(win) {
                    // 復活のときは散った線を組み直す
                    ShowConstellation(registry, state->rollConstellationId);
                    ApplyLit(registry, static_cast<int>(m_dots.size()), kGold);
                    HudUi::SetTextColor(registry, m_name, kGold);
                    HudUi::PlayTo(registry,
                                  HudUi::FindAnyPart(registry, "Flash"),
                                  Tsukino::BuiltIn::ECS::UITweenProperty::Alpha,
                                  0.55f,
                                  0.0f,
                                  0.7f,
                                  Tsukino::Core::Math::Easing::EaseType::OutQuad,
                                  HudUi::Tag::kFlash);
                }
            }
        }
        if(m_root == entt::null)
            return;

        // 星図や設定を開いている間は隠す。抽選の時間も止まっている
        const bool paused                                                                   = IsPlayPaused(registry);
        registry.GetComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(m_root).visible = !paused;
        if(paused)
            return;
        AnimateBackdrop(registry, state->rollGold, deltaTime);

        //-------------------------------------------------------------
        // 抽選中: 段階ごとに見せ方を変える
        //-------------------------------------------------------------
        if(m_result == 0) {
            const Roll::Timeline timeline{config->rollSpinSeconds, config->rollHangSeconds, config->rollFakeSeconds};
            const Roll::Scenario scenario = static_cast<Roll::Scenario>(state->rollScenario);
            const float          elapsed  = state->rollTotal - state->rollTimer;
            const Roll::Phase    phase    = Roll::PhaseAt(timeline, scenario, elapsed);

            if(phase == Roll::Phase::Spin) {
                const int step = Roll::ReelStep(elapsed / std::max(timeline.spin, 0.01f), config->rollSteps);
                if(step != m_step) {
                    m_step                      = step;
                    const auto&  constellations = Data::GetConstellationTable().constellations;
                    const size_t pick           = std::uniform_int_distribution<size_t>(0, constellations.size() - 1)(m_random);
                    ShowConstellation(registry, constellations[pick].id);
                    ApplyLit(registry, static_cast<int>(m_dots.size()), color);
                    HudUi::SetTextColor(registry, m_name, color);
                    m_eventBus->Publish(StarGateRollEvent{StarGateRollStage::Tick});
                }
            } else if(phase == Roll::Phase::Hang) {
                if(m_phase != static_cast<int>(phase)) {
                    m_phase = static_cast<int>(phase);
                    m_lit   = -1;
                    ShowConstellation(registry, state->rollConstellationId);
                    Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, m_figure, 0.12f, 0.3f);
                    m_eventBus->Publish(StarGateRollEvent{StarGateRollStage::Hang});
                }
                const int lit = Roll::LitStars((elapsed - timeline.spin) / std::max(timeline.hang, 0.01f), static_cast<int>(m_dots.size()), scenario);
                if(lit != m_lit) {
                    m_lit = lit;
                    ApplyLit(registry, lit, color);
                    if(lit > 0)
                        m_eventBus->Publish(StarGateRollEvent{StarGateRollStage::Tick});
                }
            } else if(phase == Roll::Phase::Fake) {
                if(m_phase != static_cast<int>(phase)) {
                    m_phase = static_cast<int>(phase);
                    HudUi::SetText(registry, m_caption, "星は応えなかった");
                    m_eventBus->Publish(StarGateRollEvent{StarGateRollStage::Fake});
                }
                Scatter(registry, std::clamp((elapsed - timeline.spin - timeline.hang) / std::max(timeline.fake, 0.01f), 0.0f, 1.0f) * 0.6f);
            }
            return;
        }

        //-------------------------------------------------------------
        // 結果: 当たりは星座が大きく広がり、外れは線がほどけて散る
        //-------------------------------------------------------------
        m_resultTime  += deltaTime;
        const float t  = std::clamp(m_resultTime / kResultSeconds, 0.0f, 1.0f);
        if(m_result == 1)
            Move(registry, m_figure, config->rollFigureX, config->rollFigureY, 1.0f + 0.3f * (1.0f - (1.0f - t) * (1.0f - t)));
        else
            Scatter(registry, t);
        registry.GetComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_root).alpha = t < 0.6f ? 1.0f : (1.0f - t) / 0.4f;
        if(t >= 1.0f)
            Clear(registry);
    }

}    // namespace StardustFever::ECS

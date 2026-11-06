//-------------------------------------------------------------
//! @file   StarGetFxSystem.cpp
//! @brief  StarGetFxSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarGetFxSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/StarGetFxConfigComponent.hpp>
#include <StardustFever/ECS/Event/HudMessageEvent.hpp>
#include <StardustFever/ECS/Event/StarGetFxEvent.hpp>
#include <StardustFever/ECS/Utility/CollectionText.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/GamePause.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/StarColor.hpp>
#include <StardustFever/ECS/Utility/StarMapGeometry.hpp>
#include <StardustFever/ECS/Utility/UiParts.hpp>

#include <Tsukino/BuiltIn/ECS/Component/UICanvasGroupComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::SpriteBlendMode;
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        constexpr float kPi = 3.14159265f;

        //-------------------------------------------------------------
        //! @brief  行き過ぎて戻る動きの割合（弾んで現れる）
        //-------------------------------------------------------------
        float EaseOutBack(float t) {
            const float c = 1.9f;
            const float u = t - 1.0f;
            return 1.0f + (c + 1.0f) * u * u * u + c * u * u;
        }

        //-------------------------------------------------------------
        //! @brief  位置と大きさと回転を書く
        //-------------------------------------------------------------
        void Place(Tsukino::ECS::Registry& registry, entt::entity entity, float x, float y, float scale, float angle = 0.0f) {
            TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
            transform.position            = hlslpp::float3(x, y, 0.0f);
            transform.scale               = hlslpp::float3(scale, scale, 1.0f);
            transform.rotation            = hlslpp::quaternion::rotation_z(angle);
            transform.dirty               = true;
        }

        //-------------------------------------------------------------
        //! @brief  色の濃さを書く
        //-------------------------------------------------------------
        void SetAlpha(Tsukino::ECS::Registry& registry, entt::entity entity, float alpha) {
            SpriteComponent& sprite = registry.GetComponent<SpriteComponent>(entity);
            sprite.tintColor.w      = alpha;
        }
    }    // namespace

    StarGetFxSystem::StarGetFxSystem(Tsukino::ECS::Scene& scene, Tsukino::ECS::EventBus& eventBus)
        : m_scene(&scene)
        , m_eventBus(&eventBus)
        , m_draws(eventBus)
        , m_color(1.0f, 1.0f, 1.0f, 1.0f)
        , m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 演出を進める
    //-------------------------------------------------------------
    void StarGetFxSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        for(StarDrawnEvent& drawn : m_draws.Drain()) {
            m_queue.push_back(std::move(drawn));
        }

        //-------------------------------------------------------------
        // 星図や設定を開いたら、見せずに届けて通知だけ残す
        //-------------------------------------------------------------
        if(IsPlayPaused(registry)) {
            if(m_active) {
                Clear(registry);
                Arrive(registry, m_current, false);
            }
            while(!m_queue.empty()) {
                Arrive(registry, m_queue.front(), false);
                m_queue.pop_front();
            }
            return;
        }

        if(!m_active && !m_queue.empty())
            Begin(registry);
        if(!m_active)
            return;

        const StarGetFxConfigComponent* config = FindSingleton<StarGetFxConfigComponent>(registry);
        if(!config)
            return;
        m_time += deltaTime;
        if(!m_flying && m_time >= config->appearSeconds + config->holdSeconds) {
            m_flying = true;
            m_eventBus->Publish(StarGetFxEvent{false});
        }
        if(m_time >= config->appearSeconds + config->holdSeconds + config->flySeconds) {
            Clear(registry);
            Arrive(registry, m_current, true);
            return;
        }
        Animate(registry);
    }

    //-------------------------------------------------------------
    // 次の星の演出を始める
    //-------------------------------------------------------------
    void StarGetFxSystem::Begin(Tsukino::ECS::Registry& registry) {
        const StarGetFxConfigComponent*            config  = FindSingleton<StarGetFxConfigComponent>(registry);
        const SettingsConfigComponent*             style   = FindSingleton<SettingsConfigComponent>(registry);
        const HudLayoutConfigComponent*            layout  = FindSingleton<HudLayoutConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        const Data::StarDef*                       star    = Data::GetStarTable().Find(m_queue.front().starId);
        if(!config || !style || !context || !context->assetManager || !star) {
            Arrive(registry, m_queue.front(), false);
            m_queue.pop_front();
            return;
        }
        m_current = m_queue.front();
        m_queue.pop_front();
        m_active = true;
        m_flying = false;
        m_time   = 0.0f;

        // 等級 -1.5 を最も明るい 1、等級 5 を 0 とする
        m_power = std::clamp(static_cast<float>((5.0 - star->magnitude) / 6.5), 0.0f, 1.0f);
        m_color = SpectralColor(layout, star->spectralType);
        // 星屑に換わった星は星屑のカードへ、それ以外は星図のボタンへ飛ぶ
        m_targetX = m_current.converted ? config->walletX : style->actionBarX + style->actionButtonWidth + style->actionButtonGap;
        m_targetY = m_current.converted ? config->walletY : style->actionBarY;

        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const auto                    glow   = assets.Load(Tsukino::Core::Path(config->glowTexture));
        const auto                    image  = assets.Load(Tsukino::Core::Path(config->starTexture));
        const int                     order  = config->sortOrder;
        const hlslpp::float4          tint   = m_color;

        m_root = UiParts::MakeNode(*m_scene, registry, entt::null, 0.0f, 0.0f);

        m_flash = UiParts::MakeNode(*m_scene, registry, m_root, 800.0f, 500.0f);
        UiParts::AddImage(
            registry, m_flash, assets.Load(Tsukino::Core::Path(config->whiteTexture)), 1600.0f, 1000.0f, order, hlslpp::float4(1.0f, 1.0f, 1.0f, 0.0f));

        const float flareSize = config->flareSize * (0.6f + 0.4f * m_power);
        m_flare               = UiParts::MakeNode(*m_scene, registry, m_root, config->centerX, config->centerY);
        UiParts::AddImage(registry, m_flare, assets.Load(Tsukino::Core::Path(config->flareTexture)), flareSize, flareSize, order + 1, tint).blendMode =
            SpriteBlendMode::Additive;

        m_glow = UiParts::MakeNode(*m_scene, registry, m_root, config->centerX, config->centerY);
        UiParts::AddImage(registry, m_glow, glow, config->starSize * 2.6f, config->starSize * 2.6f, order + 2, tint).blendMode = SpriteBlendMode::Additive;

        //-------------------------------------------------------------
        // 星屑と尾の粒。動きは時刻から計算するので、作るだけでよい
        //-------------------------------------------------------------
        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        const int                             count = config->sparkMin + static_cast<int>(static_cast<float>(config->sparkMax - config->sparkMin) * m_power);
        for(int i = 0; i < count; ++i) {
            Spark spark;
            spark.entity     = UiParts::MakeNode(*m_scene, registry, m_root, config->centerX, config->centerY);
            spark.angle      = unit(m_random) * kPi * 2.0f;
            spark.reach      = 0.45f + unit(m_random) * 0.55f;
            const float size = 14.0f + unit(m_random) * 22.0f;
            UiParts::AddImage(registry, spark.entity, image, size, size, order + 3, hlslpp::float4(1.0f, 0.92f, 0.7f, 0.0f));
            m_sparks.push_back(spark);
        }
        for(int i = 0; i < config->trailCount; ++i) {
            const entt::entity dot = UiParts::MakeNode(*m_scene, registry, m_root, config->centerX, config->centerY);
            UiParts::AddImage(registry, dot, glow, 46.0f, 46.0f, order + 3, tint).blendMode = SpriteBlendMode::Additive;
            SetAlpha(registry, dot, 0.0f);
            m_trail.push_back(dot);
        }

        m_star = UiParts::MakeNode(*m_scene, registry, m_root, config->centerX, config->centerY);
        UiParts::AddImage(registry, m_star, image, config->starSize, config->starSize, order + 4, hlslpp::float4(1.0f, 0.95f, 0.75f, 1.0f));

        //-------------------------------------------------------------
        // 文字。まとめて消せるよう、1つの親に付ける
        //-------------------------------------------------------------
        m_text = UiParts::MakeNode(*m_scene, registry, m_root, config->centerX, config->centerY);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_text);

        std::string tag = "新しい星！";
        if(m_current.converted)
            tag = "★最大  星屑に換えた";
        else if(!m_current.isNew)
            tag = "★" + std::to_string(m_current.level) + " に成長！";
        UiParts::AddLabel(registry,
                          UiParts::MakeNode(*m_scene, registry, m_text, 0.0f, -config->starSize * 0.5f - 44.0f),
                          tag,
                          config->tagScale,
                          order + 5,
                          config->tagColor,
                          true);
        UiParts::AddLabel(
            registry, UiParts::MakeNode(*m_scene, registry, m_text, 0.0f, config->textY), star->name, config->nameScale, order + 5, config->textColor, true);
        UiParts::AddLabel(registry,
                          UiParts::MakeNode(*m_scene, registry, m_text, 0.0f, config->textY + 56.0f),
                          CollectionText::StarFacts(*star),
                          config->factsScale,
                          order + 5,
                          config->textColor,
                          true);

        BuildMapCard(registry, *star);
    }

    //-------------------------------------------------------------
    // 星座の形の中で、手に入れた星の位置を見せるカードを作る
    //-------------------------------------------------------------
    void StarGetFxSystem::BuildMapCard(Tsukino::ECS::Registry& registry, const Data::StarDef& star) {
        const StarGetFxConfigComponent*            config     = FindSingleton<StarGetFxConfigComponent>(registry);
        const SettingsConfigComponent*             style      = FindSingleton<SettingsConfigComponent>(registry);
        const HudLayoutConfigComponent*            layout     = FindSingleton<HudLayoutConfigComponent>(registry);
        const StarCollectionComponent*             collection = FindSingleton<StarCollectionComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context    = GetEngineContext(registry);
        if(!config || !style || !collection || !context || !context->assetManager)
            return;

        //-------------------------------------------------------------
        // つないだ星座を見せる。無ければ星が線に入っている星座を探す（線は境界の別の星座に使われることがある）
        //-------------------------------------------------------------
        const Data::ConstellationTable& table    = Data::GetConstellationTable();
        const auto                      contains = [&](const Data::ConstellationDef& constellation) {
            return std::any_of(
                constellation.stars.begin(), constellation.stars.end(), [&](const Data::ConstellationStarDef& member) { return member.starId == star.id; });
        };
        const Data::ConstellationDef* home = table.Find(m_current.constellationId.empty() ? star.constellationId : m_current.constellationId);
        if(!home || !contains(*home)) {
            home = nullptr;
            for(const Data::ConstellationDef& constellation : table.constellations) {
                if(contains(constellation))
                    home = &constellation;
            }
        }
        if(!home)
            return;

        //-------------------------------------------------------------
        // 星座の中心から見た向きで星を写し、カードの枠に収まる倍率を求める
        //-------------------------------------------------------------
        const Data::StarTable& stars = Data::GetStarTable();
        double                 ra    = 0.0;
        double                 dec   = 0.0;
        if(!StarMapGeometry::Centroid(*home, stars, ra, dec))
            return;

        std::vector<Core::SkyMath::Point> points(home->stars.size());
        std::vector<const Data::StarDef*> defs(home->stars.size(), nullptr);
        double                            minX = 1e9;
        double                            maxX = -1e9;
        double                            minY = 1e9;
        double                            maxY = -1e9;
        for(size_t i = 0; i < home->stars.size(); ++i) {
            const Data::StarDef* member = stars.Find(home->stars[i].starId);
            if(!member || !Core::SkyMath::ProjectToScreen(ra, dec, 90.0, 1.0, member->ra * 15.0, member->dec, points[i]))
                continue;
            defs[i] = member;
            minX    = std::min(minX, points[i].x);
            maxX    = std::max(maxX, points[i].x);
            minY    = std::min(minY, points[i].y);
            maxY    = std::max(maxY, points[i].y);
        }
        const double areaW   = config->mapCardWidth - 90.0;
        const double areaH   = config->mapCardHeight - 150.0;
        const double scale   = std::min({areaW / std::max(maxX - minX, 1e-3), areaH / std::max(maxY - minY, 1e-3), 900.0});
        const double middleX = (minX + maxX) * 0.5;
        const double middleY = (minY + maxY) * 0.5;
        const auto   mapX    = [&](size_t i) { return static_cast<float>((points[i].x - middleX) * scale); };
        const auto   mapY    = [&](size_t i) { return static_cast<float>((points[i].y - middleY) * scale) + 34.0f; };

        Tsukino::Asset::AssetManager& assets = *context->assetManager;
        const auto                    white  = assets.Load(Tsukino::Core::Path(config->whiteTexture));
        const auto                    glow   = assets.Load(Tsukino::Core::Path(config->glowTexture));
        const auto                    image  = assets.Load(Tsukino::Core::Path(config->starTexture));
        const int                     order  = config->sortOrder + 1;

        m_map = UiParts::MakeNode(*m_scene, registry, m_root, config->mapCardX, config->mapCardY);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_map).alpha = 0.0f;
        // カードの面は半透明なので、釘が星と紛らわしくならないよう重ねて濃くする
        for(int layer = 0; layer < 3; ++layer) {
            UiParts::SetSlice(UiParts::AddImage(registry,
                                                UiParts::MakeNode(*m_scene, registry, m_map, 0.0f, 0.0f),
                                                assets.Load(Tsukino::Core::Path(style->panelTexture)),
                                                config->mapCardWidth,
                                                config->mapCardHeight,
                                                order,
                                                style->panelColor),
                              UiParts::kCardBorder);
        }
        UiParts::AddCard(*m_scene, registry, assets, *style, m_map, config->mapCardWidth, config->mapCardHeight, order + 1);

        //-------------------------------------------------------------
        // 題: 星座の名前と、持っている星の数
        //-------------------------------------------------------------
        const int   owned = static_cast<int>(std::count_if(
            home->stars.begin(), home->stars.end(), [&](const Data::ConstellationStarDef& member) { return collection->starLevels.contains(member.starId); }));
        const float top   = -config->mapCardHeight * 0.5f;
        UiParts::AddLabel(
            registry, UiParts::MakeNode(*m_scene, registry, m_map, 0.0f, top + 38.0f), home->name, config->tagScale, order + 6, config->tagColor, true);
        UiParts::AddLabel(registry,
                          UiParts::MakeNode(*m_scene, registry, m_map, 0.0f, top + 72.0f),
                          "星 " + std::to_string(owned) + " / " + std::to_string(home->stars.size()),
                          config->factsScale * 0.85f,
                          order + 6,
                          config->textColor,
                          true);

        //-------------------------------------------------------------
        // 線
        //-------------------------------------------------------------
        const int count = static_cast<int>(home->stars.size());
        for(const Data::ConstellationLineDef& line : home->lines) {
            if(line.from < 0 || line.from >= count || line.to < 0 || line.to >= count || !defs[static_cast<size_t>(line.from)]
               || !defs[static_cast<size_t>(line.to)])
                continue;
            const float        x0     = mapX(static_cast<size_t>(line.from));
            const float        y0     = mapY(static_cast<size_t>(line.from));
            const float        x1     = mapX(static_cast<size_t>(line.to));
            const float        y1     = mapY(static_cast<size_t>(line.to));
            const entt::entity entity = UiParts::MakeNode(*m_scene, registry, m_map, (x0 + x1) * 0.5f, (y0 + y1) * 0.5f);
            UiParts::AddImage(registry, entity, white, std::hypot(x1 - x0, y1 - y0), 2.0f, order + 2, config->mapLineColor);
            registry.GetComponent<TransformComponent>(entity).rotation = hlslpp::quaternion::rotation_z(std::atan2(y1 - y0, x1 - x0));
        }

        //-------------------------------------------------------------
        // 星: 持っている星は色付き、無い星は淡い点。手に入れた星は脈打つ光で示す
        //-------------------------------------------------------------
        for(size_t i = 0; i < defs.size(); ++i) {
            if(!defs[i])
                continue;
            const bool   isTarget = defs[i]->id == star.id;
            const bool   has      = collection->starLevels.contains(defs[i]->id);
            const float  size     = 12.0f + 12.0f * std::clamp(static_cast<float>((5.0 - defs[i]->magnitude) / 5.0), 0.0f, 1.0f);
            entt::entity dot      = UiParts::MakeNode(*m_scene, registry, m_map, mapX(i), mapY(i));
            if(isTarget) {
                m_mapTarget = UiParts::MakeNode(*m_scene, registry, m_map, mapX(i), mapY(i));
                UiParts::AddImage(registry, m_mapTarget, glow, 84.0f, 84.0f, order + 3, config->tagColor).blendMode = SpriteBlendMode::Additive;
                UiParts::AddImage(registry, dot, image, 30.0f, 30.0f, order + 5, hlslpp::float4(1.0f, 0.95f, 0.75f, 1.0f));
                UiParts::AddLabel(registry,
                                  UiParts::MakeNode(*m_scene, registry, m_map, mapX(i), mapY(i) + 30.0f),
                                  star.name,
                                  config->factsScale * 0.8f,
                                  order + 6,
                                  config->textColor,
                                  true);
            } else {
                UiParts::AddImage(registry, dot, glow, size, size, order + 4, has ? SpectralColor(layout, defs[i]->spectralType) : config->mapDimColor)
                    .blendMode = SpriteBlendMode::Additive;
            }
        }
    }

    //-------------------------------------------------------------
    // 今の時刻に合わせて部品を動かす
    //-------------------------------------------------------------
    void StarGetFxSystem::Animate(Tsukino::ECS::Registry& registry) {
        const StarGetFxConfigComponent* config = FindSingleton<StarGetFxConfigComponent>(registry);
        if(!config)
            return;

        const float holdEnd = config->appearSeconds + config->holdSeconds;
        const float appear  = std::clamp(m_time / std::max(config->appearSeconds, 0.01f), 0.0f, 1.0f);
        const float fly     = std::clamp((m_time - holdEnd) / std::max(config->flySeconds, 0.01f), 0.0f, 1.0f);
        const float x0      = config->centerX;
        const float y0      = config->centerY;

        // 弧を描く曲線。飛ぶ先の真上を通るように曲げる
        const auto curve = [&](float t, float& x, float& y) {
            const float cx = m_targetX + (x0 - m_targetX) * 0.25f;
            const float cy = y0 + (m_targetY - y0) * 0.1f;
            const float a  = (1.0f - t) * (1.0f - t);
            const float b  = 2.0f * t * (1.0f - t);
            const float c  = t * t;
            x              = a * x0 + b * cx + c * m_targetX;
            y              = a * y0 + b * cy + c * m_targetY;
        };

        SetAlpha(registry, m_flash, config->flashAlpha * (1.0f - std::clamp(m_time / 0.35f, 0.0f, 1.0f)));

        //-------------------------------------------------------------
        // 星: 弾んで現れ、飛ぶ間に小さくなる。加速するので吸い込まれて見える
        //-------------------------------------------------------------
        const float flyEased = fly * fly;
        float       x        = x0;
        float       y        = y0;
        curve(flyEased, x, y);
        const float breathe = 1.0f + 0.03f * std::sin(m_time * 6.0f);
        const float size    = EaseOutBack(appear) * breathe * (1.0f - 0.86f * flyEased);
        Place(registry, m_star, x, y, size, m_time * 0.6f + flyEased * 6.0f);
        Place(registry, m_glow, x, y, size);
        SetAlpha(registry, m_glow, 0.75f);

        const float fade = 1.0f - std::clamp(fly * 3.0f, 0.0f, 1.0f);
        Place(registry, m_flare, x0, y0, EaseOutBack(appear) * (1.0f + 0.05f * std::sin(m_time * 2.0f)), m_time * 0.35f);
        SetAlpha(registry, m_flare, (0.55f + 0.35f * m_power) * fade);
        registry.GetComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_text).alpha = std::clamp(m_time / 0.25f, 0.0f, 1.0f) * fade;

        //-------------------------------------------------------------
        // 星座のカード: 少し遅れて現れ、手に入れた星の光が脈打つ
        //-------------------------------------------------------------
        if(m_map != entt::null) {
            registry.GetComponent<Tsukino::BuiltIn::ECS::UICanvasGroupComponent>(m_map).alpha = std::clamp((m_time - 0.2f) / 0.3f, 0.0f, 1.0f) * fade;
            if(m_mapTarget != entt::null) {
                const float pulse                                            = 0.8f + 0.35f * std::sin(m_time * 7.0f);
                registry.GetComponent<TransformComponent>(m_mapTarget).scale = hlslpp::float3(pulse, pulse, 1.0f);
                registry.GetComponent<TransformComponent>(m_mapTarget).dirty = true;
            }
        }

        //-------------------------------------------------------------
        // 星屑: 外へ飛び散って消える
        //-------------------------------------------------------------
        const float burst = std::clamp(m_time / 1.0f, 0.0f, 1.0f);
        const float out   = 1.0f - (1.0f - burst) * (1.0f - burst);
        for(const Spark& spark : m_sparks) {
            const float distance = config->sparkDistance * spark.reach * out;
            Place(registry, spark.entity, x0 + std::cos(spark.angle) * distance, y0 + std::sin(spark.angle) * distance, 1.0f, m_time * 3.0f);
            SetAlpha(registry, spark.entity, 1.0f - burst);
        }

        //-------------------------------------------------------------
        // 尾の粒: 少し前の位置をたどり、後ろほど薄くする
        //-------------------------------------------------------------
        for(size_t i = 0; i < m_trail.size(); ++i) {
            const float lag = fly - 0.035f * static_cast<float>(i + 1);
            if(!m_flying || lag <= 0.0f) {
                SetAlpha(registry, m_trail[i], 0.0f);
                continue;
            }
            float tx = 0.0f;
            float ty = 0.0f;
            curve(lag * lag, tx, ty);
            const float life = 1.0f - static_cast<float>(i) / static_cast<float>(m_trail.size());
            Place(registry, m_trail[i], tx, ty, 0.5f + 0.5f * life);
            SetAlpha(registry, m_trail[i], 0.8f * life);
        }
    }

    //-------------------------------------------------------------
    // 星が届いた処理
    //-------------------------------------------------------------
    void StarGetFxSystem::Arrive(Tsukino::ECS::Registry& registry, const StarDrawnEvent& drawn, bool animated) {
        if(!drawn.converted) {
            if(StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry)) {
                std::erase(collection->unseenStars, drawn.starId);
                collection->unseenStars.push_back(drawn.starId);
            }
        }
        if(animated) {
            m_eventBus->Publish(StarGetFxEvent{true});
            if(!drawn.converted) {
                for(const auto entity : registry.View<UiActionComponent>()) {
                    if(registry.GetComponent<UiActionComponent>(entity).action == UiAction::OpenMap)
                        Tsukino::BuiltIn::ECS::UITweenUtility::Punch(registry, entity, 0.3f, 0.4f);
                }
            }
        }
        if(!drawn.followUp.empty())
            m_eventBus->Publish(HudMessageEvent{drawn.followUp, HudMessageImportance::Headline});
    }

    //-------------------------------------------------------------
    // 部品を片付ける
    //-------------------------------------------------------------
    void StarGetFxSystem::Clear(Tsukino::ECS::Registry& registry) {
        if(m_root != entt::null && registry.IsValid(m_root))
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
        m_root      = entt::null;
        m_map       = entt::null;
        m_mapTarget = entt::null;
        m_active    = false;
        m_sparks.clear();
        m_trail.clear();
    }

}    // namespace StardustFever::ECS

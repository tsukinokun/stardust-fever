//-------------------------------------------------------------
//! @file   StarMapViewSystem.cpp
//! @brief  StarMapViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarMapViewSystem.hpp>

#include <StardustFever/Core/TextUtility.hpp>
#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/StarMapConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarMapElementComponent.hpp>
#include <StardustFever/ECS/Component/StarMapStateComponent.hpp>
#include <StardustFever/ECS/Utility/CollectionText.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/StarMapGeometry.hpp>

#include <Tsukino/BuiltIn/ECS/Component/AmbientParticleComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/CameraComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/MaterialPropertyBlockComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/ModelComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/WorldAnchorComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIVisibilityComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>
#include <Tsukino/Renderer/Renderer.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::FontComponent;
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  スペクトル型（の頭文字）から星の色を選ぶ（右の星図パネルと同じ色）
        //-------------------------------------------------------------
        hlslpp::float4 SpectralColor(const HudLayoutConfigComponent* layout, const std::string& spectralType) {
            if(!layout)
                return hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);
            switch(spectralType.empty() ? 'A' : spectralType.front()) {
            case 'O':
                return layout->colorO;
            case 'B':
                return layout->colorB;
            case 'F':
                return layout->colorF;
            case 'G':
                return layout->colorG;
            case 'K':
                return layout->colorK;
            case 'M':
                return layout->colorM;
            default:
                return layout->colorA;
            }
        }

        //-------------------------------------------------------------
        //! @brief  星座の星を全部持っているか
        //-------------------------------------------------------------
        bool IsCollected(const Data::ConstellationDef& constellation, const StarCollectionComponent& collection) {
            return !constellation.stars.empty()
                   && std::all_of(constellation.stars.begin(), constellation.stars.end(), [&](const Data::ConstellationStarDef& member) {
                          return collection.starLevels.contains(member.starId);
                      });
        }

        //-------------------------------------------------------------
        //! @brief  部品のエンティティを作る（位置は親からのずれ）
        //-------------------------------------------------------------
        entt::entity MakeElement(
            Tsukino::ECS::Scene& scene, Tsukino::ECS::Registry& registry, entt::entity parent, StarMapElementKind kind, int index, int subIndex = 0) {
            const entt::entity  entity    = scene.CreateEntity();
            TransformComponent& transform = registry.AddComponent<TransformComponent>(entity);
            if(parent != entt::null)
                transform.parent = parent;
            transform.dirty                  = true;
            StarMapElementComponent& element = registry.AddComponent<StarMapElementComponent>(entity);
            element.kind                     = kind;
            element.index                    = index;
            element.subIndex                 = subIndex;
            return entity;
        }

        //-------------------------------------------------------------
        //! @brief  スプライトを付ける
        //-------------------------------------------------------------
        SpriteComponent& AddSprite(Tsukino::ECS::Registry&            registry,
                                   entt::entity                       entity,
                                   Tsukino::Asset::AssetHandle        texture,
                                   int                                sortOrder,
                                   bool                               additive,
                                   Tsukino::BuiltIn::ECS::SpriteSpace space = Tsukino::BuiltIn::ECS::SpriteSpace::Screen) {
            SpriteComponent& sprite = registry.AddComponent<SpriteComponent>(entity);
            sprite.textureHandle    = texture;
            sprite.space            = space;
            sprite.blendMode        = additive ? Tsukino::BuiltIn::ECS::SpriteBlendMode::Additive : Tsukino::BuiltIn::ECS::SpriteBlendMode::Alpha;
            sprite.raycastTarget    = false;
            sprite.sortOrder        = sortOrder;
            return sprite;
        }

        //-------------------------------------------------------------
        //! @brief  文字を付ける
        //-------------------------------------------------------------
        FontComponent& AddText(Tsukino::ECS::Registry& registry, entt::entity entity, float scale, int sortOrder, bool centered) {
            FontComponent& font  = registry.AddComponent<FontComponent>(entity);
            font.scale           = scale;
            font.sortOrder       = sortOrder;
            font.outlineColor    = hlslpp::float4(0.0f, 0.0f, 0.05f, 0.9f);
            font.outlineWidth    = 1.0f;
            font.horizontalAlign = centered ? Tsukino::BuiltIn::ECS::HorizontalAlign::Center : Tsukino::BuiltIn::ECS::HorizontalAlign::Left;
            font.verticalAlign   = centered ? Tsukino::BuiltIn::ECS::VerticalAlign::Middle : Tsukino::BuiltIn::ECS::VerticalAlign::Top;
            return font;
        }

        //-------------------------------------------------------------
        //! @brief  位置と大きさを書く
        //-------------------------------------------------------------
        void Place(Tsukino::ECS::Registry& registry, entt::entity entity, float x, float y, float scaleX, float scaleY) {
            TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
            transform.position            = hlslpp::float3(x, y, 0.0f);
            transform.scale               = hlslpp::float3(scaleX, scaleY, 1.0f);
            transform.dirty               = true;
        }

        //-------------------------------------------------------------
        //! @brief  天球の中心を求める
        //-------------------------------------------------------------
        hlslpp::float3 SkyCenter(const StarMapConfigComponent& config) {
            return hlslpp::float3(0.0f, 0.0f, config.skyOffsetZ);
        }

        //-------------------------------------------------------------
        //! @brief  向きを天球の上の位置にする
        //-------------------------------------------------------------
        hlslpp::float3 SkyPoint(const StarMapConfigComponent& config, const Core::SkyMath::Direction& direction, float radius) {
            return SkyCenter(config)
                   + hlslpp::float3(static_cast<float>(direction.x), static_cast<float>(direction.y), static_cast<float>(direction.z)) * radius;
        }

        //-------------------------------------------------------------
        //! @brief  主カメラを探す
        //-------------------------------------------------------------
        entt::entity FindMainCamera(Tsukino::ECS::Registry& registry) {
            using Tsukino::BuiltIn::ECS::CameraComponent;
            for(const auto entity : registry.View<CameraComponent, TransformComponent>()) {
                if(registry.GetComponent<CameraComponent>(entity).isPrimary)
                    return entity;
            }
            return entt::null;
        }
    }    // namespace

    StarMapViewSystem::StarMapViewSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 開け閉めの段階を進め、空を見ている間はカメラと色を合わせる
    //-------------------------------------------------------------
    void StarMapViewSystem::Update(Tsukino::ECS::Registry& registry, float deltaTime) {
        const StarMapStateComponent*  state  = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent* config = FindSingleton<StarMapConfigComponent>(registry);
        if(!state || !config)
            return;
        const bool open = state->open;

        //-------------------------------------------------------------
        // 幕を動かす。空を作ったフレームは長いので、1フレームで明けてしまわないよう経過時間を抑える
        //-------------------------------------------------------------
        const float step      = std::min(deltaTime, 1.0f / 30.0f);
        const bool  darkening = m_phase == Phase::Darkening || m_phase == Phase::Hiding;
        if(darkening)
            m_veilAlpha = std::min(1.0f, m_veilAlpha + step / std::max(config->fadeOutSeconds, 0.01f));
        else if(m_phase != Phase::Closed)
            m_veilAlpha = std::max(0.0f, m_veilAlpha - step / std::max(config->revealSeconds, 0.01f));

        switch(m_phase) {
        case Phase::Closed:
            if(open) {
                MakeVeil(registry);
                m_phase = Phase::Darkening;
            }
            break;
        case Phase::Darkening:
            if(!open) {
                m_phase = Phase::Returning;
            } else if(m_veilAlpha >= 1.0f) {
                EnterSky(registry);
                m_phase = Phase::Revealing;
            }
            break;
        case Phase::Revealing:
            if(!open)
                m_phase = Phase::Hiding;
            else if(m_veilAlpha <= 0.0f)
                m_phase = Phase::Shown;
            break;
        case Phase::Shown:
            if(!open)
                m_phase = Phase::Hiding;
            break;
        case Phase::Hiding:
            if(open) {
                m_phase = Phase::Revealing;
            } else if(m_veilAlpha >= 1.0f) {
                ExitSky(registry);
                m_phase = Phase::Returning;
            }
            break;
        case Phase::Returning:
            if(open) {
                m_phase = Phase::Darkening;
            } else if(m_veilAlpha <= 0.0f) {
                if(m_veil != entt::null && registry.IsValid(m_veil))
                    registry.QueueDestroy(m_veil);
                m_veil  = entt::null;
                m_phase = Phase::Closed;
            }
            break;
        }

        if(m_veil != entt::null && registry.IsValid(m_veil)) {
            if(SpriteComponent* sprite = registry.try_get<SpriteComponent>(m_veil))
                sprite->tintColor = hlslpp::float4(0.0f, 0.0f, 0.0f, m_veilAlpha);
        }

        if(m_root == entt::null || !registry.IsValid(m_root))
            return;
        ApplyCamera(registry);
        if(state->version != m_appliedVersion) {
            Layout(registry);
            m_appliedVersion = state->version;
        }
        UpdateTooltip(registry);
    }

    //-------------------------------------------------------------
    // マウスの下の星の吹き出しを出す
    //-------------------------------------------------------------
    void StarMapViewSystem::UpdateTooltip(Tsukino::ECS::Registry& registry) {
        const StarMapStateComponent*   state      = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent*  config     = FindSingleton<StarMapConfigComponent>(registry);
        const StarCollectionComponent* collection = FindSingleton<StarCollectionComponent>(registry);
        if(!state || !config || !collection || m_tooltip == entt::null)
            return;

        const Data::StarTable& stars = Data::GetStarTable();
        const bool             shown = state->hoverStar >= 0 && state->hoverStar < static_cast<int>(stars.stars.size());
        registry.GetComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(m_tooltip).visible = shown;
        if(!shown) {
            m_tooltipStar = -1;
            return;
        }

        //-------------------------------------------------------------
        // マウスの右下に置く。画面からはみ出すなら反対側へ回す
        //-------------------------------------------------------------
        const float gap = 16.0f;
        float       x   = state->hoverX + gap;
        float       y   = state->hoverY + gap;
        if(x + config->tooltipWidth > config->panelX)
            x = state->hoverX - gap - config->tooltipWidth;
        if(y + config->tooltipHeight > 1000.0f)
            y = state->hoverY - gap - config->tooltipHeight;
        Place(registry, m_tooltip, x, y, 1.0f, 1.0f);

        if(state->hoverStar == m_tooltipStar)
            return;
        m_tooltipStar = state->hoverStar;

        const Data::StarDef&          star  = stars.stars[static_cast<size_t>(m_tooltipStar)];
        const Data::ConstellationDef* home  = Data::GetConstellationTable().Find(star.constellationId);
        const auto                    found = collection->starLevels.find(star.id);
        HudUi::SetText(registry, m_tooltipText[0], star.name);
        HudUi::SetText(registry, m_tooltipText[1], CollectionText::StarFacts(star));
        HudUi::SetText(
            registry,
            m_tooltipText[2],
            (home ? home->name + "  " : std::string()) + (found != collection->starLevels.end() ? "★" + std::to_string(found->second) : std::string("未獲得")));
    }

    //-------------------------------------------------------------
    // 暗転の幕を作る
    //-------------------------------------------------------------
    void StarMapViewSystem::MakeVeil(Tsukino::ECS::Registry& registry) {
        const StarMapConfigComponent*              config  = FindSingleton<StarMapConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager || m_veil != entt::null)
            return;
        const Tsukino::Asset::AssetHandle white = context->assetManager->Load(Tsukino::Core::Path(config->backgroundTexture));
        const float                       px    = std::max(config->backgroundPixels, 1.0f);
        m_veil                                  = MakeElement(*m_scene, registry, entt::null, StarMapElementKind::Veil, 0);
        // 文字より手前に置く
        AddSprite(registry, m_veil, white, config->sortOrder + 20, false).tintColor = hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f);
        Place(registry, m_veil, 800.0f, 500.0f, 1600.0f / px, 1000.0f / px);
    }

    //-------------------------------------------------------------
    // 空へ切り替える
    //-------------------------------------------------------------
    void StarMapViewSystem::EnterSky(Tsukino::ECS::Registry& registry) {
        const StarMapConfigComponent*              config  = FindSingleton<StarMapConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context)
            return;

        //-------------------------------------------------------------
        // 画面の文字と絵は 3D の手前に描かれるので、空を見ている間は隠す。幕だけは残す
        //-------------------------------------------------------------
        using Tsukino::BuiltIn::ECS::UIVisibilityComponent;
        std::vector<entt::entity> hud;
        for(const auto entity : registry.View<SpriteComponent>()) {
            if(entity != m_veil && registry.GetComponent<SpriteComponent>(entity).space == Tsukino::BuiltIn::ECS::SpriteSpace::Screen)
                hud.push_back(entity);
        }
        for(const auto entity : registry.View<FontComponent>()) {
            hud.push_back(entity);
        }
        m_hiddenHud.clear();
        for(const entt::entity entity : hud) {
            UIVisibilityComponent& visibility = registry.HasComponent<UIVisibilityComponent>(entity) ? registry.GetComponent<UIVisibilityComponent>(entity)
                                                                                                     : registry.AddComponent<UIVisibilityComponent>(entity);
            if(visibility.visible) {
                visibility.visible = false;
                m_hiddenHud.push_back(entity);
            }
        }

        //-------------------------------------------------------------
        // カメラは1つしか持てないので、盤面の見え方を覚えて、遠く離れた天球の中心へ動かす。
        // 盤面はカメラの遠い面より外へ出るので、隠さなくても描かれない
        //-------------------------------------------------------------
        using Tsukino::BuiltIn::ECS::CameraComponent;
        const entt::entity camera = FindMainCamera(registry);
        if(camera != entt::null) {
            m_boardCameraPosition = registry.GetComponent<TransformComponent>(camera).position;
            m_boardCameraTarget   = registry.GetComponent<CameraComponent>(camera).lookAtTarget;
            m_boardCameraFov      = registry.GetComponent<CameraComponent>(camera).fov;
        }

        // 漂う粒子はカメラの周りに湧くので、空の手前に混ざらないよう止める
        using Tsukino::BuiltIn::ECS::AmbientParticleComponent;
        for(const auto entity : registry.View<AmbientParticleComponent>()) {
            AmbientParticleComponent& particles = registry.GetComponent<AmbientParticleComponent>(entity);
            m_particlesEnabled                  = particles.enabled;
            particles.enabled                   = false;
        }

        // 盤面は背景のモデルが画面を覆うので、地の色は戻さなくてよい
        if(context->renderer) {
            const hlslpp::float4& clear = config->skyClearColor;
            context->renderer->SetClearColor(clear.x, clear.y, clear.z, clear.w);
        }

        Build(registry);
        m_appliedVersion = -1;
    }

    //-------------------------------------------------------------
    // 盤面へ戻す
    //-------------------------------------------------------------
    void StarMapViewSystem::ExitSky(Tsukino::ECS::Registry& registry) {
        if(m_root != entt::null && registry.IsValid(m_root))
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
        m_root           = entt::null;
        m_tooltip        = entt::null;
        m_appliedVersion = -1;

        using Tsukino::BuiltIn::ECS::UIVisibilityComponent;
        for(const entt::entity entity : m_hiddenHud) {
            if(registry.IsValid(entity) && registry.HasComponent<UIVisibilityComponent>(entity))
                registry.GetComponent<UIVisibilityComponent>(entity).visible = true;
        }
        m_hiddenHud.clear();

        using Tsukino::BuiltIn::ECS::CameraComponent;
        const entt::entity camera = FindMainCamera(registry);
        if(camera != entt::null) {
            TransformComponent& transform = registry.GetComponent<TransformComponent>(camera);
            CameraComponent&    component = registry.GetComponent<CameraComponent>(camera);
            transform.position            = m_boardCameraPosition;
            transform.dirty               = true;
            component.lookAtTarget        = m_boardCameraTarget;
            component.fov                 = m_boardCameraFov;
            component.dirty               = true;
        }

        using Tsukino::BuiltIn::ECS::AmbientParticleComponent;
        for(const auto entity : registry.View<AmbientParticleComponent>()) {
            registry.GetComponent<AmbientParticleComponent>(entity).enabled = m_particlesEnabled;
        }
    }

    //-------------------------------------------------------------
    // カメラを今の向きへ合わせる
    //-------------------------------------------------------------
    void StarMapViewSystem::ApplyCamera(Tsukino::ECS::Registry& registry) {
        const StarMapStateComponent*  state  = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent* config = FindSingleton<StarMapConfigComponent>(registry);
        const entt::entity            camera = FindMainCamera(registry);
        if(!state || !config || camera == entt::null)
            return;

        // 開いた直後は少し下を向いていて、幕が明けるのに合わせて空を見上げる
        const double look = static_cast<double>(config->revealLookDegrees) * m_veilAlpha;
        const double dec  = std::clamp(state->decCenter - look, -static_cast<double>(config->maxDec), static_cast<double>(config->maxDec));

        using Tsukino::BuiltIn::ECS::CameraComponent;
        TransformComponent& transform = registry.GetComponent<TransformComponent>(camera);
        CameraComponent&    component = registry.GetComponent<CameraComponent>(camera);
        transform.position            = SkyCenter(*config);
        transform.dirty               = true;
        component.lookAtTarget        = SkyPoint(*config, Core::SkyMath::ToDirection(state->raCenter, dec), 100.0f);
        component.fov                 = config->baseFov / static_cast<float>(std::max(state->zoom, 0.1));
        component.dirty               = true;    // Transform の dirty は先に消えるので、行列を作り直すにはこれが要る
    }

    //-------------------------------------------------------------
    // 画面の部品を全て作る
    //-------------------------------------------------------------
    void StarMapViewSystem::Build(Tsukino::ECS::Registry& registry) {
        const StarMapConfigComponent*              config  = FindSingleton<StarMapConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager)
            return;

        using Tsukino::BuiltIn::ECS::SpriteSpace;
        const Tsukino::Asset::AssetHandle white  = context->assetManager->Load(Tsukino::Core::Path(config->backgroundTexture));
        const Tsukino::Asset::AssetHandle dot    = context->assetManager->Load(Tsukino::Core::Path(config->starTexture));
        const Tsukino::Asset::AssetHandle line   = context->assetManager->Load(Tsukino::Core::Path(config->lineModel));
        const float                       px     = std::max(config->backgroundPixels, 1.0f);
        const float                       starPx = std::max(config->starPixels, 1.0f);
        const int                         order  = config->sortOrder;

        m_root = MakeElement(*m_scene, registry, entt::null, StarMapElementKind::Root, 0);
        Place(registry, m_root, 0.0f, 0.0f, 1.0f, 1.0f);

        //-------------------------------------------------------------
        // 右のパネルの板。空の上に半透明で重ねる
        //-------------------------------------------------------------
        const float        mapCenterY                                 = config->mapY + config->mapHeight * 0.5f;
        const entt::entity panel                                      = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Root, 3);
        AddSprite(registry, panel, white, order + 1, false).tintColor = config->panelColor;
        Place(registry, panel, config->panelX + config->panelWidth * 0.5f, mapCenterY, config->panelWidth / px, config->mapHeight / px);

        //-------------------------------------------------------------
        // 星雲: 星より奥に、向きを黄金角で散らして置く
        //-------------------------------------------------------------
        if(!config->nebulaTextures.empty()) {
            for(int i = 0; i < config->nebulaCount; ++i) {
                const double dec = std::asin(1.0 - 2.0 * (i + 0.5) / config->nebulaCount) * 57.29577951308232;
                const double ra  = std::fmod(i * 137.50776405003785, 360.0);
                const auto   texture =
                    context->assetManager->Load(Tsukino::Core::Path(config->nebulaTextures[static_cast<size_t>(i) % config->nebulaTextures.size()]));
                const entt::entity nebula = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Root, 5 + i);
                AddSprite(registry, nebula, texture, 0, true, SpriteSpace::World).tintColor = config->nebulaColor;
                TransformComponent& transform                                               = registry.GetComponent<TransformComponent>(nebula);
                transform.position = SkyPoint(*config, Core::SkyMath::ToDirection(ra, dec), config->skyRadius * 1.5f);
                transform.scale    = hlslpp::float3(config->nebulaScale, config->nebulaScale, 1.0f);
            }
        }

        //-------------------------------------------------------------
        // 線: 2つの星の間に細い箱を渡す。天球の上の点は動かないので、ここで一度だけ置く
        //-------------------------------------------------------------
        const Data::StarTable&          stars = Data::GetStarTable();
        const Data::ConstellationTable& table = Data::GetConstellationTable();
        for(size_t c = 0; c < table.constellations.size(); ++c) {
            const Data::ConstellationDef& constellation = table.constellations[c];
            const int                     count         = static_cast<int>(constellation.stars.size());
            for(size_t l = 0; l < constellation.lines.size(); ++l) {
                const Data::ConstellationLineDef& def  = constellation.lines[l];
                const Data::StarDef*              from = def.from >= 0 && def.from < count ? stars.Find(constellation.stars[def.from].starId) : nullptr;
                const Data::StarDef*              to   = def.to >= 0 && def.to < count ? stars.Find(constellation.stars[def.to].starId) : nullptr;
                if(!from || !to)
                    continue;
                const hlslpp::float3          a  = SkyPoint(*config, Core::SkyMath::ToDirection(from->ra * 15.0, from->dec), config->skyRadius);
                const hlslpp::float3          b  = SkyPoint(*config, Core::SkyMath::ToDirection(to->ra * 15.0, to->dec), config->skyRadius);
                const hlslpp::float3          d  = b - a;
                const float                   dx = d.x;
                const float                   dy = d.y;
                const float                   dz = d.z;
                const Core::SkyMath::Rotation q  = Core::SkyMath::ArcFromXAxis(Core::SkyMath::Direction{dx, dy, dz});

                const entt::entity  entity    = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Line, static_cast<int>(c), static_cast<int>(l));
                TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                transform.position            = (a + b) * 0.5f;
                transform.rotation = hlslpp::quaternion(static_cast<float>(q.x), static_cast<float>(q.y), static_cast<float>(q.z), static_cast<float>(q.w));
                transform.scale    = hlslpp::float3(std::sqrt(dx * dx + dy * dy + dz * dz) * 0.5f, config->lineThickness, config->lineThickness);
                Tsukino::BuiltIn::ECS::ModelComponent& model = registry.AddComponent<Tsukino::BuiltIn::ECS::ModelComponent>(entity);
                model.modelHandle                            = line;
                model.opacity                                = config->lineOpacity;
                // 照明を反射すると線の途中に光の筋が出るので、基本色を黒にして、発光だけで見せる
                auto& block     = registry.AddComponent<Tsukino::BuiltIn::ECS::MaterialPropertyBlockComponent>(entity);
                block.baseColor = hlslpp::float4(0.0f, 0.0f, 0.0f, 1.0f);
                block.metallic  = 0.0f;
                block.roughness = 1.0f;
            }
        }

        //-------------------------------------------------------------
        // 星: カメラを向く板。大きさは色と一緒に Layout が決める
        //-------------------------------------------------------------
        for(size_t i = 0; i < stars.stars.size(); ++i) {
            const entt::entity star = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Star, static_cast<int>(i));
            AddSprite(registry, star, dot, 1, true, SpriteSpace::World);
            TransformComponent& transform = registry.GetComponent<TransformComponent>(star);
            transform.position            = SkyPoint(*config, Core::SkyMath::ToDirection(stars.stars[i].ra * 15.0, stars.stars[i].dec), config->skyRadius);
        }

        //-------------------------------------------------------------
        // 星座名: 画面の文字を天球の一点に貼り付ける。後ろへ回ると隠れる
        //-------------------------------------------------------------
        for(size_t c = 0; c < table.constellations.size(); ++c) {
            const entt::entity label = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Label, static_cast<int>(c));
            AddText(registry, label, config->labelScale, order, true);
            HudUi::SetText(registry, label, table.constellations[c].name);
            double ra  = 0.0;
            double dec = 0.0;
            StarMapGeometry::Centroid(table.constellations[c], stars, ra, dec);
            Tsukino::BuiltIn::ECS::WorldAnchorComponent& anchor = registry.AddComponent<Tsukino::BuiltIn::ECS::WorldAnchorComponent>(label);
            anchor.useFixedWorldPosition                        = true;
            anchor.fixedWorldPosition                           = SkyPoint(*config, Core::SkyMath::ToDirection(ra, dec), config->skyRadius);
        }

        //-------------------------------------------------------------
        // 選んだ星座の星の名前。枠を使い回し、貼り付ける星は Layout が決める
        //-------------------------------------------------------------
        for(int i = 0; i < config->maxStarLabels; ++i) {
            const entt::entity label                            = MakeElement(*m_scene, registry, m_root, StarMapElementKind::StarLabel, i);
            FontComponent&     font                             = AddText(registry, label, config->starLabelScale, order, true);
            font.horizontalAlign                                = Tsukino::BuiltIn::ECS::HorizontalAlign::Left;
            Tsukino::BuiltIn::ECS::WorldAnchorComponent& anchor = registry.AddComponent<Tsukino::BuiltIn::ECS::WorldAnchorComponent>(label);
            anchor.useFixedWorldPosition                        = true;
            anchor.screenOffset                                 = hlslpp::float2(config->starLabelOffset, 0.0f);
        }

        //-------------------------------------------------------------
        // マウスを乗せた星の吹き出し。板の左上を基準に置く
        //-------------------------------------------------------------
        m_tooltip = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Tooltip, 0);
        Place(registry, m_tooltip, 0.0f, 0.0f, 1.0f, 1.0f);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIVisibilityComponent>(m_tooltip).visible = false;
        const entt::entity tooltipBoard                                      = MakeElement(*m_scene, registry, m_tooltip, StarMapElementKind::Tooltip, 1);
        AddSprite(registry, tooltipBoard, white, order + 6, false).tintColor = config->tooltipColor;
        Place(registry, tooltipBoard, config->tooltipWidth * 0.5f, config->tooltipHeight * 0.5f, config->tooltipWidth / px, config->tooltipHeight / px);
        for(int i = 0; i < 3; ++i) {
            m_tooltipText[i]    = MakeElement(*m_scene, registry, m_tooltip, StarMapElementKind::TooltipText, i);
            FontComponent& font = AddText(registry, m_tooltipText[i], config->tooltipScale * (i == 0 ? 1.25f : 1.0f), order + 7, false);
            font.maxWidth       = config->tooltipWidth - 24.0f;
            Place(registry, m_tooltipText[i], 12.0f, i == 0 ? 8.0f : 12.0f + 24.0f * static_cast<float>(i), 1.0f, 1.0f);
        }
        m_tooltipStar = -1;

        //-------------------------------------------------------------
        // 上のまとめと操作の説明、右のパネルの文字
        //-------------------------------------------------------------
        //-------------------------------------------------------------
        const entt::entity summary = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Summary, 0);
        AddText(registry, summary, config->textScale * 1.25f, order + 5, false);
        Place(registry, summary, config->mapX, 18.0f, 1.0f, 1.0f);
        const entt::entity hint                                           = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Root, 4);
        AddText(registry, hint, config->rowScale, order + 5, false).color = hlslpp::float4(0.6f, 0.65f, 0.8f, 0.9f);
        Place(registry, hint, config->mapX, config->mapY + config->mapHeight + 14.0f, 1.0f, 1.0f);
        HudUi::SetText(
            registry,
            hint,
            "ドラッグ・矢印 見回す   ホイール・Q/E 拡大   クリック 星座を選ぶ   Z/X 前後   G グループの一覧   Tab・Esc 閉じる（遊びは止まっています）");

        float y         = config->mapY + 18.0f;
        auto  panelText = [&](StarMapElementKind kind, int index, float scale, float spacing) {
            const entt::entity entity = MakeElement(*m_scene, registry, m_root, kind, index);
            AddText(registry, entity, scale, order + 5, false);
            Place(registry, entity, config->panelX + 20.0f, y, 1.0f, 1.0f);
            y += spacing;
        };
        panelText(StarMapElementKind::Title, 0, config->titleScale, 52.0f);
        panelText(StarMapElementKind::Subtitle, 0, config->textScale, 30.0f);
        panelText(StarMapElementKind::Effect, 0, config->textScale, 40.0f);
        for(int row = 0; row < config->maxRows; ++row) {
            panelText(StarMapElementKind::Row, row, config->rowScale, config->rowSpacing);
        }
    }

    //-------------------------------------------------------------
    // 星・線・名前の色を今の状態にし、詳しいパネルの中身を書く
    //-------------------------------------------------------------
    void StarMapViewSystem::Layout(Tsukino::ECS::Registry& registry) {
        StarMapStateComponent*              state      = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent*       config     = FindSingleton<StarMapConfigComponent>(registry);
        const StarCollectionComponent*      collection = FindSingleton<StarCollectionComponent>(registry);
        const CollectionModifiersComponent* modifiers  = FindSingleton<CollectionModifiersComponent>(registry);
        const StarChanceConfigComponent*    chance     = FindSingleton<StarChanceConfigComponent>(registry);
        const HudLayoutConfigComponent*     layout     = FindSingleton<HudLayoutConfigComponent>(registry);
        if(!state || !config || !collection)
            return;

        const Data::StarTable&          stars    = Data::GetStarTable();
        const Data::ConstellationTable& table    = Data::GetConstellationTable();
        const float                     starPx   = std::max(config->starPixels, 1.0f);
        const Data::ConstellationDef*   selected = table.Find(state->selectedId);

        //-------------------------------------------------------------
        // 星座ごとに、そろったか・目立たせるかを先に決めておく。
        // 目立たせるのは、星座の詳しい中身では選んだ星座、グループの一覧では選んだグループの星座
        //-------------------------------------------------------------
        const Data::ConstellationGroupTable& groups = Data::GetConstellationGroupTable();
        const Data::ConstellationGroupDef*   group  = nullptr;
        if(state->groupMode && !groups.groups.empty())
            group = &groups.groups[static_cast<size_t>(std::clamp(state->selectedGroup, 0, static_cast<int>(groups.groups.size()) - 1))];
        std::vector<bool> collected(table.constellations.size());
        std::vector<bool> highlighted(table.constellations.size());
        for(size_t c = 0; c < table.constellations.size(); ++c) {
            const Data::ConstellationDef& constellation = table.constellations[c];
            collected[c]                                = IsCollected(constellation, *collection);
            if(group)
                highlighted[c] = std::find(group->constellationIds.begin(), group->constellationIds.end(), constellation.id) != group->constellationIds.end();
            else
                highlighted[c] = selected == &constellation;
        }

        //-------------------------------------------------------------
        // 選んだ星座の、名前を添える星。固有名のある星を先に、その中は明るい順に選ぶ
        //-------------------------------------------------------------
        std::vector<const Data::StarDef*> labelled;
        if(selected && !group) {
            for(const Data::ConstellationStarDef& member : selected->stars) {
                if(const Data::StarDef* star = stars.Find(member.starId))
                    labelled.push_back(star);
            }
            std::sort(labelled.begin(), labelled.end(), [](const Data::StarDef* a, const Data::StarDef* b) {
                const bool namedA = !a->nameEn.empty();
                const bool namedB = !b->nameEn.empty();
                return namedA != namedB ? namedA : a->magnitude < b->magnitude;
            });
        }

        for(const auto entity : registry.View<StarMapElementComponent>()) {
            const StarMapElementComponent& element = registry.GetComponent<StarMapElementComponent>(entity);
            switch(element.kind) {
            //-------------------------------------------------------------
            // 星の名前: 呼び名の頭の星座名は省いて短くする（「オリオン座π4」→「π4」）
            //-------------------------------------------------------------
            case StarMapElementKind::StarLabel:
                {
                    if(element.index >= static_cast<int>(labelled.size())) {
                        HudUi::SetTextColor(registry, entity, hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f));
                        if(FontComponent* font = registry.try_get<FontComponent>(entity))
                            font->outlineWidth = 0.0f;
                        break;
                    }
                    const Data::StarDef& star = *labelled[static_cast<size_t>(element.index)];
                    std::string          name = star.name;
                    if(name.rfind(selected->name, 0) == 0 && name.size() > selected->name.size())
                        name = name.substr(selected->name.size());
                    HudUi::SetText(registry, entity, name);
                    HudUi::SetTextColor(registry, entity, config->starLabelColor);
                    if(FontComponent* font = registry.try_get<FontComponent>(entity))
                        font->outlineWidth = 1.0f;
                    if(auto* anchor = registry.try_get<Tsukino::BuiltIn::ECS::WorldAnchorComponent>(entity))
                        anchor->fixedWorldPosition = SkyPoint(*config, Core::SkyMath::ToDirection(star.ra * 15.0, star.dec), config->skyRadius);
                    break;
                }
            //-------------------------------------------------------------
            // 星: 明るいほど大きく。持っていればスペクトル型の色で強く光り、無ければ淡い点
            //-------------------------------------------------------------
            case StarMapElementKind::Star:
                {
                    if(element.index < 0 || element.index >= static_cast<int>(stars.stars.size()))
                        break;
                    const Data::StarDef& star   = stars.stars[element.index];
                    const float          bright = std::clamp(static_cast<float>((5.0 - star.magnitude) / 5.0), 0.0f, 1.3f);
                    const bool           owned  = collection->starLevels.contains(star.id);
                    float                size   = config->faintStarSize + (config->brightStarSize - config->faintStarSize) * bright;
                    if(!owned)
                        size *= 0.7f;
                    const float         scale     = size * config->worldUnitsPerPixel / starPx;
                    TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
                    transform.scale               = hlslpp::float3(scale, scale, 1.0f);
                    transform.dirty               = true;
                    if(SpriteComponent* sprite = registry.try_get<SpriteComponent>(entity)) {
                        const float glow = config->ownedStarGlow;
                        sprite->tintColor =
                            owned ? SpectralColor(layout, star.spectralType) * hlslpp::float4(glow, glow, glow, 1.0f) : config->missingStarColor;
                    }
                    break;
                }
            //-------------------------------------------------------------
            // 線: そろった星座は金色で太く、選んだ星座は明るく、それ以外は淡く
            //-------------------------------------------------------------
            case StarMapElementKind::Line:
                {
                    if(element.index < 0 || element.index >= static_cast<int>(table.constellations.size()))
                        break;
                    const bool            isDone    = collected[element.index];
                    const bool            isSel     = highlighted[element.index];
                    const float           thickness = (isDone || isSel) ? config->collectedLineThickness : config->lineThickness;
                    const hlslpp::float4& color     = isDone ? config->collectedLineColor : (isSel ? config->selectedLineColor : config->lineColor);
                    const float           power     = static_cast<float>(color.w) * config->lineGlow;
                    TransformComponent&   transform = registry.GetComponent<TransformComponent>(entity);
                    transform.scale                 = hlslpp::float3(static_cast<float>(transform.scale.x), thickness, thickness);
                    transform.dirty                 = true;
                    if(auto* block = registry.try_get<Tsukino::BuiltIn::ECS::MaterialPropertyBlockComponent>(entity))
                        block->emissive =
                            hlslpp::float3(static_cast<float>(color.x) * power, static_cast<float>(color.y) * power, static_cast<float>(color.z) * power);
                    break;
                }
            //-------------------------------------------------------------
            // 星座名: 縮小しているときは、そろった星座と選んだ星座だけ出す
            //-------------------------------------------------------------
            case StarMapElementKind::Label:
                {
                    if(element.index < 0 || element.index >= static_cast<int>(table.constellations.size()))
                        break;
                    const bool isDone = collected[element.index];
                    const bool isSel  = highlighted[element.index];
                    const bool shown  = isDone || isSel || state->zoom >= config->labelMinZoom;
                    HudUi::SetTextColor(registry,
                                        entity,
                                        !shown ? hlslpp::float4(0.0f, 0.0f, 0.0f, 0.0f)
                                               : (isSel ? config->selectedLabelColor : (isDone ? config->collectedLabelColor : config->labelColor)));
                    if(FontComponent* font = registry.try_get<FontComponent>(entity)) {
                        font->scale        = config->labelScale * (isSel ? 1.3f : 1.0f);
                        font->outlineWidth = shown ? 1.0f : 0.0f;
                    }
                    break;
                }
            default:
                break;
            }
        }

        //-------------------------------------------------------------
        // 上のまとめ
        //-------------------------------------------------------------
        const auto findText = [&](StarMapElementKind kind, int index) {
            for(const auto entity : registry.View<StarMapElementComponent>()) {
                const StarMapElementComponent& element = registry.GetComponent<StarMapElementComponent>(entity);
                if(element.kind == kind && element.index == index)
                    return entity;
            }
            return entt::entity(entt::null);
        };
        if(modifiers) {
            char buffer[128];
            std::snprintf(buffer,
                          sizeof(buffer),
                          "88星座の星図   そろった星座 %d / %d   集めた星 %d / %d   そろったグループ %d / %d",
                          modifiers->collectedConstellations,
                          modifiers->totalConstellations,
                          modifiers->ownedStars,
                          modifiers->totalStars,
                          modifiers->collectedGroups,
                          modifiers->totalGroups);
            HudUi::SetText(registry, findText(StarMapElementKind::Summary, 0), buffer);
        }

        //-------------------------------------------------------------
        // 右のパネル: 選んだ星座の名前・そろった具合・効果・星の一覧
        //-------------------------------------------------------------
        const entt::entity title    = findText(StarMapElementKind::Title, 0);
        const entt::entity subtitle = findText(StarMapElementKind::Subtitle, 0);
        const entt::entity effect   = findText(StarMapElementKind::Effect, 0);
        const int          maxLevel = chance ? std::max(1, chance->maxStarLevel) : 5;
        // 行の文字の大きさは、グループの一覧で小さくすることがあるので、毎回戻す
        for(int row = 0; row < config->maxRows; ++row) {
            if(FontComponent* font = registry.try_get<FontComponent>(findText(StarMapElementKind::Row, row)))
                font->scale = config->rowScale;
        }

        //-------------------------------------------------------------
        // グループの一覧: 上にグループを並べ、選んだグループの説明・効果・属する星座を出す
        //-------------------------------------------------------------
        if(state->groupMode) {
            const int collectedGroups = modifiers ? modifiers->collectedGroups : 0;
            HudUi::SetText(registry, title, "星座のグループ");
            HudUi::SetTextColor(registry, title, config->selectedLabelColor);
            HudUi::SetText(
                registry, subtitle, "そろった " + std::to_string(collectedGroups) + " / " + std::to_string(groups.groups.size()) + "   Z/X で選ぶ  G で星座へ");

            auto isConstellationCollected = [&](const std::string& id) {
                for(size_t c = 0; c < table.constellations.size(); ++c) {
                    if(table.constellations[c].id == id)
                        return static_cast<bool>(collected[c]);
                }
                return false;
            };
            auto countCollected = [&](const Data::ConstellationGroupDef& g) {
                return static_cast<int>(std::count_if(g.constellationIds.begin(), g.constellationIds.end(), isConstellationCollected));
            };

            if(group) {
                const bool done = countCollected(*group) == static_cast<int>(group->constellationIds.size());
                HudUi::SetText(
                    registry, effect, group->name + (done ? "  効果 発動中  " : "  そろえると  ") + CollectionText::Effect(group->effect, group->effect.value));
                HudUi::SetTextColor(registry, effect, done ? config->collectedLabelColor : config->labelColor);
            } else {
                HudUi::SetText(registry, effect, "");
            }

            std::vector<std::pair<std::string, hlslpp::float4>> lines;
            for(size_t g = 0; g < groups.groups.size(); ++g) {
                const Data::ConstellationGroupDef& entry = groups.groups[g];
                const int                          have  = countCollected(entry);
                const int                          total = static_cast<int>(entry.constellationIds.size());
                const bool                         done  = have == total;
                const bool                         isSel = &entry == group;
                lines.emplace_back(
                    std::string(isSel ? "▶ " : "　 ") + (done ? "✓ " : "") + entry.name + "  " + std::to_string(have) + " / " + std::to_string(total),
                    done ? config->collectedLabelColor : (isSel ? config->selectedLabelColor : config->labelColor));
            }
            const int detailStart = static_cast<int>(lines.size()) + 1;
            if(group) {
                lines.emplace_back("", config->labelColor);
                lines.emplace_back(group->description, config->selectedLabelColor);
                // 属する星座は1行に4つずつ。そろった星座には ✓ を付ける
                std::string line;
                int         inLine = 0;
                for(const std::string& id : group->constellationIds) {
                    const Data::ConstellationDef* member  = table.Find(id);
                    line                                 += (isConstellationCollected(id) ? "✓" : "・") + (member ? member->name : id) + "  ";
                    if(++inLine == 4) {
                        lines.emplace_back(line, config->labelColor);
                        line.clear();
                        inLine = 0;
                    }
                }
                if(!line.empty())
                    lines.emplace_back(line, config->labelColor);
            }
            for(int row = 0; row < config->maxRows; ++row) {
                const entt::entity text = findText(StarMapElementKind::Row, row);
                if(row >= static_cast<int>(lines.size())) {
                    HudUi::SetText(registry, text, "");
                    continue;
                }
                HudUi::SetText(registry, text, lines[row].first);
                HudUi::SetTextColor(registry, text, lines[row].second);
                if(row > detailStart)
                    if(FontComponent* font = registry.try_get<FontComponent>(text))
                        font->scale = config->rowScale * 0.88f;
            }
            return;
        }

        if(!selected) {
            HudUi::SetText(registry, title, "星座を選んでください");
            HudUi::SetTextColor(registry, title, config->labelColor);
            HudUi::SetText(registry, subtitle, "クリックで星座を選ぶ  G でグループ一覧");
            HudUi::SetText(registry, effect, "");
            for(int row = 0; row < config->maxRows; ++row) {
                HudUi::SetText(registry, findText(StarMapElementKind::Row, row), "");
            }
            return;
        }

        int owned    = 0;
        int levelSum = 0;
        for(const Data::ConstellationStarDef& member : selected->stars) {
            if(const auto found = collection->starLevels.find(member.starId); found != collection->starLevels.end()) {
                ++owned;
                levelSum += found->second;
            }
        }
        const bool isDone = owned == static_cast<int>(selected->stars.size());
        // そろっていれば、CollectionEffectSystem と同じく星の平均レベルで伸びた値を出す
        double effectValue = selected->effect.value;
        if(isDone && chance && owned > 0) {
            const double averageLevel  = static_cast<double>(levelSum) / static_cast<double>(owned);
            effectValue               *= 1.0 + chance->effectLevelBonus * std::max(0.0, averageLevel - 1.0);
        }
        HudUi::SetText(registry, title, selected->name);
        HudUi::SetTextColor(registry, title, isDone ? config->collectedLabelColor : config->selectedLabelColor);
        HudUi::SetText(
            registry,
            subtitle,
            selected->nameEn + "   " + (isDone ? std::string("そろった！") : "星 " + std::to_string(owned) + " / " + std::to_string(selected->stars.size())));
        HudUi::SetText(registry, effect, std::string(isDone ? "効果 発動中  " : "そろえると  ") + CollectionText::Effect(selected->effect, effectValue));
        HudUi::SetTextColor(registry, effect, isDone ? config->collectedLabelColor : config->labelColor);

        //-------------------------------------------------------------
        // 星の一覧。明るい順に並べ、持っている星は★と色、持っていない星は淡く出す
        //-------------------------------------------------------------
        std::vector<const Data::StarDef*> members;
        for(const Data::ConstellationStarDef& member : selected->stars) {
            if(const Data::StarDef* star = stars.Find(member.starId))
                members.push_back(star);
        }
        std::sort(members.begin(), members.end(), [](const Data::StarDef* a, const Data::StarDef* b) { return a->magnitude < b->magnitude; });

        //-------------------------------------------------------------
        // 図鑑の文章の下に星の一覧を続け、入りきらない分はホイールで送る
        //-------------------------------------------------------------
        std::vector<std::pair<std::string, hlslpp::float4>> lines;
        for(const std::string& line : Core::WrapText(selected->caption, config->captionWidth)) {
            lines.emplace_back(line, config->labelColor * hlslpp::float4(1.3f, 1.3f, 1.3f, 1.3f));
        }
        if(!lines.empty())
            lines.emplace_back("", config->labelColor);
        for(const Data::StarDef* star : members) {
            const auto  found = collection->starLevels.find(star->id);
            const int   level = found == collection->starLevels.end() ? 0 : found->second;
            std::string marks;
            for(int i = 0; i < maxLevel; ++i) {
                marks += i < level ? "★" : "☆";
            }
            char facts[64];
            std::snprintf(facts, sizeof(facts), "  %.2f等 %s", star->magnitude, CollectionText::SpectralColor(star->spectralType));
            lines.emplace_back((level > 0 ? marks : std::string("未獲得")) + "  " + star->name + facts,
                               level > 0 ? SpectralColor(layout, star->spectralType) : config->missingStarColor * hlslpp::float4(1.4f, 1.4f, 1.4f, 1.4f));
        }

        state->rowScrollMax = std::max(0, static_cast<int>(lines.size()) - config->maxRows);
        state->rowScroll    = std::clamp(state->rowScroll, 0, state->rowScrollMax);
        for(int row = 0; row < config->maxRows; ++row) {
            const entt::entity text  = findText(StarMapElementKind::Row, row);
            const int          index = row + state->rowScroll;
            if(index >= static_cast<int>(lines.size())) {
                HudUi::SetText(registry, text, "");
                continue;
            }
            const bool more = row == config->maxRows - 1 && index + 1 < static_cast<int>(lines.size());
            HudUi::SetText(registry, text, more ? std::string("▼ ホイールで続き") : lines[static_cast<size_t>(index)].first);
            HudUi::SetTextColor(registry, text, more ? config->selectedLabelColor : lines[static_cast<size_t>(index)].second);
        }
    }

}    // namespace StardustFever::ECS

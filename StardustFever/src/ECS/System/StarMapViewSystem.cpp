//-------------------------------------------------------------
//! @file   StarMapViewSystem.cpp
//! @brief  StarMapViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarMapViewSystem.hpp>

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

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIClipComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>
#include <Tsukino/Core/Path.hpp>

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
        //! @brief  画面空間のスプライトを付ける
        //-------------------------------------------------------------
        SpriteComponent& AddSprite(Tsukino::ECS::Registry& registry, entt::entity entity, Tsukino::Asset::AssetHandle texture, int sortOrder, bool additive) {
            SpriteComponent& sprite = registry.AddComponent<SpriteComponent>(entity);
            sprite.textureHandle    = texture;
            sprite.space            = Tsukino::BuiltIn::ECS::SpriteSpace::Screen;
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
        //! @brief  位置と大きさと向きを書く
        //-------------------------------------------------------------
        void Place(Tsukino::ECS::Registry& registry, entt::entity entity, float x, float y, float scaleX, float scaleY, float angle = 0.0f) {
            TransformComponent& transform = registry.GetComponent<TransformComponent>(entity);
            transform.position            = hlslpp::float3(x, y, 0.0f);
            transform.scale               = hlslpp::float3(scaleX, scaleY, 1.0f);
            transform.rotation            = hlslpp::quaternion::rotation_z(angle);
            transform.dirty               = true;
        }
    }    // namespace

    StarMapViewSystem::StarMapViewSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 開け閉めに合わせて作り・片付け、見え方が変わったら並べ直す
    //-------------------------------------------------------------
    void StarMapViewSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const StarMapStateComponent* state = FindSingleton<StarMapStateComponent>(registry);
        const bool                   open  = state && state->open;

        if(!open) {
            if(m_root != entt::null) {
                if(registry.IsValid(m_root))
                    Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, m_root);
                m_root           = entt::null;
                m_appliedVersion = -1;
            }
            return;
        }

        if(m_root == entt::null || !registry.IsValid(m_root)) {
            Build(registry);
            m_appliedVersion = -1;
        }
        if(m_root != entt::null && state->version != m_appliedVersion) {
            Layout(registry);
            m_appliedVersion = state->version;
        }
    }

    //-------------------------------------------------------------
    // 画面の部品を全て作る
    //-------------------------------------------------------------
    void StarMapViewSystem::Build(Tsukino::ECS::Registry& registry) {
        const StarMapConfigComponent*              config  = FindSingleton<StarMapConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!config || !context || !context->assetManager)
            return;

        const Tsukino::Asset::AssetHandle white = context->assetManager->Load(Tsukino::Core::Path(config->backgroundTexture));
        const Tsukino::Asset::AssetHandle dot   = context->assetManager->Load(Tsukino::Core::Path(config->starTexture));
        const float                       px    = std::max(config->backgroundPixels, 1.0f);
        const int                         order = config->sortOrder;

        //-------------------------------------------------------------
        // 画面全体の背景と、星図の枠・右のパネルの板
        //-------------------------------------------------------------
        m_root = MakeElement(*m_scene, registry, entt::null, StarMapElementKind::Root, 0);
        Place(registry, m_root, 0.0f, 0.0f, 1.0f, 1.0f);

        const entt::entity background                                  = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Root, 1);
        AddSprite(registry, background, white, order, false).tintColor = config->backgroundColor;
        Place(registry, background, 800.0f, 500.0f, 1600.0f / px, 1000.0f / px);

        const float        mapCenterX                                    = config->mapX + config->mapWidth * 0.5f;
        const float        mapCenterY                                    = config->mapY + config->mapHeight * 0.5f;
        const entt::entity mapBoard                                      = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Root, 2);
        AddSprite(registry, mapBoard, white, order + 1, false).tintColor = config->mapColor;
        Place(registry, mapBoard, mapCenterX, mapCenterY, config->mapWidth / px, config->mapHeight / px);

        const float        panelHeight                                = config->mapHeight;
        const entt::entity panel                                      = MakeElement(*m_scene, registry, m_root, StarMapElementKind::Root, 3);
        AddSprite(registry, panel, white, order + 1, false).tintColor = config->panelColor;
        Place(registry, panel, config->panelX + config->panelWidth * 0.5f, mapCenterY, config->panelWidth / px, panelHeight / px);

        //-------------------------------------------------------------
        // 星図の枠。星・線・名前はこの子にして、枠の外を切り取る
        //-------------------------------------------------------------
        const entt::entity mapRoot = MakeElement(*m_scene, registry, m_root, StarMapElementKind::MapRoot, 0);
        Place(registry, mapRoot, mapCenterX, mapCenterY, 1.0f, 1.0f);
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIClipComponent>(mapRoot).size = hlslpp::float2(config->mapWidth, config->mapHeight);

        const Data::StarTable&          stars = Data::GetStarTable();
        const Data::ConstellationTable& table = Data::GetConstellationTable();
        for(size_t c = 0; c < table.constellations.size(); ++c) {
            for(size_t l = 0; l < table.constellations[c].lines.size(); ++l) {
                const entt::entity line = MakeElement(*m_scene, registry, mapRoot, StarMapElementKind::Line, static_cast<int>(c), static_cast<int>(l));
                AddSprite(registry, line, white, order + 2, true);
            }
        }
        for(size_t i = 0; i < stars.stars.size(); ++i) {
            const entt::entity star = MakeElement(*m_scene, registry, mapRoot, StarMapElementKind::Star, static_cast<int>(i));
            AddSprite(registry, star, dot, order + 3, true);
        }
        for(size_t c = 0; c < table.constellations.size(); ++c) {
            const entt::entity label = MakeElement(*m_scene, registry, mapRoot, StarMapElementKind::Label, static_cast<int>(c));
            AddText(registry, label, config->labelScale, order + 4, true);
            HudUi::SetText(registry, label, table.constellations[c].name);
        }

        //-------------------------------------------------------------
        // 上のまとめと操作の説明、右のパネルの文字
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
            "ドラッグ・矢印 動かす   ホイール・Q/E 拡大   クリック 星座を選ぶ   Z/X 前後   G グループの一覧   Tab・Esc 閉じる（遊びは止まっています）");

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
    // 星・線・名前を今の見え方で並べ、詳しいパネルの中身を書く
    //-------------------------------------------------------------
    void StarMapViewSystem::Layout(Tsukino::ECS::Registry& registry) {
        const StarMapStateComponent*        state      = FindSingleton<StarMapStateComponent>(registry);
        const StarMapConfigComponent*       config     = FindSingleton<StarMapConfigComponent>(registry);
        const StarCollectionComponent*      collection = FindSingleton<StarCollectionComponent>(registry);
        const CollectionModifiersComponent* modifiers  = FindSingleton<CollectionModifiersComponent>(registry);
        const StarChanceConfigComponent*    chance     = FindSingleton<StarChanceConfigComponent>(registry);
        const HudLayoutConfigComponent*     layout     = FindSingleton<HudLayoutConfigComponent>(registry);
        if(!state || !config || !collection)
            return;

        const Data::StarTable&          stars    = Data::GetStarTable();
        const Data::ConstellationTable& table    = Data::GetConstellationTable();
        const Core::SkyMath::View       view     = StarMapGeometry::MakeView(state->raCenter, state->decCenter, config->pixelsPerDegree, state->zoom);
        const float                     linePx   = std::max(config->backgroundPixels, 1.0f);
        const float                     starPx   = std::max(config->starPixels, 1.0f);
        const float                     growth   = 1.0f + config->starZoomGrowth * static_cast<float>(state->zoom - 1.0);
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

        for(const auto entity : registry.View<StarMapElementComponent>()) {
            const StarMapElementComponent& element = registry.GetComponent<StarMapElementComponent>(entity);
            switch(element.kind) {
            //-------------------------------------------------------------
            // 星: 明るいほど大きく。持っていればスペクトル型の色、無ければ淡い点
            //-------------------------------------------------------------
            case StarMapElementKind::Star:
                {
                    if(element.index < 0 || element.index >= static_cast<int>(stars.stars.size()))
                        break;
                    const Data::StarDef&       star   = stars.stars[element.index];
                    const Core::SkyMath::Point point  = Core::SkyMath::Project(view, star.ra * 15.0, star.dec);
                    const float                bright = std::clamp(static_cast<float>((5.0 - star.magnitude) / 5.0), 0.0f, 1.3f);
                    const bool                 owned  = collection->starLevels.contains(star.id);
                    float                      size   = (config->faintStarSize + (config->brightStarSize - config->faintStarSize) * bright) * growth;
                    if(!owned)
                        size *= 0.7f;
                    Place(registry, entity, static_cast<float>(point.x), static_cast<float>(point.y), size / starPx, size / starPx);
                    if(SpriteComponent* sprite = registry.try_get<SpriteComponent>(entity))
                        sprite->tintColor =
                            owned ? SpectralColor(layout, star.spectralType) * hlslpp::float4(1.6f, 1.6f, 1.6f, 1.0f) : config->missingStarColor;
                    break;
                }
            //-------------------------------------------------------------
            // 線: そろった星座は金色で太く、選んだ星座は明るく、それ以外は淡く
            //-------------------------------------------------------------
            case StarMapElementKind::Line:
                {
                    if(element.index < 0 || element.index >= static_cast<int>(table.constellations.size()))
                        break;
                    const Data::ConstellationDef& constellation = table.constellations[element.index];
                    if(element.subIndex < 0 || element.subIndex >= static_cast<int>(constellation.lines.size()))
                        break;
                    const Data::ConstellationLineDef& line  = constellation.lines[element.subIndex];
                    const int                         count = static_cast<int>(constellation.stars.size());
                    const Data::StarDef*              from  = line.from >= 0 && line.from < count ? stars.Find(constellation.stars[line.from].starId) : nullptr;
                    const Data::StarDef*              to    = line.to >= 0 && line.to < count ? stars.Find(constellation.stars[line.to].starId) : nullptr;
                    if(!from || !to)
                        break;
                    const Core::SkyMath::Point a         = Core::SkyMath::Project(view, from->ra * 15.0, from->dec);
                    const Core::SkyMath::Point b         = Core::SkyMath::ProjectFrom(view, a, from->ra * 15.0, to->ra * 15.0, to->dec);
                    const float                dx        = static_cast<float>(b.x - a.x);
                    const float                dy        = static_cast<float>(b.y - a.y);
                    const bool                 isDone    = collected[element.index];
                    const bool                 isSel     = highlighted[element.index];
                    const float                thickness = (isDone || isSel) ? config->collectedLineThickness : config->lineThickness;
                    Place(registry,
                          entity,
                          static_cast<float>(a.x) + dx * 0.5f,
                          static_cast<float>(a.y) + dy * 0.5f,
                          std::sqrt(dx * dx + dy * dy) / linePx,
                          thickness / linePx,
                          std::atan2(dy, dx));
                    if(SpriteComponent* sprite = registry.try_get<SpriteComponent>(entity))
                        sprite->tintColor = isDone ? config->collectedLineColor : (isSel ? config->selectedLineColor : config->lineColor);
                    break;
                }
            //-------------------------------------------------------------
            // 星座名: 星の平均の位置に。縮小しているときは、そろった星座と選んだ星座だけ出す
            //-------------------------------------------------------------
            case StarMapElementKind::Label:
                {
                    if(element.index < 0 || element.index >= static_cast<int>(table.constellations.size()))
                        break;
                    const Data::ConstellationDef& constellation = table.constellations[element.index];
                    double                        ra            = 0.0;
                    double                        dec           = 0.0;
                    StarMapGeometry::Centroid(constellation, stars, ra, dec);
                    const Core::SkyMath::Point point  = Core::SkyMath::Project(view, ra, dec);
                    const bool                 isDone = collected[element.index];
                    const bool                 isSel  = highlighted[element.index];
                    const bool                 shown  = isDone || isSel || state->zoom >= config->labelMinZoom;
                    Place(registry, entity, static_cast<float>(point.x), static_cast<float>(point.y), 1.0f, 1.0f);
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
            HudUi::SetText(registry, subtitle, "星図の星か星座名をクリック（Z/X でも選べます）");
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
        for(int row = 0; row < config->maxRows; ++row) {
            const entt::entity text = findText(StarMapElementKind::Row, row);
            if(row >= static_cast<int>(members.size())) {
                HudUi::SetText(registry, text, row == config->maxRows - 1 && static_cast<int>(members.size()) > config->maxRows ? "…" : "");
                continue;
            }
            const Data::StarDef& star  = *members[row];
            const auto           found = collection->starLevels.find(star.id);
            const int            level = found == collection->starLevels.end() ? 0 : found->second;
            std::string          stars_;
            for(int i = 0; i < maxLevel; ++i) {
                stars_ += i < level ? "★" : "☆";
            }
            char facts[64];
            std::snprintf(facts, sizeof(facts), "  %.2f等 %s", star.magnitude, CollectionText::SpectralColor(star.spectralType));
            HudUi::SetText(registry, text, (level > 0 ? stars_ : std::string("未獲得")) + "  " + star.name + facts);
            HudUi::SetTextColor(
                registry, text, level > 0 ? SpectralColor(layout, star.spectralType) : config->missingStarColor * hlslpp::float4(1.4f, 1.4f, 1.4f, 1.4f));
        }
    }

}    // namespace StardustFever::ECS

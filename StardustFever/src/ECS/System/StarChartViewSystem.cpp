//-------------------------------------------------------------
//! @file   StarChartViewSystem.cpp
//! @brief  StarChartViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarChartViewSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChartEntryComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/StarLinkConfigComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;
        namespace Tween = Tsukino::BuiltIn::ECS::UITweenUtility;

        //-------------------------------------------------------------
        //! @brief  スペクトル型（の頭文字）から星の色を選ぶ
        //-------------------------------------------------------------
        hlslpp::float4 SpectralColor(const HudLayoutConfigComponent& layout, const std::string& spectralType) {
            switch(spectralType.empty() ? 'A' : spectralType.front()) {
            case 'O':
                return layout.colorO;
            case 'B':
                return layout.colorB;
            case 'F':
                return layout.colorF;
            case 'G':
                return layout.colorG;
            case 'K':
                return layout.colorK;
            case 'M':
                return layout.colorM;
            case 'A':
            default:
                return layout.colorA;
            }
        }

        //-------------------------------------------------------------
        //! @brief  星図に出す行の数（見出し＋星）
        //-------------------------------------------------------------
        size_t CountRows() {
            size_t rows = 0;
            for(const Data::ConstellationDef& constellation : Data::GetConstellationTable().constellations) {
                rows += 1 + constellation.stars.size();
            }
            return rows;
        }

        //-------------------------------------------------------------
        //! @brief  レベルを ★★☆☆☆ のように表す
        //-------------------------------------------------------------
        std::string LevelStars(int level, int maxLevel) {
            std::string text;
            for(int i = 0; i < maxLevel; ++i) {
                text += i < level ? "★" : "☆";
            }
            return text;
        }
    }    // namespace

    StarChartViewSystem::StarChartViewSystem()
        : m_random(std::random_device{}()) {
    }

    //-------------------------------------------------------------
    // 行を全て作り直す
    //-------------------------------------------------------------
    void StarChartViewSystem::Rebuild(Tsukino::ECS::Registry& registry) {
        const HudLayoutConfigComponent*            layout  = FindSingleton<HudLayoutConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!layout || !context || !context->prefabFactory)
            return;

        std::vector<Tsukino::ECS::Entity> old;
        for(const auto entity : registry.View<StarChartEntryComponent>()) {
            old.push_back(entity);
        }
        for(const Tsukino::ECS::Entity entity : old) {
            Tsukino::BuiltIn::ECS::TransformUtility::DestroyHierarchy(registry, entity);
        }

        float y      = layout->chartY;
        int   order  = 0;
        auto  appear = [&](Tsukino::ECS::Entity row) {
            // 右からずれて順番に現れる。間隔はわずかにばらつかせ、機械的な等間隔にしない
            std::uniform_real_distribution<float> jitter(0.0f, 0.02f);
            Tween::SlideIn(registry, row, hlslpp::float2(40.0f, 0.0f), 0.025f * static_cast<float>(order) + jitter(m_random), 0.45f);
            ++order;
        };

        for(const Data::ConstellationDef& constellation : Data::GetConstellationTable().constellations) {
            const Tsukino::ECS::Entity header = HudUi::InstantiateTemplate(*context->prefabFactory, registry, layout->chartHeaderGroup);
            if(header == entt::null)
                return;
            HudUi::SetPosition(registry, header, layout->chartX, y);
            registry.AddComponent<StarChartEntryComponent>(header).constellationId = constellation.id;
            HudUi::SetText(registry, HudUi::FindPart(registry, header, "Name"), constellation.name);
            appear(header);
            y += layout->chartHeaderSpacing;

            for(const Data::ConstellationStarDef& member : constellation.stars) {
                const Tsukino::ECS::Entity row = HudUi::InstantiateTemplate(*context->prefabFactory, registry, layout->chartRowGroup);
                if(row == entt::null)
                    return;
                HudUi::SetPosition(registry, row, layout->chartX + layout->chartIndent, y);
                registry.AddComponent<StarChartEntryComponent>(row).starId = member.starId;
                const Data::StarDef* star                                  = Data::GetStarTable().Find(member.starId);
                HudUi::SetText(registry, HudUi::FindPart(registry, row, "Name"), star ? star->name : member.starId);
                appear(row);
                y += layout->chartRowSpacing;
            }
        }
    }

    //-------------------------------------------------------------
    // 行を作り、見た目を更新する
    //-------------------------------------------------------------
    void StarChartViewSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const HudLayoutConfigComponent*            layout     = FindSingleton<HudLayoutConfigComponent>(registry);
        const StarCollectionComponent*             collection = FindSingleton<StarCollectionComponent>(registry);
        const StarLinkConfigComponent*             starLink   = FindSingleton<StarLinkConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context    = GetEngineContext(registry);
        if(!layout || !collection || !starLink || !context || !context->assetManager)
            return;

        size_t rows = 0;
        for([[maybe_unused]] const auto entity : registry.View<StarChartEntryComponent>()) {
            ++rows;
        }
        if(rows != CountRows())
            Rebuild(registry);

        const int maxLevel = std::max(1, starLink->maxStarLevel);
        for(const auto row : registry.View<StarChartEntryComponent>()) {
            StarChartEntryComponent& entry = registry.GetComponent<StarChartEntryComponent>(row);

            //-------------------------------------------------------------
            // 星座の見出し: 完成回数。増えた瞬間に弾ませる
            //-------------------------------------------------------------
            if(!entry.constellationId.empty()) {
                const auto found = collection->constellationCompletions.find(entry.constellationId);
                const int  count = found == collection->constellationCompletions.end() ? 0 : found->second;
                HudUi::SetText(registry, HudUi::FindPart(registry, row, "Count"), count > 0 ? "完成 " + std::to_string(count) : "未完成");
                if(entry.level >= 0 && count > entry.level)
                    Tween::Punch(registry, row, 0.15f, 0.5f);
                entry.level = count;
                continue;
            }

            //-------------------------------------------------------------
            // 星の行: 見つけたかどうかでアイコンと色を変え、レベルが上がったら弾ませる
            //-------------------------------------------------------------
            const auto found = collection->starLevels.find(entry.starId);
            const int  level = found == collection->starLevels.end() ? 0 : found->second;
            if(level == entry.level)
                continue;

            const bool                 discovered = level > 0;
            const Data::StarDef*       star       = Data::GetStarTable().Find(entry.starId);
            const Tsukino::ECS::Entity icon       = HudUi::FindPart(registry, row, "Icon");
            if(auto* sprite = icon != entt::null ? registry.try_get<Tsukino::BuiltIn::ECS::SpriteComponent>(icon) : nullptr) {
                const std::string& texture = discovered ? layout->starIconTexture : layout->starOutlineTexture;
                if(!texture.empty())
                    sprite->textureHandle = context->assetManager->Load(Tsukino::Core::Path(texture));
                sprite->tintColor = discovered && star ? SpectralColor(*layout, star->spectralType) : layout->undiscoveredColor;
            }
            HudUi::SetTextColor(
                registry, HudUi::FindPart(registry, row, "Name"), discovered ? hlslpp::float4(0.93f, 0.95f, 1.0f, 1.0f) : layout->undiscoveredColor);
            HudUi::SetText(registry, HudUi::FindPart(registry, row, "Level"), discovered ? LevelStars(level, maxLevel) : "");

            if(entry.level >= 0 && level > entry.level) {
                std::uniform_real_distribution<float> amount(0.5f, 0.75f);
                Tween::Punch(registry, icon, amount(m_random), 0.5f);
                Tween::Punch(registry, row, 0.06f, 0.4f);
            }
            entry.level = level;
        }
    }

}    // namespace StardustFever::ECS

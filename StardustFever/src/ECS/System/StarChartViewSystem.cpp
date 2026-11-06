//-------------------------------------------------------------
//! @file   StarChartViewSystem.cpp
//! @brief  StarChartViewSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarChartViewSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChartEntryComponent.hpp>
#include <StardustFever/ECS/Component/StarCollectionComponent.hpp>
#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>
#include <StardustFever/ECS/Utility/StarColor.hpp>

#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Transform/TransformUtility.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <unordered_map>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::UITweenProperty;
        using Tsukino::Core::Math::Easing::EaseType;
        namespace Tween = Tsukino::BuiltIn::ECS::UITweenUtility;

        // 見出しの行の種類（StarChartEntryComponent::constellationId に入れて見分ける）
        constexpr const char* kConstellationsHeader = "#constellations";
        constexpr const char* kStarsHeader          = "#stars";
        constexpr const char* kGroupsHeader         = "#groups";
        constexpr const char* kSectionHeader        = "#section";

        //-------------------------------------------------------------
        //! @brief  今出すべき星の行の並び
        //! @param  registry   [in] レジストリ
        //! @param  collection [in] 持ち物
        //! @param  chanceId   [out] チャンス中なら星座のID（それ以外は空）
        //! @return 星の ID の並び。チャンス中は盤面の星、それ以外は最近手に入れた星
        //-------------------------------------------------------------
        std::vector<std::string> RowStars(Tsukino::ECS::Registry& registry, const StarCollectionComponent& collection, std::string& chanceId) {
            chanceId.clear();
            const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry);
            if(chance && chance->active) {
                if(const Data::ConstellationDef* constellation = Data::GetConstellationTable().Find(chance->constellationId)) {
                    chanceId = constellation->id;
                    std::vector<std::string> ids;
                    for(const Data::ConstellationBoardStarDef& star : constellation->board.stars) {
                        ids.push_back(star.starId);
                    }
                    return ids;
                }
            }
            return collection.recentStars;
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
    void StarChartViewSystem::Rebuild(Tsukino::ECS::Registry& registry, const std::vector<std::string>& starIds, const std::string& chanceId) {
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
        auto header = [&](const char* kind, const std::string& name) {
            const Tsukino::ECS::Entity entity = HudUi::InstantiateTemplate(*context->prefabFactory, registry, layout->chartHeaderGroup);
            if(entity == entt::null)
                return false;
            HudUi::SetPosition(registry, entity, layout->chartX, y);
            registry.AddComponent<StarChartEntryComponent>(entity).constellationId = kind;
            HudUi::SetText(registry, HudUi::FindPart(registry, entity, "Name"), name);
            appear(entity);
            y += layout->chartHeaderSpacing;
            return true;
        };

        //-------------------------------------------------------------
        // まとめ（星座・星の数）、その下に、チャンス中は盤面の星、それ以外は最近手に入れた星を並べる
        //-------------------------------------------------------------
        if(!header(kConstellationsHeader, "そろった星座") || !header(kGroupsHeader, "そろったグループ") || !header(kStarsHeader, "集めた星"))
            return;
        const Data::ConstellationDef* chance = chanceId.empty() ? nullptr : Data::GetConstellationTable().Find(chanceId);
        if(!header(kSectionHeader, chance ? chance->name : std::string("最近手に入れた星")))
            return;

        for(const std::string& starId : starIds) {
            const Tsukino::ECS::Entity row = HudUi::InstantiateTemplate(*context->prefabFactory, registry, layout->chartRowGroup);
            if(row == entt::null)
                return;
            HudUi::SetPosition(registry, row, layout->chartX + layout->chartIndent, y);
            registry.AddComponent<StarChartEntryComponent>(row).starId = starId;
            const Data::StarDef* star                                  = Data::GetStarTable().Find(starId);
            HudUi::SetText(registry, HudUi::FindPart(registry, row, "Name"), star ? star->name : starId);
            appear(row);
            y += layout->chartRowSpacing;
        }
    }

    //-------------------------------------------------------------
    // 行を作り、見た目を更新する
    //-------------------------------------------------------------
    void StarChartViewSystem::Update(Tsukino::ECS::Registry& registry, float) {
        const HudLayoutConfigComponent*            layout     = FindSingleton<HudLayoutConfigComponent>(registry);
        const StarCollectionComponent*             collection = FindSingleton<StarCollectionComponent>(registry);
        const StarChanceConfigComponent*           config     = FindSingleton<StarChanceConfigComponent>(registry);
        const CollectionModifiersComponent*        modifiers  = FindSingleton<CollectionModifiersComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context    = GetEngineContext(registry);
        if(!layout || !collection || !config || !context || !context->assetManager)
            return;

        //-------------------------------------------------------------
        // 並べる星が変わったら作り直す（チャンスの始まり・終わり、星を引いたとき）
        //-------------------------------------------------------------
        std::string                    chanceId;
        const std::vector<std::string> starIds = RowStars(registry, *collection, chanceId);
        std::string                    key     = chanceId + "|";
        for(const std::string& id : starIds) {
            key += id + ",";
        }
        if(key != m_rowsKey) {
            Rebuild(registry, starIds, chanceId);
            m_rowsKey = key;
        }

        // チャンス中は、盤面でつないだ星を明るくする
        std::unordered_map<std::string, bool> linked;
        if(!chanceId.empty()) {
            for(const auto entity : registry.View<StarPinComponent>()) {
                const StarPinComponent& pin = registry.GetComponent<StarPinComponent>(entity);
                linked[pin.starId]          = pin.linked;
            }
        }

        const int maxLevel = std::max(1, config->maxStarLevel);
        for(const auto row : registry.View<StarChartEntryComponent>()) {
            StarChartEntryComponent& entry = registry.GetComponent<StarChartEntryComponent>(row);

            //-------------------------------------------------------------
            // 見出し: そろった数。増えた瞬間に弾ませる
            //-------------------------------------------------------------
            if(!entry.constellationId.empty()) {
                int         count = -1;
                std::string text;
                if(entry.constellationId == kConstellationsHeader && modifiers) {
                    count = modifiers->collectedConstellations;
                    text  = std::to_string(count) + " / " + std::to_string(modifiers->totalConstellations);
                } else if(entry.constellationId == kGroupsHeader && modifiers) {
                    count = modifiers->collectedGroups;
                    text  = std::to_string(count) + " / " + std::to_string(modifiers->totalGroups) + " ▶";    // 押すと一覧が開く
                } else if(entry.constellationId == kStarsHeader && modifiers) {
                    count = modifiers->ownedStars;
                    text  = std::to_string(count) + " / " + std::to_string(modifiers->totalStars);
                } else if(entry.constellationId == kSectionHeader) {
                    const StarChanceStateComponent* chance = FindSingleton<StarChanceStateComponent>(registry);
                    if(chance && chance->active) {
                        count = chance->starsLinked;
                        text  = "つないだ " + std::to_string(chance->starsLinked) + " / " + std::to_string(chance->starsTotal);
                    }
                }
                HudUi::SetText(registry, HudUi::FindPart(registry, row, "Count"), text);
                if(entry.level >= 0 && count > entry.level)
                    Tween::Punch(registry, row, 0.15f, 0.5f);
                entry.level = count;
                continue;
            }

            //-------------------------------------------------------------
            // 星の行: チャンス中はつないだかで、それ以外は持っているかでアイコンと色を変える。
            // ★の段数は持っているレベル。上がったら弾ませる
            //-------------------------------------------------------------
            const auto found   = collection->starLevels.find(entry.starId);
            const int  level   = found == collection->starLevels.end() ? 0 : found->second;
            const auto pin     = linked.find(entry.starId);
            const bool lit     = chanceId.empty() ? level > 0 : (pin != linked.end() && pin->second);
            const int  stateId = level * 2 + (lit ? 1 : 0);
            if(stateId == entry.level)
                continue;

            const Data::StarDef*       star = Data::GetStarTable().Find(entry.starId);
            const Tsukino::ECS::Entity icon = HudUi::FindPart(registry, row, "Icon");
            if(auto* sprite = icon != entt::null ? registry.try_get<Tsukino::BuiltIn::ECS::SpriteComponent>(icon) : nullptr) {
                const std::string& texture = lit ? layout->starIconTexture : layout->starOutlineTexture;
                if(!texture.empty())
                    sprite->textureHandle = context->assetManager->Load(Tsukino::Core::Path(texture));
                sprite->tintColor = lit && star ? SpectralColor(layout, star->spectralType) : layout->undiscoveredColor;
            }
            HudUi::SetTextColor(registry, HudUi::FindPart(registry, row, "Name"), lit ? hlslpp::float4(0.93f, 0.95f, 1.0f, 1.0f) : layout->undiscoveredColor);
            HudUi::SetText(registry, HudUi::FindPart(registry, row, "Level"), level > 0 ? LevelStars(level, maxLevel) : "");

            if(entry.level >= 0 && stateId > entry.level) {
                std::uniform_real_distribution<float> amount(0.5f, 0.75f);
                Tween::Punch(registry, icon, amount(m_random), 0.5f);
                Tween::Punch(registry, row, 0.06f, 0.4f);
            }
            entry.level = stateId;
        }
    }

}    // namespace StardustFever::ECS

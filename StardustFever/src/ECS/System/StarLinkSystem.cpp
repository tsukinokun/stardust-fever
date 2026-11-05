//-------------------------------------------------------------
//! @file   StarLinkSystem.cpp
//! @brief  StarLinkSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarLinkSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/LinkLineComponent.hpp>
#include <StardustFever/ECS/Component/StarLinkConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarChanceStateComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Event/StarLinkedEvent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/MaterialPropertyBlockComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/ModelComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <cmath>
#include <unordered_set>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  線の太さと自己発光を、ガイドか灯った線かに合わせる
        //! @param  registry [in] レジストリ
        //! @param  entity   [in] 線
        //! @param  config   [in] 太さと色
        //! @param  lit      [in] 灯った線か
        //-------------------------------------------------------------
        void ApplyLineLook(Tsukino::ECS::Registry& registry, entt::entity entity, const StarLinkConfigComponent& config, bool lit) {
            if(TransformComponent* transform = registry.try_get<TransformComponent>(entity)) {
                transform->scale = hlslpp::float3(transform->scale.x, lit ? config.lineThickness : config.guideThickness, transform->scale.z);
                transform->dirty = true;
            }
            using Tsukino::BuiltIn::ECS::MaterialPropertyBlockComponent;
            MaterialPropertyBlockComponent& block = registry.HasComponent<MaterialPropertyBlockComponent>(entity)
                                                        ? registry.GetComponent<MaterialPropertyBlockComponent>(entity)
                                                        : registry.AddComponent<MaterialPropertyBlockComponent>(entity);
            block.emissive                        = lit ? config.litEmissive : config.guideEmissive;
        }

        //-------------------------------------------------------------
        //! @brief  2つの星の間に線の Prefab を伸ばして置く
        //! @param  factory  [in] Prefab の生成元
        //! @param  registry [in] レジストリ
        //! @param  config   [in] 線の Prefab
        //! @param  from     [in] 始点
        //! @param  to       [in] 終点
        //! @return 置いた線（作れなければ entt::null）
        //! @note   線のモデルはX方向に長さ2（±1）の箱。中点へ置き、Z軸回りに向けて、半分の長さだけ伸ばす。
        //!         Z は Prefab の値のまま（星より奥に置き、星に重ならないようにしてある）
        //-------------------------------------------------------------
        entt::entity SpawnLine(Tsukino::Engine::ECS::Prefab::PrefabFactory& factory,
                               Tsukino::ECS::Registry&                      registry,
                               const StarLinkConfigComponent&               config,
                               float                                        fromX,
                               float                                        fromY,
                               float                                        toX,
                               float                                        toY) {
            const entt::entity entity = factory.Instantiate(config.linkLinePrefab, registry);
            if(entity == entt::null)
                return entt::null;
            TransformComponent* transform = registry.try_get<TransformComponent>(entity);
            if(!transform)
                return entity;

            const float dx     = toX - fromX;
            const float dy     = toY - fromY;
            const float length = std::sqrt(dx * dx + dy * dy);
            const float z      = transform->position.z;
            const float depth  = transform->scale.z;

            transform->position = hlslpp::float3((fromX + toX) * 0.5f, (fromY + toY) * 0.5f, z);
            transform->rotation = hlslpp::quaternion::rotation_z(std::atan2(dy, dx));
            transform->scale    = hlslpp::float3(length * 0.5f, config.guideThickness, depth);
            transform->dirty    = true;
            return entity;
        }
    }    // namespace

    StarLinkSystem::StarLinkSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_touches(eventBus)
        , m_starts(eventBus) {
    }

    //-------------------------------------------------------------
    // ガイドを引いてから、溜まった接触を処理する
    //-------------------------------------------------------------
    void StarLinkSystem::Update(Tsukino::ECS::Registry& registry, float) {
        // 開始と同じフレームに触れた星も、ガイドを引いた後に処理して灯せるようにする
        for(const StarChanceStartedEvent& started : m_starts.Drain()) {
            SpawnGuideLines(registry, started);
        }
        for(const BallTouchedStarEvent& touch : m_touches.Drain()) {
            LinkStar(registry, touch);
        }
    }

    //-------------------------------------------------------------
    // 星座の線を全部ガイドとして引く
    //-------------------------------------------------------------
    void StarLinkSystem::SpawnGuideLines(Tsukino::ECS::Registry& registry, const StarChanceStartedEvent& started) {
        const StarLinkConfigComponent*             config        = FindSingleton<StarLinkConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context       = GetEngineContext(registry);
        const Data::ConstellationDef*              constellation = Data::GetConstellationTable().Find(started.constellationId);
        if(!config || !context || !context->prefabFactory || !constellation)
            return;

        // 星釘は盤面の形（board）の x, y に置かれるので、端の位置もそこから取る
        const Data::ConstellationBoardDef& board     = constellation->board;
        const int                          starCount = static_cast<int>(board.stars.size());
        for(const Data::ConstellationLineDef& line : board.lines) {
            if(line.from < 0 || line.to < 0 || line.from >= starCount || line.to >= starCount || line.from == line.to)
                continue;
            const Data::ConstellationBoardStarDef& from   = board.stars[line.from];
            const Data::ConstellationBoardStarDef& to     = board.stars[line.to];
            const entt::entity                     entity = SpawnLine(*context->prefabFactory, registry, *config, from.x, from.y, to.x, to.y);
            if(entity == entt::null)
                continue;

            LinkLineComponent& link = registry.HasComponent<LinkLineComponent>(entity) ? registry.GetComponent<LinkLineComponent>(entity)
                                                                                       : registry.AddComponent<LinkLineComponent>(entity);
            link.from               = line.from;
            link.to                 = line.to;
            link.lit                = false;
            ApplyLineLook(registry, entity, *config, false);
        }
    }

    //-------------------------------------------------------------
    // 星釘1つをつなぐ
    //-------------------------------------------------------------
    void StarLinkSystem::LinkStar(Tsukino::ECS::Registry& registry, const BallTouchedStarEvent& touch) {
        StarChanceStateComponent*                  chance       = FindSingleton<StarChanceStateComponent>(registry);
        const StarChanceConfigComponent*           chanceConfig = FindSingleton<StarChanceConfigComponent>(registry);
        const StarLinkConfigComponent*             config       = FindSingleton<StarLinkConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context      = GetEngineContext(registry);
        if(!chance || !chance->active || chance->completed || !config || !context)
            return;
        if(!registry.IsValid(touch.ball) || !registry.IsValid(touch.starPin))
            return;

        // 既につながった星に触れても何も起きない
        StarPinComponent* pin = registry.try_get<StarPinComponent>(touch.starPin);
        if(!pin || pin->linked)
            return;

        pin->linked = true;
        hlslpp::float3 pinPosition(0.0f, 0.0f, 0.0f);
        if(const auto* pinTransform = registry.try_get<TransformComponent>(touch.starPin))
            pinPosition = pinTransform->position;
        if(auto* model = registry.try_get<Tsukino::BuiltIn::ECS::ModelComponent>(touch.starPin);
           model && !config->litStarModel.empty() && context->assetManager)
            model->modelHandle = context->assetManager->Load(Tsukino::Core::Path(config->litStarModel));

        ++chance->starsLinked;
        m_eventBus->Publish(StarLinkedEvent{pin->starId, pinPosition});

        //-------------------------------------------------------------
        // この星を端に持ち、もう片方の端も既につながっているガイドを灯す
        //-------------------------------------------------------------
        std::unordered_set<int> linkedIndices;
        for(const auto entity : registry.View<StarPinComponent>()) {
            const StarPinComponent& other = registry.GetComponent<StarPinComponent>(entity);
            if(other.linked)
                linkedIndices.insert(other.starIndex);
        }

        const int index = pin->starIndex;
        for(const auto entity : registry.View<LinkLineComponent>()) {
            LinkLineComponent& line = registry.GetComponent<LinkLineComponent>(entity);
            if(line.lit || (line.from != index && line.to != index))
                continue;
            const int otherIndex = (line.from == index) ? line.to : line.from;
            if(!linkedIndices.contains(otherIndex))
                continue;
            line.lit = true;
            ApplyLineLook(registry, entity, *config, true);
        }

        // 全部つないだら、余韻のあとで StarChanceSystem が終わらせ、StarDrawSystem が星を引く
        if(chance->starsLinked >= chance->starsTotal) {
            chance->completed = true;
            chance->endDelay  = chanceConfig ? chanceConfig->endDelayAfterComplete : 0.0f;
        }
    }

}    // namespace StardustFever::ECS

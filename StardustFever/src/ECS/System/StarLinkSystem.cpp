//-------------------------------------------------------------
//! @file   StarLinkSystem.cpp
//! @brief  StarLinkSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarLinkSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/StarLinkConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Event/ConstellationCompletedEvent.hpp>
#include <StardustFever/ECS/Event/StardustEarnedEvent.hpp>
#include <StardustFever/ECS/Event/StarLinkedEvent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>
#include <StardustFever/ECS/Utility/Singleton.hpp>

#include <Tsukino/BuiltIn/ECS/Component/ModelComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Path.hpp>

#include <algorithm>
#include <cmath>
#include <unordered_map>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  2つの星の間に線の Prefab を伸ばして置く
        //! @param  factory  [in] Prefab の生成元
        //! @param  registry [in] レジストリ
        //! @param  config   [in] 線の Prefab と太さ
        //! @param  from     [in] 始点
        //! @param  to       [in] 終点
        //! @note   線のモデルはX方向に長さ2（±1）の箱。中点へ置き、Z軸回りに向けて、半分の長さだけ伸ばす。
        //!         Z は Prefab の値のまま（星より奥に置き、星に重ならないようにしてある）
        //-------------------------------------------------------------
        void SpawnLine(Tsukino::Engine::ECS::Prefab::PrefabFactory& factory,
                       Tsukino::ECS::Registry&                      registry,
                       const StarLinkConfigComponent&               config,
                       const hlslpp::float3&                        from,
                       const hlslpp::float3&                        to) {
            const entt::entity entity = factory.Instantiate(config.linkLinePrefab, registry);
            if(entity == entt::null)
                return;
            TransformComponent* transform = registry.try_get<TransformComponent>(entity);
            if(!transform)
                return;

            const float dx     = static_cast<float>(to.x) - static_cast<float>(from.x);
            const float dy     = static_cast<float>(to.y) - static_cast<float>(from.y);
            const float length = std::sqrt(dx * dx + dy * dy);
            const float z      = transform->position.z;
            const float depth  = transform->scale.z;

            transform->position = hlslpp::float3(
                (static_cast<float>(from.x) + static_cast<float>(to.x)) * 0.5f, (static_cast<float>(from.y) + static_cast<float>(to.y)) * 0.5f, z);
            transform->rotation = hlslpp::quaternion::rotation_z(std::atan2(dy, dx));
            transform->scale    = hlslpp::float3(length * 0.5f, config.lineThickness, depth);
            transform->dirty    = true;
        }
    }    // namespace

    StarLinkSystem::StarLinkSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus)
        , m_touches(eventBus) {
    }

    //-------------------------------------------------------------
    // 溜まった接触を処理する
    //-------------------------------------------------------------
    void StarLinkSystem::Update(Tsukino::ECS::Registry& registry, float) {
        for(const BallTouchedStarEvent& touch : m_touches.Drain()) {
            LinkStar(registry, touch);
        }
    }

    //-------------------------------------------------------------
    // 星釘1つをつなぐ
    //-------------------------------------------------------------
    void StarLinkSystem::LinkStar(Tsukino::ECS::Registry& registry, const BallTouchedStarEvent& touch) {
        FeverStateComponent*                       fever   = FindSingleton<FeverStateComponent>(registry);
        const StarLinkConfigComponent*             config  = FindSingleton<StarLinkConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!fever || !fever->active || !config || !context)
            return;
        if(!registry.IsValid(touch.ball) || !registry.IsValid(touch.starPin))
            return;

        StarPinComponent* pin  = registry.try_get<StarPinComponent>(touch.starPin);
        BallComponent*    ball = registry.try_get<BallComponent>(touch.ball);
        // 既につながった星に触れても何も起きない（一筆書きの数も途切れない）
        if(!pin || pin->linked || !ball)
            return;

        pin->linked = true;
        hlslpp::float3 pinPosition(0.0f, 0.0f, 0.0f);
        if(const auto* pinTransform = registry.try_get<TransformComponent>(touch.starPin))
            pinPosition = pinTransform->position;
        if(auto* model = registry.try_get<Tsukino::BuiltIn::ECS::ModelComponent>(touch.starPin);
           model && !config->litStarModel.empty() && context->assetManager)
            model->modelHandle = context->assetManager->Load(Tsukino::Core::Path(config->litStarModel));

        //-------------------------------------------------------------
        // 一筆書き: この玉がこのフィーバーでつないだ n 個目なら strokeBase^(n-1) 倍。
        // 最初の玉と違う玉でつないだ時点で、星座完成時のパーフェクトは無くなる
        //-------------------------------------------------------------
        ++ball->linkedStarCount;
        const int                        chain            = ball->linkedStarCount;
        const UpgradeModifiersComponent* upgrades         = FindSingleton<UpgradeModifiersComponent>(registry);
        const double                     strokeBase       = config->strokeBase + (upgrades ? upgrades->strokeBaseBonus : 0.0);
        const Core::BigNumber            strokeMultiplier = Core::BigNumber::Pow(strokeBase, static_cast<double>(chain - 1));

        ++fever->starsLinked;
        fever->bestChain = std::max(fever->bestChain, chain);
        if(fever->firstLinkBall == entt::null)
            fever->firstLinkBall = touch.ball;
        else if(fever->firstLinkBall != touch.ball)
            fever->perfect = false;

        if(const Data::StarDef* star = Data::GetStarTable().Find(pin->starId))
            m_eventBus->Publish(StardustEarnedEvent{Core::BigNumber(star->basePayout) * strokeMultiplier, pinPosition, true});
        m_eventBus->Publish(StarLinkedEvent{pin->starId, chain, strokeMultiplier.ToDouble(), pinPosition});

        //-------------------------------------------------------------
        // この星を端に持ち、もう片方の端も既につながっている線を引く
        //-------------------------------------------------------------
        const Data::ConstellationDef* constellation = Data::GetConstellationTable().Find(fever->constellationId);
        if(constellation && context->prefabFactory) {
            std::unordered_map<int, hlslpp::float3> linkedPositions;
            for(const auto entity : registry.View<StarPinComponent, TransformComponent>()) {
                const StarPinComponent& other = registry.GetComponent<StarPinComponent>(entity);
                if(other.linked)
                    linkedPositions.emplace(other.starIndex, registry.GetComponent<TransformComponent>(entity).position);
            }

            const int index = pin->starIndex;
            for(const Data::ConstellationLineDef& line : constellation->lines) {
                if(line.from != index && line.to != index)
                    continue;
                const int  otherIndex = (line.from == index) ? line.to : line.from;
                const auto self       = linkedPositions.find(index);
                const auto other      = linkedPositions.find(otherIndex);
                if(self != linkedPositions.end() && other != linkedPositions.end() && otherIndex != index)
                    SpawnLine(*context->prefabFactory, registry, *config, self->second, other->second);
            }
        }

        if(!fever->completed && fever->starsLinked >= fever->starsTotal) {
            fever->completed = true;
            m_eventBus->Publish(ConstellationCompletedEvent{fever->constellationId, fever->perfect});
        }
    }

}    // namespace StardustFever::ECS

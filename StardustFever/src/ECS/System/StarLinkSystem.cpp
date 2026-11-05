//-------------------------------------------------------------
//! @file   StarLinkSystem.cpp
//! @brief  StarLinkSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/StarLinkSystem.hpp>

#include <StardustFever/Data/GameDatabase.hpp>
#include <StardustFever/ECS/Component/BallComponent.hpp>
#include <StardustFever/ECS/Component/FeverStateComponent.hpp>
#include <StardustFever/ECS/Component/LinkLineComponent.hpp>
#include <StardustFever/ECS/Component/StarLinkConfigComponent.hpp>
#include <StardustFever/ECS/Component/StarPinComponent.hpp>
#include <StardustFever/ECS/Component/UpgradeModifiersComponent.hpp>
#include <StardustFever/ECS/Event/ConstellationCompletedEvent.hpp>
#include <StardustFever/ECS/Event/StardustEarnedEvent.hpp>
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
        for(const FeverStartedEvent& started : m_starts.Drain()) {
            SpawnGuideLines(registry, started);
        }
        for(const BallTouchedStarEvent& touch : m_touches.Drain()) {
            LinkStar(registry, touch);
        }
    }

    //-------------------------------------------------------------
    // 星座の線を全部ガイドとして引く
    //-------------------------------------------------------------
    void StarLinkSystem::SpawnGuideLines(Tsukino::ECS::Registry& registry, const FeverStartedEvent& started) {
        const StarLinkConfigComponent*             config        = FindSingleton<StarLinkConfigComponent>(registry);
        Tsukino::EngineIntegration::EngineContext* context       = GetEngineContext(registry);
        const Data::ConstellationDef*              constellation = Data::GetConstellationTable().Find(started.constellationId);
        if(!config || !context || !context->prefabFactory || !constellation)
            return;

        // 星釘は星座データの x, y に置かれるので、端の位置もデータから取る
        const int starCount = static_cast<int>(constellation->stars.size());
        for(const Data::ConstellationLineDef& line : constellation->lines) {
            if(line.from < 0 || line.to < 0 || line.from >= starCount || line.to >= starCount || line.from == line.to)
                continue;
            const Data::ConstellationStarDef& from   = constellation->stars[line.from];
            const Data::ConstellationStarDef& to     = constellation->stars[line.to];
            const entt::entity                entity = SpawnLine(*context->prefabFactory, registry, *config, from.x, from.y, to.x, to.y);
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

        if(!fever->completed && fever->starsLinked >= fever->starsTotal) {
            fever->completed = true;
            m_eventBus->Publish(ConstellationCompletedEvent{fever->constellationId, fever->perfect});
        }
    }

}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   BoardBuildSystem.cpp
//! @brief  BoardBuildSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/BoardBuildSystem.hpp>

#include <StardustFever/Data/BoardLayoutData.hpp>
#include <StardustFever/ECS/Component/BoardBuildRequestComponent.hpp>
#include <StardustFever/ECS/Utility/EngineAccess.hpp>

#include <Tsukino/BuiltIn/ECS/Component/CollisionComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/Log.hpp>

#include <cereal/archives/json.hpp>

#include <exception>
#include <fstream>
#include <numbers>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        using Tsukino::BuiltIn::ECS::CollisionComponent;
        using Tsukino::BuiltIn::ECS::TransformComponent;

        //-------------------------------------------------------------
        //! @brief  配置データを読む
        //! @param  path   [in]  BoardLayout.json のパス
        //! @param  layout [out] 読み込み先
        //! @return 読めたか
        //-------------------------------------------------------------
        bool LoadLayout(const std::string& path, Data::BoardLayoutData& layout) {
            std::ifstream stream(path);
            if(!stream.is_open()) {
                Tsukino::Core::Log::Error("Board layout not found: " + path + " (the board will be empty)");
                return false;
            }
            try {
                cereal::JSONInputArchive archive(stream);
                archive(cereal::make_nvp("BoardLayout", layout));
                return true;
            } catch(const std::exception& e) {
                Tsukino::Core::Log::Error("Failed to parse board layout: " + path + " (" + e.what() + ")");
                return false;
            }
        }

        //-------------------------------------------------------------
        //! @brief  Prefab を生成して盤面上の位置へ置く
        //! @param  factory    [in] Prefab の生成元
        //! @param  registry   [in] レジストリ
        //! @param  prefabPath [in] Prefab のパス
        //! @param  x          [in] 盤面上のX
        //! @param  y          [in] 盤面上のY
        //! @return 生成したエンティティ（失敗時は entt::null）
        //! @note   Z は Prefab の値のまま残す。盤面の手前・奥の並びは Prefab 側で決める
        //-------------------------------------------------------------
        entt::entity PlaceOnBoard(
            Tsukino::Engine::ECS::Prefab::PrefabFactory& factory, Tsukino::ECS::Registry& registry, const std::string& prefabPath, float x, float y) {
            const entt::entity entity = factory.Instantiate(prefabPath, registry);
            if(entity == entt::null)
                return entt::null;

            if(TransformComponent* transform = registry.try_get<TransformComponent>(entity)) {
                const float z       = transform->position.z;
                transform->position = hlslpp::float3(x, y, z);
                transform->dirty    = true;
            }
            return entity;
        }
    }    // namespace

    //-------------------------------------------------------------
    // 組み立て要求を処理する
    //-------------------------------------------------------------
    void BoardBuildSystem::Update(Tsukino::ECS::Registry& registry, float) {
        // 生成の途中で View が無効にならないよう、先に要求を集めておく
        std::vector<entt::entity> requests;
        for(const auto entity : registry.View<BoardBuildRequestComponent>()) {
            requests.push_back(entity);
        }
        if(requests.empty())
            return;

        Tsukino::EngineIntegration::EngineContext* context = GetEngineContext(registry);
        if(!context || !context->prefabFactory)
            return;
        Tsukino::Engine::ECS::Prefab::PrefabFactory& factory = *context->prefabFactory;

        for(const entt::entity requestEntity : requests) {
            const std::string layoutPath = registry.GetComponent<BoardBuildRequestComponent>(requestEntity).layoutPath;
            registry.QueueDestroy(requestEntity);

            Data::BoardLayoutData layout;
            if(!LoadLayout(layoutPath, layout))
                continue;

            for(const Data::BoardPointDef& pin : layout.pins) {
                PlaceOnBoard(factory, registry, layout.pinPrefab, pin.x, pin.y);
            }

            //-------------------------------------------------------------
            // 壁は1つの Prefab を伸ばして使う。見た目（scale）と当たり判定（extent）の
            // 両方を同じ大きさにする。奥行きは Prefab の値のまま
            //-------------------------------------------------------------
            for(const Data::BoardWallDef& wall : layout.walls) {
                const entt::entity entity = PlaceOnBoard(factory, registry, layout.wallPrefab, wall.x, wall.y);
                if(entity == entt::null)
                    continue;

                if(TransformComponent* transform = registry.try_get<TransformComponent>(entity)) {
                    const float depth   = transform->scale.z;
                    transform->rotation = hlslpp::quaternion::rotation_z(wall.angleDeg * std::numbers::pi_v<float> / 180.0f);
                    transform->scale    = hlslpp::float3(wall.halfWidth, wall.halfHeight, depth);
                }
                if(CollisionComponent* collision = registry.try_get<CollisionComponent>(entity)) {
                    const float depth = collision->extent.z;
                    collision->extent = hlslpp::float3(wall.halfWidth, wall.halfHeight, depth);
                }
            }

            for(const Data::BoardObjectDef& object : layout.objects) {
                PlaceOnBoard(factory, registry, object.prefab, object.x, object.y);
            }

            Tsukino::Core::Log::Info("Board built from " + layoutPath + ": " + std::to_string(layout.pins.size()) + " pins, "
                                     + std::to_string(layout.walls.size()) + " walls, " + std::to_string(layout.objects.size()) + " objects");
        }
    }

}    // namespace StardustFever::ECS

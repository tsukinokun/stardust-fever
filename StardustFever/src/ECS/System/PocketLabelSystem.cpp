//-------------------------------------------------------------
//! @file   PocketLabelSystem.cpp
//! @brief  PocketLabelSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/PocketLabelSystem.hpp>

#include <StardustFever/ECS/Component/PocketLabelComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/WorldAnchorComponent.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>

#include <algorithm>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    namespace {
        // 名札は盤面の飾りなので、HUD のパネルやトーストより下に描く
        constexpr int kLabelSortOrder = 3;
    }

    PocketLabelSystem::PocketLabelSystem(Tsukino::ECS::Scene& scene)
        : m_scene(&scene) {
    }

    //-------------------------------------------------------------
    // 名札を作り、持ち主が消えた名札を片付ける
    //-------------------------------------------------------------
    void PocketLabelSystem::Update(Tsukino::ECS::Registry& registry, float) {
        std::erase_if(m_labels, [&](const std::pair<entt::entity, entt::entity>& pair) {
            if(registry.IsValid(pair.first) && registry.HasComponent<PocketLabelComponent>(pair.first))
                return false;
            if(registry.IsValid(pair.second))
                registry.QueueDestroy(pair.second);
            return true;
        });

        for(const auto pocket : registry.View<PocketLabelComponent>()) {
            PocketLabelComponent& label = registry.GetComponent<PocketLabelComponent>(pocket);
            if((label.label != entt::null && registry.IsValid(label.label)) || label.text.empty())
                continue;

            //-------------------------------------------------------------
            // 入賞口の上に貼り付く文字を作る。真ん中揃えで、縁取りを付けて盤面の上でも読めるようにする
            //-------------------------------------------------------------
            const entt::entity entity = m_scene->CreateEntity();
            registry.AddComponent<Tsukino::BuiltIn::ECS::TransformComponent>(entity);
            auto& font           = registry.AddComponent<Tsukino::BuiltIn::ECS::FontComponent>(entity);
            font.scale           = label.scale;
            font.color           = label.color;
            font.outlineColor    = hlslpp::float4(0.0f, 0.0f, 0.05f, 0.9f);
            font.outlineWidth    = 1.2f;
            font.sortOrder       = kLabelSortOrder;
            font.horizontalAlign = Tsukino::BuiltIn::ECS::HorizontalAlign::Center;
            font.verticalAlign   = Tsukino::BuiltIn::ECS::VerticalAlign::Middle;
            auto& anchor         = registry.AddComponent<Tsukino::BuiltIn::ECS::WorldAnchorComponent>(entity);
            anchor.target        = pocket;
            anchor.worldOffset   = hlslpp::float3(0.0f, label.worldOffsetY, 0.0f);
            HudUi::SetText(registry, entity, label.text);

            label.label = entity;
            m_labels.emplace_back(pocket, entity);
        }
    }

}    // namespace StardustFever::ECS

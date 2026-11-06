//-------------------------------------------------------------
//! @file   UiButtonSystem.cpp
//! @brief  UiButtonSystem の実装
//-------------------------------------------------------------
#include <StardustFever/ECS/System/UiButtonSystem.hpp>

#include <StardustFever/ECS/Event/UiActionEvent.hpp>

#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    UiButtonSystem::UiButtonSystem(Tsukino::ECS::EventBus& eventBus)
        : m_eventBus(&eventBus) {
    }

    //-------------------------------------------------------------
    // 押された部品を知らせ、色を合わせる
    //-------------------------------------------------------------
    void UiButtonSystem::Update(Tsukino::ECS::Registry& registry, float) {
        using Tsukino::BuiltIn::ECS::PointerTargetComponent;
        using Tsukino::BuiltIn::ECS::SpriteComponent;
        for(const auto entity : registry.View<UiActionComponent, PointerTargetComponent>()) {
            const UiActionComponent&      button  = registry.GetComponent<UiActionComponent>(entity);
            const PointerTargetComponent& pointer = registry.GetComponent<PointerTargetComponent>(entity);
            if(pointer.clicked)
                m_eventBus->Publish(UiActionEvent{button.action, button.index});
            if(button.tinted) {
                if(SpriteComponent* sprite = registry.try_get<SpriteComponent>(entity))
                    sprite->tintColor = pointer.hovered ? button.hoverColor : button.baseColor;
            }
        }
    }

}    // namespace StardustFever::ECS

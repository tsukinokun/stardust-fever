//-------------------------------------------------------------
//! @file   UiParts.hpp
//! @brief  HUD に合わせた画面の部品を作る処理
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>
#include <StardustFever/ECS/Component/UiActionComponent.hpp>
#include <StardustFever/ECS/Utility/HudUi.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/PointerTargetComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIButtonFeedbackComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/UIRectComponent.hpp>
#include <Tsukino/Core/Path.hpp>
#include <Tsukino/Engine/Asset/AssetManager.hpp>
#include <Tsukino/Engine/ECS/Scene.hpp>

#include <string>

// 名前空間 : StardustFever::ECS::UiParts
namespace StardustFever::ECS::UiParts {

    inline constexpr float kPillBorder = 16.0f;    //!< pill.png の9スライスの幅
    inline constexpr float kCardBorder = 24.0f;    //!< panel_fill.png・panel_frame.png の9スライスの幅

    //-------------------------------------------------------------
    //! @brief  位置だけのエンティティを作る
    //! @param  scene    [in] シーン
    //! @param  registry [in] レジストリ
    //! @param  parent   [in] 親（無ければ entt::null）
    //! @param  x        [in] 親からのずれ
    //! @param  y        [in] 親からのずれ
    //! @return 作ったエンティティ
    //-------------------------------------------------------------
    inline entt::entity MakeNode(Tsukino::ECS::Scene& scene, Tsukino::ECS::Registry& registry, entt::entity parent, float x, float y) {
        const entt::entity                         entity    = scene.CreateEntity();
        Tsukino::BuiltIn::ECS::TransformComponent& transform = registry.AddComponent<Tsukino::BuiltIn::ECS::TransformComponent>(entity);
        if(parent != entt::null)
            transform.parent = parent;
        transform.position = hlslpp::float3(x, y, 0.0f);
        transform.dirty    = true;
        return entity;
    }

    //-------------------------------------------------------------
    //! @brief  画像を四角いっぱいに付ける（位置が四角の中心になる）
    //! @return 付けたスプライト
    //-------------------------------------------------------------
    inline Tsukino::BuiltIn::ECS::SpriteComponent& AddImage(Tsukino::ECS::Registry&     registry,
                                                            entt::entity                entity,
                                                            Tsukino::Asset::AssetHandle texture,
                                                            float                       width,
                                                            float                       height,
                                                            int                         sortOrder,
                                                            const hlslpp::float4&       color) {
        Tsukino::BuiltIn::ECS::SpriteComponent& sprite                             = registry.AddComponent<Tsukino::BuiltIn::ECS::SpriteComponent>(entity);
        sprite.textureHandle                                                       = texture;
        sprite.space                                                               = Tsukino::BuiltIn::ECS::SpriteSpace::Screen;
        sprite.sortOrder                                                           = sortOrder;
        sprite.tintColor                                                           = color;
        sprite.raycastTarget                                                       = false;
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIRectComponent>(entity).size = hlslpp::float2(width, height);
        return sprite;
    }

    //-------------------------------------------------------------
    //! @brief  9スライスの境界を設定する
    //! @param  sprite [in,out] スプライト
    //! @param  border [in] 画像の中の境界の幅
    //! @param  scale  [in] 画面での境界の倍率（小さな部品では角を小さくする）
    //-------------------------------------------------------------
    inline void SetSlice(Tsukino::BuiltIn::ECS::SpriteComponent& sprite, float border, float scale = 1.0f) {
        sprite.sliceBorder = hlslpp::float4(border, border, border, border);
        sprite.sliceScale  = scale;
    }

    //-------------------------------------------------------------
    //! @brief  文字を付ける（HUD の文字と同じ縁取りにする）
    //-------------------------------------------------------------
    inline void AddLabel(Tsukino::ECS::Registry& registry,
                         entt::entity            entity,
                         const std::string&      text,
                         float                   scale,
                         int                     sortOrder,
                         const hlslpp::float4&   color,
                         bool                    centered) {
        Tsukino::BuiltIn::ECS::FontComponent& font = registry.AddComponent<Tsukino::BuiltIn::ECS::FontComponent>(entity);
        font.scale                                 = scale;
        font.sortOrder                             = sortOrder;
        font.color                                 = color;
        font.outlineColor                          = hlslpp::float4(0.0f, 0.0f, 0.06f, 0.9f);
        font.outlineWidth                          = 1.8f;
        font.horizontalAlign                       = centered ? Tsukino::BuiltIn::ECS::HorizontalAlign::Center : Tsukino::BuiltIn::ECS::HorizontalAlign::Left;
        font.verticalAlign                         = Tsukino::BuiltIn::ECS::VerticalAlign::Middle;
        HudUi::SetText(registry, entity, text);
    }

    //-------------------------------------------------------------
    //! @brief  部品を押せるようにする
    //! @param  registry [in] レジストリ
    //! @param  entity   [in] スプライトを持つ部品
    //! @param  action   [in] 押したときの意味
    //! @param  index    [in] 意味ごとの番号
    //! @return 付けた部品の情報
    //-------------------------------------------------------------
    inline UiActionComponent& MakeClickable(Tsukino::ECS::Registry& registry, entt::entity entity, UiAction action, int index = 0) {
        registry.GetComponent<Tsukino::BuiltIn::ECS::SpriteComponent>(entity).raycastTarget = true;
        registry.AddComponent<Tsukino::BuiltIn::ECS::PointerTargetComponent>(entity);
        UiActionComponent& button = registry.AddComponent<UiActionComponent>(entity);
        button.action             = action;
        button.index              = index;
        return button;
    }

    //-------------------------------------------------------------
    //! @brief  カプセル形のボタンを作る
    //! @param  scene     [in] シーン
    //! @param  registry  [in] レジストリ
    //! @param  assets    [in] アセット
    //! @param  config    [in] 見た目
    //! @param  parent    [in] 親
    //! @param  x         [in] 中心の位置
    //! @param  y         [in] 中心の位置
    //! @param  width     [in] 幅
    //! @param  height    [in] 高さ
    //! @param  sortOrder [in] 描く順
    //! @param  text      [in] 文字
    //! @param  action    [in] 押したときの意味
    //! @param  index     [in] 意味ごとの番号
    //! @return 作ったボタン
    //-------------------------------------------------------------
    inline entt::entity MakeButton(Tsukino::ECS::Scene&           scene,
                                   Tsukino::ECS::Registry&        registry,
                                   Tsukino::Asset::AssetManager&  assets,
                                   const SettingsConfigComponent& config,
                                   entt::entity                   parent,
                                   float                          x,
                                   float                          y,
                                   float                          width,
                                   float                          height,
                                   int                            sortOrder,
                                   const std::string&             text,
                                   UiAction                       action,
                                   int                            index = 0) {
        const entt::entity button = MakeNode(scene, registry, parent, x, y);
        SetSlice(
            AddImage(registry, button, assets.Load(Tsukino::Core::Path(config.pillTexture)), width, height, sortOrder, config.buttonColor), kPillBorder, 0.8f);
        UiActionComponent& action_ = MakeClickable(registry, button, action, index);
        action_.tinted             = true;
        action_.baseColor          = config.buttonColor;
        action_.hoverColor         = config.hoverColor;
        registry.AddComponent<Tsukino::BuiltIn::ECS::UIButtonFeedbackComponent>(button);

        const entt::entity label = MakeNode(scene, registry, button, 0.0f, 0.0f);
        AddLabel(registry, label, text, config.textScale * 0.85f, sortOrder + 1, config.textColor, true);
        registry.GetComponent<UiActionComponent>(button).label = label;
        return button;
    }

    //-------------------------------------------------------------
    //! @brief  HUD と同じカードの面と縁を付ける
    //! @param  scene     [in] シーン
    //! @param  registry  [in] レジストリ
    //! @param  assets    [in] アセット
    //! @param  config    [in] 見た目
    //! @param  card      [in] カードのエンティティ（ここに面が付く）
    //! @param  width     [in] 幅
    //! @param  height    [in] 高さ
    //! @param  sortOrder [in] 描く順（縁は +1）
    //-------------------------------------------------------------
    inline void AddCard(Tsukino::ECS::Scene&           scene,
                        Tsukino::ECS::Registry&        registry,
                        Tsukino::Asset::AssetManager&  assets,
                        const SettingsConfigComponent& config,
                        entt::entity                   card,
                        float                          width,
                        float                          height,
                        int                            sortOrder) {
        SetSlice(AddImage(registry, card, assets.Load(Tsukino::Core::Path(config.panelTexture)), width, height, sortOrder, config.panelColor), kCardBorder);
        SetSlice(AddImage(registry,
                          MakeNode(scene, registry, card, 0.0f, 0.0f),
                          assets.Load(Tsukino::Core::Path(config.frameTexture)),
                          width,
                          height,
                          sortOrder + 1,
                          config.frameColor),
                 kCardBorder);
    }

}    // namespace StardustFever::ECS::UiParts

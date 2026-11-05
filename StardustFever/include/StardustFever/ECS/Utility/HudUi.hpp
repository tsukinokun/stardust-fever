//-------------------------------------------------------------
//! @file   HudUi.hpp
//! @brief  HUD の部品を探す・書き換える・動かすための小さな道具
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/TextUtility.hpp>
#include <StardustFever/ECS/Component/HudPartComponent.hpp>

#include <Tsukino/BuiltIn/ECS/Component/FontComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/SpriteComponent.hpp>
#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>
#include <Tsukino/BuiltIn/ECS/UI/UITweenUtility.hpp>
#include <Tsukino/Engine/ECS/Prefab/PrefabFactory.hpp>
#include <Tsukino/Core/ECS/Registry/Registry.hpp>

#include <cstdint>
#include <string>
#include <string_view>

// 名前空間 : StardustFever::ECS::HudUi
namespace StardustFever::ECS::HudUi {

    //-------------------------------------------------------------
    //! ゲームが使うトゥイーンの tag（エンジンの定型は 0xFFFFFF00 以上を使うので、それより小さくする）
    //-------------------------------------------------------------
    namespace Tag {
        inline constexpr std::uint32_t kIdleFloat   = 1;     //!< 待機中の上下の漂い
        inline constexpr std::uint32_t kIdleScale   = 2;     //!< 待機中の呼吸
        inline constexpr std::uint32_t kIdleAlpha   = 3;     //!< 待機中の明滅
        inline constexpr std::uint32_t kReelJitter  = 10;    //!< 回転中のリールの震え
        inline constexpr std::uint32_t kFlash       = 11;    //!< 一瞬光らせる（閃光・枠の点滅）
        inline constexpr std::uint32_t kRise        = 12;    //!< 浮かぶ数字の上昇
        inline constexpr std::uint32_t kAppearScale = 13;    //!< 出てくるときの拡大
        inline constexpr std::uint32_t kTilt        = 14;    //!< 傾き（揃いすぎて見えないように少し傾ける）
        inline constexpr std::uint32_t kDim         = 15;    //!< 買えない強化などを沈める
        inline constexpr std::uint32_t kBreath      = 16;    //!< 買える強化の縁の明滅
        inline constexpr std::uint32_t kPulse       = 17;    //!< 溜めている間のゲージの脈打ち
    }    // namespace Tag

    //-------------------------------------------------------------
    //! @brief  root の子孫から名前の付いた部品を探す
    //! @param  registry [in] レジストリ
    //! @param  root     [in] 探す範囲の根（自分自身も対象）
    //! @param  name     [in] HudPartComponent::name
    //! @return 見つかった部品（無ければ entt::null）
    //! @note   部品は数十個なので、毎回全体を見ても負荷は無視できる
    //-------------------------------------------------------------
    inline Tsukino::ECS::Entity FindPart(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity root, std::string_view name) {
        using Tsukino::BuiltIn::ECS::TransformComponent;
        for(const auto entity : registry.View<HudPartComponent>()) {
            if(registry.GetComponent<HudPartComponent>(entity).name != name)
                continue;
            Tsukino::ECS::Entity current = entity;
            for(int depth = 0; depth < 8 && current != entt::null; ++depth) {
                if(current == root)
                    return entity;
                const TransformComponent* transform = registry.try_get<TransformComponent>(current);
                if(!transform)
                    break;
                current = transform->parent;
            }
        }
        return entt::null;
    }

    //-------------------------------------------------------------
    //! @brief  名前の付いた部品を HUD 全体から探す（1つしか無い部品用）
    //! @param  registry [in] レジストリ
    //! @param  name     [in] HudPartComponent::name
    //! @return 見つかった部品（無ければ entt::null）
    //-------------------------------------------------------------
    inline Tsukino::ECS::Entity FindAnyPart(Tsukino::ECS::Registry& registry, std::string_view name) {
        for(const auto entity : registry.View<HudPartComponent>()) {
            if(registry.GetComponent<HudPartComponent>(entity).name == name)
                return entity;
        }
        return entt::null;
    }

    //-------------------------------------------------------------
    //! @brief  文字を書き換える（UTF-8 を受け取り、変わったときだけ変換する）
    //-------------------------------------------------------------
    inline void SetText(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, const std::string& utf8) {
        if(entity == entt::null)
            return;
        if(auto* font = registry.try_get<Tsukino::BuiltIn::ECS::FontComponent>(entity)) {
            std::wstring wide = Core::Utf8ToWide(utf8);
            if(font->text != wide)
                font->text = std::move(wide);
        }
    }

    //-------------------------------------------------------------
    //! @brief  文字の色を書き換える
    //-------------------------------------------------------------
    inline void SetTextColor(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, const hlslpp::float4& color) {
        if(entity == entt::null)
            return;
        if(auto* font = registry.try_get<Tsukino::BuiltIn::ECS::FontComponent>(entity))
            font->color = color;
    }

    //-------------------------------------------------------------
    //! @brief  スプライトの色を書き換える
    //-------------------------------------------------------------
    inline void SetTint(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, const hlslpp::float4& color) {
        if(entity == entt::null)
            return;
        if(auto* sprite = registry.try_get<Tsukino::BuiltIn::ECS::SpriteComponent>(entity))
            sprite->tintColor = color;
    }

    //-------------------------------------------------------------
    //! @brief  行って戻るを繰り返すトゥイーンを付ける（待機中のゆらぎ・明滅など）
    //! @param  period [in] 1往復の長さ（秒）
    //! @param  delay  [in] 始めるまでの待ち（秒）。位相をずらして、揃って動かないようにする
    //-------------------------------------------------------------
    inline void PlayPingPong(Tsukino::ECS::Registry&                registry,
                             Tsukino::ECS::Entity                   entity,
                             Tsukino::BuiltIn::ECS::UITweenProperty property,
                             float                                  from,
                             float                                  to,
                             float                                  period,
                             float                                  delay,
                             std::uint32_t                          tag) {
        Tsukino::BuiltIn::ECS::UITween tween;
        tween.property = property;
        tween.from     = from;
        tween.to       = to;
        tween.duration = period * 0.5f;
        tween.delay    = delay;
        tween.ease     = Tsukino::Core::Math::Easing::EaseType::InOutSine;
        tween.loop     = Tsukino::BuiltIn::ECS::UITweenLoop::PingPong;
        tween.tag      = tag;
        Tsukino::BuiltIn::ECS::UITweenUtility::Play(registry, entity, tween);
    }

    //-------------------------------------------------------------
    //! @brief  1回だけ動かして、終わりの値で止めるトゥイーンを付ける
    //-------------------------------------------------------------
    inline void PlayTo(Tsukino::ECS::Registry&                registry,
                       Tsukino::ECS::Entity                   entity,
                       Tsukino::BuiltIn::ECS::UITweenProperty property,
                       float                                  from,
                       float                                  to,
                       float                                  seconds,
                       Tsukino::Core::Math::Easing::EaseType  ease,
                       std::uint32_t                          tag,
                       float                                  delay = 0.0f) {
        Tsukino::BuiltIn::ECS::UITween tween;
        tween.property  = property;
        tween.from      = from;
        tween.to        = to;
        tween.duration  = seconds;
        tween.delay     = delay;
        tween.ease      = ease;
        tween.holdAtEnd = true;
        tween.tag       = tag;
        Tsukino::BuiltIn::ECS::UITweenUtility::Play(registry, entity, tween);
    }

    //-------------------------------------------------------------
    //! @brief  雛形のグループを生成し、"Root" という名前のエンティティを返す
    //! @param  factory   [in] Prefab の生成元
    //! @param  registry  [in] レジストリ
    //! @param  groupPath [in] グループ JSON のパス
    //! @return 根のエンティティ（失敗時は entt::null）
    //! @note   部品は TransformComponent.parent で根の子孫になっている。破棄は根ごと
    //!         （TransformUtility::DestroyHierarchy か LifetimeComponent）で行う
    //-------------------------------------------------------------
    inline Tsukino::ECS::Entity InstantiateTemplate(Tsukino::Engine::ECS::Prefab::PrefabFactory& factory,
                                                    Tsukino::ECS::Registry&                      registry,
                                                    const std::string&                           groupPath) {
        const auto instance = factory.InstantiateGroup(groupPath, registry);
        const auto root     = instance.find("Root");
        return root == instance.end() ? entt::null : root->second;
    }

    //-------------------------------------------------------------
    //! @brief  UI の部品を置く（Transform の位置を書き換えて行列を作り直させる）
    //-------------------------------------------------------------
    inline void SetPosition(Tsukino::ECS::Registry& registry, Tsukino::ECS::Entity entity, float x, float y) {
        if(auto* transform = registry.try_get<Tsukino::BuiltIn::ECS::TransformComponent>(entity)) {
            const float z       = transform->position.z;
            transform->position = hlslpp::float3(x, y, z);
            transform->dirty    = true;
        }
    }

}    // namespace StardustFever::ECS::HudUi

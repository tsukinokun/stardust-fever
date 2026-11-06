//-------------------------------------------------------------
//! @file   UpgradeTooltipSystem.hpp
//! @brief  強化のカードの吹き出しを描く System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>

// 名前空間 : Tsukino::ECS
namespace Tsukino::ECS {
    class Scene;
}    // namespace Tsukino::ECS

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  UpgradeTooltipSystem
    //! @brief  マウスが乗ったカードの横に、次のレベルの効果と値段を出す
    //-------------------------------------------------------------
    class UpgradeTooltipSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 部品を作るシーン
        //-------------------------------------------------------------
        explicit UpgradeTooltipSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  吹き出しを出し入れする
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  吹き出しを作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene* m_scene   = nullptr;
        entt::entity         m_root    = entt::null;                              //!< 吹き出し
        entt::entity         m_text[3] = {entt::null, entt::null, entt::null};    //!< 文字
    };
}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   BigBangDialogSystem.hpp
//! @brief  ビッグバンの確認画面を描く System
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
    //! @class  BigBangDialogSystem
    //! @brief  PrestigeStateComponent::confirmOpen の間だけ確認画面を出す
    //-------------------------------------------------------------
    class BigBangDialogSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 部品を作るシーン
        //-------------------------------------------------------------
        explicit BigBangDialogSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  確認画面を作り・片付ける
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  確認画面を作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene* m_scene = nullptr;
        entt::entity         m_root  = entt::null;    //!< 確認画面（開いている間だけある）
    };
}    // namespace StardustFever::ECS

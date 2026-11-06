//-------------------------------------------------------------
//! @file   ActionBarSystem.hpp
//! @brief  左下の操作ボタンを描く System
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
    //! @class  ActionBarSystem
    //! @brief  自動発射・星図・ビッグバンのボタンと、右のパネルの「そろったグループ」の当たりを作る
    //-------------------------------------------------------------
    class ActionBarSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 部品を作るシーン
        //-------------------------------------------------------------
        explicit ActionBarSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  ボタンを作り、今の状態に合わせる
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  ボタンを作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene* m_scene      = nullptr;
        entt::entity         m_autoFire   = entt::null;    //!< 自動発射のボタン
        entt::entity         m_bigBang    = entt::null;    //!< ビッグバンのボタン
        entt::entity         m_badge      = entt::null;    //!< 星図のボタンの赤い丸
        entt::entity         m_badgeText  = entt::null;    //!< 赤い丸の数
        int                  m_badgeCount = 0;             //!< 表示している数
    };
}    // namespace StardustFever::ECS

//-------------------------------------------------------------
//! @file   SettingsViewSystem.hpp
//! @brief  設定の画面を描く System
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
    //! @class  SettingsViewSystem
    //! @brief  「設定」のボタンと、開いている間だけの設定の画面を作り、値に合わせて動かす
    //! @note   値の正は AudioSettingsComponent と SettingsStateComponent。キーで音量を変えてもスライダーが付いてくる
    //-------------------------------------------------------------
    class SettingsViewSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 部品を作るシーン
        //-------------------------------------------------------------
        explicit SettingsViewSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  画面を作り・片付け、値に合わせて動かす
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  「設定」のボタンを作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void BuildOpenButton(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  設定の画面を作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  部品を今の値に合わせる
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Sync(Tsukino::ECS::Registry& registry, float deltaTime);

        Tsukino::ECS::Scene* m_scene      = nullptr;
        entt::entity         m_openButton = entt::null;    //!< 遊んでいる画面の「設定」のボタン
        entt::entity         m_root       = entt::null;    //!< 設定の画面（開いている間だけある）
    };
}    // namespace StardustFever::ECS

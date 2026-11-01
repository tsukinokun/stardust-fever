//-------------------------------------------------------------
//! @file   PachinkoScene.hpp
//! @brief  パチンコ台のシーン
//-------------------------------------------------------------
#pragma once
#include <Tsukino/EngineIntegration/Scene/GameSceneBase.hpp>

#include <memory>

namespace StardustFever::ECS {
    class SaveSystem;
}

// 名前空間 : StardustFever::Scene
namespace StardustFever::Scene {
    //-------------------------------------------------------------
    //! @class  PachinkoScene
    //! @brief  盤面・発射台・HUDを組み立て、ゲームのSystemを登録するシーン
    //! @note   シーンはエンティティとSystemを並べるだけで、ゲームのルールは持たない
    //-------------------------------------------------------------
    class PachinkoScene final : public Tsukino::EngineIntegration::GameSceneBase {
    public:
        PachinkoScene()           = default;
        ~PachinkoScene() override = default;

        //-------------------------------------------------------------
        //! @brief  シーンの更新
        //! @param  api       [in] エンジンAPI
        //! @param  deltaTime [in] 前フレームからの経過時間（秒）
        //-------------------------------------------------------------
        void OnUpdate(Tsukino::EngineIntegration::EngineAPI& api, float deltaTime) override;

        //-------------------------------------------------------------
        //! @brief  シーンの終了処理
        //-------------------------------------------------------------
        void OnExit() override;

    private:
        //-------------------------------------------------------------
        //! @brief  Systemの登録とエンティティの生成
        //! @param  api [in] エンジンAPI
        //-------------------------------------------------------------
        void OnInitialize(Tsukino::EngineIntegration::EngineAPI& api) override;

        //! 終了時に進行を保存するため、登録した SaveSystem を覚えておく（所有は Scene の SystemManager と共有）
        std::shared_ptr<ECS::SaveSystem> m_saveSystem;
    };
}    // namespace StardustFever::Scene

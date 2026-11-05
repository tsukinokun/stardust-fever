//-------------------------------------------------------------
//! @file   PocketLabelSystem.hpp
//! @brief  入賞口の上に名札を出す System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <utility>
#include <vector>

#include <entt/entt.hpp>

// 名前空間 : Tsukino::ECS
namespace Tsukino::ECS {
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  PocketLabelSystem
    //! @brief  PocketLabelComponent を持つ入賞口に、入賞口の色の文字で名札を作り、入賞口の上へ貼り付ける。
    //!         入賞口が消えたら名札も片付ける
    //! @note   名札は画面空間の文字で、エンジンの WorldAnchorSystem が入賞口の位置から画面の位置を決める
    //-------------------------------------------------------------
    class PocketLabelSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 名札のエンティティを作るシーン
        //-------------------------------------------------------------
        explicit PocketLabelSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  名札を作り、持ち主が消えた名札を片付ける
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        Tsukino::ECS::Scene*                               m_scene = nullptr;
        std::vector<std::pair<entt::entity, entt::entity>> m_labels;    //!< (入賞口, 名札)
    };
}    // namespace StardustFever::ECS

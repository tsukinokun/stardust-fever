//-------------------------------------------------------------
//! @file   BoardBuildSystem.hpp
//! @brief  配置データから盤面のエンティティを組み立てる System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  BoardBuildSystem
    //! @brief  BoardBuildRequestComponent を見つけたら、BoardLayout.json に従って
    //!         釘・壁・入賞口などの Prefab を並べ、要求のエンティティを消す
    //! @note   釘1本ごとに Prefab を作ると数百のファイルになるため、
    //!         Prefab は種類ごとに1つにして、位置だけを配置データで上書きする
    //-------------------------------------------------------------
    class BoardBuildSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  組み立て要求を処理する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;
    };
}    // namespace StardustFever::ECS

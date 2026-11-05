//-------------------------------------------------------------
//! @file   StarMapViewSystem.hpp
//! @brief  88星座の星図の画面を作り、並べ、閉じたら片付ける System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <entt/entt.hpp>

// 名前空間 : Tsukino::ECS
namespace Tsukino::ECS {
    class Scene;
}

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarMapViewSystem
    //! @brief  開いたら画面全体を覆う星図を作る。710の星を等級の大きさとスペクトル型の色の点で、
    //!         88星座の線と名前を並べ、右に選んだ星座の詳しい中身（効果・星の一覧）を出す。
    //!         持っている星は明るく、持っていない星は淡い点。星を全部そろえた星座は線と名前が金色になる。
    //!         G で右のパネルをグループの一覧に切り替え、選んだグループの星座を星図で目立たせる
    //! @note   操作は StarMapInputSystem が状態に書く。ここは状態の version が変わったときだけ並べ直す
    //-------------------------------------------------------------
    class StarMapViewSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  scene [in] 部品のエンティティを作るシーン
        //-------------------------------------------------------------
        explicit StarMapViewSystem(Tsukino::ECS::Scene& scene);

        //-------------------------------------------------------------
        //! @brief  開け閉めに合わせて作り・片付け、見え方が変わったら並べ直す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  画面の部品を全て作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  星・線・名前を今の見え方で並べ、詳しいパネルの中身を書く
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Layout(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene* m_scene          = nullptr;
        entt::entity         m_root           = entt::null;    //!< 画面全体の部品（開いている間だけある）
        int                  m_appliedVersion = -1;            //!< 最後に並べたときの状態の version
    };
}    // namespace StardustFever::ECS

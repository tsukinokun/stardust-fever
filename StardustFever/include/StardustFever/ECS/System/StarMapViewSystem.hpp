//-------------------------------------------------------------
//! @file   StarMapViewSystem.hpp
//! @brief  88星座の星図の画面を作り、並べ、閉じたら片付ける System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <Tsukino/BuiltIn/ECS/Component/TransformComponent.hpp>

#include <entt/entt.hpp>

#include <string>
#include <vector>

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
        //! 開け閉めの段階
        enum class Phase {
            Closed,       //!< 遊んでいる画面
            Darkening,    //!< 盤面から暗転している
            Revealing,    //!< 空が浮かび上がっている
            Shown,        //!< 空を見ている
            Hiding,       //!< 空から暗転している
            Returning,    //!< 盤面へ明けていく
        };

        //-------------------------------------------------------------
        //! @brief  暗転の幕を作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void MakeVeil(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  空へ切り替える（幕で隠れている間に呼ぶ）
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void EnterSky(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  盤面へ戻す（幕で隠れている間に呼ぶ）
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void ExitSky(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  カメラを今の向きへ合わせる
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void ApplyCamera(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  マウスの下の星の吹き出しを出す
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void UpdateTooltip(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  画面の部品を全て作る
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Build(Tsukino::ECS::Registry& registry);

        //-------------------------------------------------------------
        //! @brief  星・線・名前の色を今の状態にし、詳しいパネルの中身を書く
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Layout(Tsukino::ECS::Registry& registry);

        Tsukino::ECS::Scene*      m_scene          = nullptr;
        entt::entity              m_root           = entt::null;                              //!< 空の部品（空を見ている間だけある）
        entt::entity              m_veil           = entt::null;                              //!< 暗転の幕（開け閉めの間だけある）
        int                       m_appliedVersion = -1;                                      //!< 最後に並べたときの状態の version
        entt::entity              m_groupButton    = entt::null;                              //!< 星座とグループの切り替えのボタン
        entt::entity              m_objectButton   = entt::null;                              //!< 図鑑を切り替えるボタン
        entt::entity              m_reticleText    = entt::null;                              //!< 照準が指す星座の名前
        std::string               m_reticleId;                                                //!< 書いてある星座
        std::string               m_reticleSelected;                                          //!< 書いたときに選ばれていた星座
        entt::entity              m_tooltip        = entt::null;                              //!< 吹き出しの板
        entt::entity              m_tooltipText[3] = {entt::null, entt::null, entt::null};    //!< 吹き出しの文字
        int                       m_tooltipStar    = -1;                                      //!< 吹き出しに書いてある星
        Phase                     m_phase          = Phase::Closed;
        float                     m_veilAlpha      = 0.0f;       //!< 幕の濃さ
        std::vector<entt::entity> m_hiddenHud;                   //!< 空を見ている間だけ隠した HUD
        hlslpp::float3            m_boardCameraPosition;         //!< 戻す先のカメラの位置
        hlslpp::float3            m_boardCameraTarget;           //!< 戻す先のカメラの注視点
        float                     m_boardCameraFov   = 60.0f;    //!< 戻す先のカメラの視野
        bool                      m_particlesEnabled = false;    //!< 戻す先の漂う粒子の有効・無効
    };
}    // namespace StardustFever::ECS

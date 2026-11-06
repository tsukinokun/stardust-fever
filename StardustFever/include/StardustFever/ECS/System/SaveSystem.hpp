//-------------------------------------------------------------
//! @file   SaveSystem.hpp
//! @brief  進行の保存と読み込みを行う System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BigBangEvent.hpp>
#include <StardustFever/ECS/Event/StarDrawnEvent.hpp>
#include <StardustFever/ECS/Event/UpgradePurchasedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  SaveSystem
    //! @brief  最初の更新でセーブを読み、以後は一定間隔と大事な出来事（強化の購入・星座の完成・
    //!         ビッグバン）のたびに保存する
    //! @note   保存は一時ファイルに書いてから名前を変える。書いている途中で落ちても前のセーブが残る。
    //!         読めないセーブは消さずに .bad へ退避し、新しく始める（壊れたセーブで上書きして失わないため）
    //-------------------------------------------------------------
    class SaveSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 保存のきっかけになるイベントの購読元
        //-------------------------------------------------------------
        explicit SaveSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  読み込みと定期保存
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

        //-------------------------------------------------------------
        //! @brief  今の進行をすぐ保存する
        //! @param  registry [in] レジストリ
        //! @note   シーンの終了時にも呼ぶ。読み込む前（最初の更新より前）には保存しない
        //-------------------------------------------------------------
        void SaveNow(Tsukino::ECS::Registry& registry);

    private:
        //-------------------------------------------------------------
        //! @brief  セーブを読んでコンポーネントへ入れる
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Load(Tsukino::ECS::Registry& registry);

        EventQueue<UpgradePurchasedEvent> m_purchases;
        EventQueue<UpgradeHoldEndedEvent> m_holdEnds;
        EventQueue<StarDrawnEvent>        m_draws;
        EventQueue<BigBangEvent>          m_bigBangs;
        bool                              m_loaded        = false;    //!< 読み込みを終えたか
        float                             m_autoSaveTimer = 0.0f;     //!< 次の自動保存までの時間（秒）
    };
}    // namespace StardustFever::ECS

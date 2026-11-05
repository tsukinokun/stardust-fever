//-------------------------------------------------------------
//! @file   HudReactionSystem.hpp
//! @brief  大きな出来事に HUD 全体で反応する System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BigBangEvent.hpp>
#include <StardustFever/ECS/Event/FeverEndedEvent.hpp>
#include <StardustFever/ECS/Event/FeverStartedEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceEndedEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceStartedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  HudReactionSystem
    //! @brief  フィーバーの帯を上から滑り込ませ・引っ込め、残り時間が少なくなったら秒数を脈打たせる。
    //!         ビッグバンでは画面全体を白く光らせ、財布のカードを弾ませる
    //! @note   部品は HudPartComponent の名前（"FeverBanner" / "Flash" / "WalletCard"）で探す
    //-------------------------------------------------------------
    class HudReactionSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] フィーバー・ビッグバンの購読元
        //-------------------------------------------------------------
        explicit HudReactionSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  出来事に反応する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        EventQueue<FeverStartedEvent>      m_feverStarts;
        EventQueue<FeverEndedEvent>        m_feverEnds;
        EventQueue<StarChanceStartedEvent> m_chanceStarts;
        EventQueue<StarChanceEndedEvent>   m_chanceEnds;
        EventQueue<BigBangEvent>           m_bigBangs;
        bool                               m_initialized = false;    //!< 帯を隠し、閃光を透明にしたか
        float                              m_hideTimer   = -1.0f;    //!< 帯を隠すまでの残り時間（引っ込むトゥイーンの後。負なら隠さない）
        bool                               m_timeWarning = false;    //!< 残り時間の秒数を脈打たせているか
    };
}    // namespace StardustFever::ECS

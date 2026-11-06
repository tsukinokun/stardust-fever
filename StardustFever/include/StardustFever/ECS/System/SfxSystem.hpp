//-------------------------------------------------------------
//! @file   SfxSystem.hpp
//! @brief  ゲームの出来事に効果音を鳴らす System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Event/BallEnteredPocketEvent.hpp>
#include <StardustFever/ECS/Event/BallHitPinEvent.hpp>
#include <StardustFever/ECS/Event/BallLaunchedEvent.hpp>
#include <StardustFever/ECS/Event/BigBangEvent.hpp>
#include <StardustFever/ECS/Event/ConstellationCollectedEvent.hpp>
#include <StardustFever/ECS/Event/FeverEndedEvent.hpp>
#include <StardustFever/ECS/Event/LotteryResultEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceEndedEvent.hpp>
#include <StardustFever/ECS/Event/StarChanceStartedEvent.hpp>
#include <StardustFever/ECS/Event/StarDrawnEvent.hpp>
#include <StardustFever/ECS/Event/StarGateRollEvent.hpp>
#include <StardustFever/ECS/Event/StarGetFxEvent.hpp>
#include <StardustFever/ECS/Event/StarLinkedEvent.hpp>
#include <StardustFever/ECS/Event/UpgradeDeniedEvent.hpp>
#include <StardustFever/ECS/Event/UpgradePurchasedEvent.hpp>
#include <StardustFever/ECS/Utility/EventQueue.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>
#include <Tsukino/Core/Memory.hpp>
#include <Tsukino/Engine/Asset/AssetHandle.hpp>
#include <Tsukino/Engine/Asset/Audio/AudioAsset.hpp>

#include <array>
#include <future>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    struct AudioConfigComponent;

    //-------------------------------------------------------------
    //! @class  SfxSystem
    //! @brief  イベントと状態の変化を受けて、AudioConfigComponent::sounds の音を鳴らす
    //! @note   釘の音は玉が多いと連発するので、最短の間隔で間引く
    //-------------------------------------------------------------
    class SfxSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  コンストラクタ
        //! @param  eventBus [in] 出来事の購読元
        //-------------------------------------------------------------
        explicit SfxSystem(Tsukino::ECS::EventBus& eventBus);

        //-------------------------------------------------------------
        //! @brief  このフレームの出来事の音を鳴らす
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  まだ依頼していない音を裏で読み始め、読み終えた音を受け取る
        //! @param  registry [in] レジストリ
        //! @param  config   [in] 音の設定
        //-------------------------------------------------------------
        void UpdateLoading(Tsukino::ECS::Registry& registry, const AudioConfigComponent& config);

        //-------------------------------------------------------------
        //! @brief  音を鳴らす（読み終えていない音は飛ばす）
        //! @param  registry [in] レジストリ
        //! @param  config   [in] 音の設定
        //! @param  id       [in] 音の名前
        //! @param  variant  [in] 同じ名前の組の何番目か（負なら乱数で選ぶ。組より大きければ最後の音）
        //-------------------------------------------------------------
        void Play(Tsukino::ECS::Registry& registry, const AudioConfigComponent& config, std::string_view id, int variant = 0);

        EventQueue<BallLaunchedEvent>           m_launches;
        EventQueue<BallHitPinEvent>             m_pins;
        EventQueue<BallEnteredPocketEvent>      m_pockets;
        EventQueue<LotteryResultEvent>          m_lotteries;
        EventQueue<FeverEndedEvent>             m_feverEnds;
        EventQueue<StarChanceStartedEvent>      m_chanceStarts;
        EventQueue<StarChanceEndedEvent>        m_chanceEnds;
        EventQueue<StarLinkedEvent>             m_links;
        EventQueue<StarDrawnEvent>              m_draws;
        EventQueue<ConstellationCollectedEvent> m_constellations;
        EventQueue<BigBangEvent>                m_bigBangs;
        EventQueue<UpgradePurchasedEvent>       m_purchases;
        EventQueue<UpgradeDeniedEvent>          m_denials;
        EventQueue<StarGetFxEvent>              m_starFx;
        EventQueue<StarGateRollEvent>           m_gateRolls;

        std::unordered_map<std::string, std::shared_future<Tsukino::Asset::AssetHandle>> m_loading;               //!< 読んでいる音（パス → 結果）
        std::unordered_map<std::string, Tsukino::Core::Ref<Tsukino::Asset::AudioAsset>>  m_loaded;                //!< 読み終えた音（パス → 音。失敗は nullptr）
        float                                                                            m_pinCooldown = 0.0f;    //!< 次の釘の音までの時間（秒）
        int                                                                              m_linkCount   = 0;       //!< このチャンスでつないだ数（音階を上げる）
        bool                                                                             m_wasReach    = false;    //!< 前のフレームでリーチだったか
        bool                                                                             m_wasMapOpen  = false;    //!< 前のフレームで星図が開いていたか
        float               m_lastSfxLevel = -1.0f;                 //!< 前のフレームの効果音の音量（変えた手応えを鳴らす）
        std::array<bool, 3> m_wasStopped   = {true, true, true};    //!< 前のフレームで各リールが止まっていたか
        std::mt19937        m_random;
    };
}    // namespace StardustFever::ECS

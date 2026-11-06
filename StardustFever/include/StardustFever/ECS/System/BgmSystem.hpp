//-------------------------------------------------------------
//! @file   BgmSystem.hpp
//! @brief  場面に合わせて BGM を切り替える System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>
#include <Tsukino/Core/Memory.hpp>
#include <Tsukino/Engine/Asset/Audio/AudioAsset.hpp>

#include <array>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  BgmSystem
    //! @brief  今の場面を状態の Component から決め、曲の音量を動かしてクロスフェードする
    //! @note   イベントではなく状態を毎フレーム見る。星図を開くとフィーバーの時間が止まるなど、
    //!         場面が重なるため。星図を閉じれば、止まっていた場面の曲へ自然に戻る。
    //!         エンジンは初回の再生で音のバンクを開くので、切り替えの瞬間に引っかからないよう、
    //!         最初のフレームで全曲を音量0で鳴らして開いておく
    //-------------------------------------------------------------
    class BgmSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  場面を決め、曲を切り替える
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //! 場面。曲の添字を兼ねる
        enum Scene {
            kNormal,
            kReach,
            kFever,
            kChance,
            kMap,
            kSceneCount,
        };

        //-------------------------------------------------------------
        //! @struct Track
        //! @brief  場面ごとの曲
        //-------------------------------------------------------------
        struct Track {
            std::string                                    path;             //!< 読み込んだ曲のパス（設定が変わったら読み直す）
            Tsukino::Core::Ref<Tsukino::Asset::AudioAsset> asset;            //!< 曲
            float                                          volume = 0.0f;    //!< 今の音量
        };

        std::array<Track, kSceneCount> m_tracks;
        Scene                          m_scene = kNormal;    //!< 今の場面
    };
}    // namespace StardustFever::ECS

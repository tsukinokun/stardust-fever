//-------------------------------------------------------------
//! @file   AudioConfigComponentSerialization.hpp
//! @brief  AudioConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/AudioConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const SfxDef& sound) {
        archive(cereal::make_nvp("id", sound.id));
        archive(cereal::make_nvp("path", sound.path));
        archive(cereal::make_nvp("volume", sound.volume));
    }

    template <class Archive>
    void load(Archive& archive, SfxDef& sound) {
        Tsukino::IO::LoadOptional(archive, "id", sound.id);
        Tsukino::IO::LoadOptional(archive, "path", sound.path);
        Tsukino::IO::LoadOptional(archive, "volume", sound.volume);
    }

    template <class Archive>
    void save(Archive& archive, const AudioConfigComponent& component) {
        archive(cereal::make_nvp("bgmNormal", component.bgmNormal));
        archive(cereal::make_nvp("bgmReach", component.bgmReach));
        archive(cereal::make_nvp("bgmFever", component.bgmFever));
        archive(cereal::make_nvp("bgmChance", component.bgmChance));
        archive(cereal::make_nvp("bgmMap", component.bgmMap));
        archive(cereal::make_nvp("fadeSeconds", component.fadeSeconds));
        archive(cereal::make_nvp("fastFadeSeconds", component.fastFadeSeconds));
        archive(cereal::make_nvp("bgmBus", component.bgmBus));
        archive(cereal::make_nvp("sfxBus", component.sfxBus));
        archive(cereal::make_nvp("volumeStep", component.volumeStep));
        archive(cereal::make_nvp("pinMinInterval", component.pinMinInterval));
        archive(cereal::make_nvp("sounds", component.sounds));
    }

    template <class Archive>
    void load(Archive& archive, AudioConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "bgmNormal", component.bgmNormal);
        Tsukino::IO::LoadOptional(archive, "bgmReach", component.bgmReach);
        Tsukino::IO::LoadOptional(archive, "bgmFever", component.bgmFever);
        Tsukino::IO::LoadOptional(archive, "bgmChance", component.bgmChance);
        Tsukino::IO::LoadOptional(archive, "bgmMap", component.bgmMap);
        Tsukino::IO::LoadOptional(archive, "fadeSeconds", component.fadeSeconds);
        Tsukino::IO::LoadOptional(archive, "fastFadeSeconds", component.fastFadeSeconds);
        Tsukino::IO::LoadOptional(archive, "bgmBus", component.bgmBus);
        Tsukino::IO::LoadOptional(archive, "sfxBus", component.sfxBus);
        Tsukino::IO::LoadOptional(archive, "volumeStep", component.volumeStep);
        Tsukino::IO::LoadOptional(archive, "pinMinInterval", component.pinMinInterval);
        Tsukino::IO::LoadOptional(archive, "sounds", component.sounds);
    }

}    // namespace StardustFever::ECS

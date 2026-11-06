//-------------------------------------------------------------
//! @file   StarGetFxConfigComponentSerialization.hpp
//! @brief  StarGetFxConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/StarGetFxConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const StarGetFxConfigComponent& component) {
        archive(cereal::make_nvp("whiteTexture", component.whiteTexture));
        archive(cereal::make_nvp("starTexture", component.starTexture));
        archive(cereal::make_nvp("glowTexture", component.glowTexture));
        archive(cereal::make_nvp("flareTexture", component.flareTexture));
        archive(cereal::make_nvp("sortOrder", component.sortOrder));
        archive(cereal::make_nvp("appearSeconds", component.appearSeconds));
        archive(cereal::make_nvp("holdSeconds", component.holdSeconds));
        archive(cereal::make_nvp("flySeconds", component.flySeconds));
        archive(cereal::make_nvp("centerX", component.centerX));
        archive(cereal::make_nvp("centerY", component.centerY));
        archive(cereal::make_nvp("starSize", component.starSize));
        archive(cereal::make_nvp("flareSize", component.flareSize));
        archive(cereal::make_nvp("flashAlpha", component.flashAlpha));
        archive(cereal::make_nvp("sparkMin", component.sparkMin));
        archive(cereal::make_nvp("sparkMax", component.sparkMax));
        archive(cereal::make_nvp("sparkDistance", component.sparkDistance));
        archive(cereal::make_nvp("trailCount", component.trailCount));
        archive(cereal::make_nvp("textY", component.textY));
        archive(cereal::make_nvp("nameScale", component.nameScale));
        archive(cereal::make_nvp("tagScale", component.tagScale));
        archive(cereal::make_nvp("factsScale", component.factsScale));
        archive(cereal::make_nvp("walletX", component.walletX));
        archive(cereal::make_nvp("walletY", component.walletY));
        archive(cereal::make_nvp("mapCardX", component.mapCardX));
        archive(cereal::make_nvp("mapCardY", component.mapCardY));
        archive(cereal::make_nvp("mapCardWidth", component.mapCardWidth));
        archive(cereal::make_nvp("mapCardHeight", component.mapCardHeight));
        archive(cereal::make_nvp("mapLineColor", component.mapLineColor));
        archive(cereal::make_nvp("mapDimColor", component.mapDimColor));
        archive(cereal::make_nvp("tagColor", component.tagColor));
        archive(cereal::make_nvp("textColor", component.textColor));
    }

    template <class Archive>
    void load(Archive& archive, StarGetFxConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "whiteTexture", component.whiteTexture);
        Tsukino::IO::LoadOptional(archive, "starTexture", component.starTexture);
        Tsukino::IO::LoadOptional(archive, "glowTexture", component.glowTexture);
        Tsukino::IO::LoadOptional(archive, "flareTexture", component.flareTexture);
        Tsukino::IO::LoadOptional(archive, "sortOrder", component.sortOrder);
        Tsukino::IO::LoadOptional(archive, "appearSeconds", component.appearSeconds);
        Tsukino::IO::LoadOptional(archive, "holdSeconds", component.holdSeconds);
        Tsukino::IO::LoadOptional(archive, "flySeconds", component.flySeconds);
        Tsukino::IO::LoadOptional(archive, "centerX", component.centerX);
        Tsukino::IO::LoadOptional(archive, "centerY", component.centerY);
        Tsukino::IO::LoadOptional(archive, "starSize", component.starSize);
        Tsukino::IO::LoadOptional(archive, "flareSize", component.flareSize);
        Tsukino::IO::LoadOptional(archive, "flashAlpha", component.flashAlpha);
        Tsukino::IO::LoadOptional(archive, "sparkMin", component.sparkMin);
        Tsukino::IO::LoadOptional(archive, "sparkMax", component.sparkMax);
        Tsukino::IO::LoadOptional(archive, "sparkDistance", component.sparkDistance);
        Tsukino::IO::LoadOptional(archive, "trailCount", component.trailCount);
        Tsukino::IO::LoadOptional(archive, "textY", component.textY);
        Tsukino::IO::LoadOptional(archive, "nameScale", component.nameScale);
        Tsukino::IO::LoadOptional(archive, "tagScale", component.tagScale);
        Tsukino::IO::LoadOptional(archive, "factsScale", component.factsScale);
        Tsukino::IO::LoadOptional(archive, "walletX", component.walletX);
        Tsukino::IO::LoadOptional(archive, "walletY", component.walletY);
        Tsukino::IO::LoadOptional(archive, "mapCardX", component.mapCardX);
        Tsukino::IO::LoadOptional(archive, "mapCardY", component.mapCardY);
        Tsukino::IO::LoadOptional(archive, "mapCardWidth", component.mapCardWidth);
        Tsukino::IO::LoadOptional(archive, "mapCardHeight", component.mapCardHeight);
        Tsukino::IO::LoadOptional(archive, "mapLineColor", component.mapLineColor);
        Tsukino::IO::LoadOptional(archive, "mapDimColor", component.mapDimColor);
        Tsukino::IO::LoadOptional(archive, "tagColor", component.tagColor);
        Tsukino::IO::LoadOptional(archive, "textColor", component.textColor);
    }

}    // namespace StardustFever::ECS

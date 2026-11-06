//-------------------------------------------------------------
//! @file   SettingsConfigComponentSerialization.hpp
//! @brief  SettingsConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/SettingsConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const SettingsConfigComponent& component) {
        archive(cereal::make_nvp("whiteTexture", component.whiteTexture));
        archive(cereal::make_nvp("whitePixels", component.whitePixels));
        archive(cereal::make_nvp("sortOrder", component.sortOrder));
        archive(cereal::make_nvp("panelWidth", component.panelWidth));
        archive(cereal::make_nvp("panelHeight", component.panelHeight));
        archive(cereal::make_nvp("rowStartY", component.rowStartY));
        archive(cereal::make_nvp("rowSpacing", component.rowSpacing));
        archive(cereal::make_nvp("labelX", component.labelX));
        archive(cereal::make_nvp("controlX", component.controlX));
        archive(cereal::make_nvp("trackWidth", component.trackWidth));
        archive(cereal::make_nvp("titleScale", component.titleScale));
        archive(cereal::make_nvp("textScale", component.textScale));
        archive(cereal::make_nvp("buttonWidth", component.buttonWidth));
        archive(cereal::make_nvp("buttonHeight", component.buttonHeight));
        archive(cereal::make_nvp("openButtonX", component.openButtonX));
        archive(cereal::make_nvp("openButtonY", component.openButtonY));
        archive(cereal::make_nvp("dimColor", component.dimColor));
        archive(cereal::make_nvp("panelColor", component.panelColor));
        archive(cereal::make_nvp("trackColor", component.trackColor));
        archive(cereal::make_nvp("fillColor", component.fillColor));
        archive(cereal::make_nvp("onColor", component.onColor));
        archive(cereal::make_nvp("offColor", component.offColor));
        archive(cereal::make_nvp("hoverColor", component.hoverColor));
        archive(cereal::make_nvp("textColor", component.textColor));
    }

    template <class Archive>
    void load(Archive& archive, SettingsConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "whiteTexture", component.whiteTexture);
        Tsukino::IO::LoadOptional(archive, "whitePixels", component.whitePixels);
        Tsukino::IO::LoadOptional(archive, "sortOrder", component.sortOrder);
        Tsukino::IO::LoadOptional(archive, "panelWidth", component.panelWidth);
        Tsukino::IO::LoadOptional(archive, "panelHeight", component.panelHeight);
        Tsukino::IO::LoadOptional(archive, "rowStartY", component.rowStartY);
        Tsukino::IO::LoadOptional(archive, "rowSpacing", component.rowSpacing);
        Tsukino::IO::LoadOptional(archive, "labelX", component.labelX);
        Tsukino::IO::LoadOptional(archive, "controlX", component.controlX);
        Tsukino::IO::LoadOptional(archive, "trackWidth", component.trackWidth);
        Tsukino::IO::LoadOptional(archive, "titleScale", component.titleScale);
        Tsukino::IO::LoadOptional(archive, "textScale", component.textScale);
        Tsukino::IO::LoadOptional(archive, "buttonWidth", component.buttonWidth);
        Tsukino::IO::LoadOptional(archive, "buttonHeight", component.buttonHeight);
        Tsukino::IO::LoadOptional(archive, "openButtonX", component.openButtonX);
        Tsukino::IO::LoadOptional(archive, "openButtonY", component.openButtonY);
        Tsukino::IO::LoadOptional(archive, "dimColor", component.dimColor);
        Tsukino::IO::LoadOptional(archive, "panelColor", component.panelColor);
        Tsukino::IO::LoadOptional(archive, "trackColor", component.trackColor);
        Tsukino::IO::LoadOptional(archive, "fillColor", component.fillColor);
        Tsukino::IO::LoadOptional(archive, "onColor", component.onColor);
        Tsukino::IO::LoadOptional(archive, "offColor", component.offColor);
        Tsukino::IO::LoadOptional(archive, "hoverColor", component.hoverColor);
        Tsukino::IO::LoadOptional(archive, "textColor", component.textColor);
    }

}    // namespace StardustFever::ECS

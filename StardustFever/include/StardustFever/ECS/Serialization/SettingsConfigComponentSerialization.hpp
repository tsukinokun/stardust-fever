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
        archive(cereal::make_nvp("panelTexture", component.panelTexture));
        archive(cereal::make_nvp("frameTexture", component.frameTexture));
        archive(cereal::make_nvp("pillTexture", component.pillTexture));
        archive(cereal::make_nvp("barTexture", component.barTexture));
        archive(cereal::make_nvp("starTexture", component.starTexture));
        archive(cereal::make_nvp("starOutlineTexture", component.starOutlineTexture));
        archive(cereal::make_nvp("glowTexture", component.glowTexture));
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
        archive(cereal::make_nvp("switchWidth", component.switchWidth));
        archive(cereal::make_nvp("switchHeight", component.switchHeight));
        archive(cereal::make_nvp("closeButtonWidth", component.closeButtonWidth));
        archive(cereal::make_nvp("closeButtonHeight", component.closeButtonHeight));
        archive(cereal::make_nvp("openButtonX", component.openButtonX));
        archive(cereal::make_nvp("openButtonY", component.openButtonY));
        archive(cereal::make_nvp("openButtonWidth", component.openButtonWidth));
        archive(cereal::make_nvp("openButtonHeight", component.openButtonHeight));
        archive(cereal::make_nvp("actionBarX", component.actionBarX));
        archive(cereal::make_nvp("actionBarY", component.actionBarY));
        archive(cereal::make_nvp("actionButtonWidth", component.actionButtonWidth));
        archive(cereal::make_nvp("actionButtonHeight", component.actionButtonHeight));
        archive(cereal::make_nvp("actionButtonGap", component.actionButtonGap));
        archive(cereal::make_nvp("tooltipX", component.tooltipX));
        archive(cereal::make_nvp("tooltipWidth", component.tooltipWidth));
        archive(cereal::make_nvp("tooltipHeight", component.tooltipHeight));
        archive(cereal::make_nvp("badgeColor", component.badgeColor));
        archive(cereal::make_nvp("dimColor", component.dimColor));
        archive(cereal::make_nvp("panelColor", component.panelColor));
        archive(cereal::make_nvp("frameColor", component.frameColor));
        archive(cereal::make_nvp("glowColor", component.glowColor));
        archive(cereal::make_nvp("accentColor", component.accentColor));
        archive(cereal::make_nvp("trackColor", component.trackColor));
        archive(cereal::make_nvp("fillColor", component.fillColor));
        archive(cereal::make_nvp("onColor", component.onColor));
        archive(cereal::make_nvp("buttonColor", component.buttonColor));
        archive(cereal::make_nvp("hoverColor", component.hoverColor));
        archive(cereal::make_nvp("textColor", component.textColor));
        archive(cereal::make_nvp("titleColor", component.titleColor));
    }

    template <class Archive>
    void load(Archive& archive, SettingsConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "whiteTexture", component.whiteTexture);
        Tsukino::IO::LoadOptional(archive, "panelTexture", component.panelTexture);
        Tsukino::IO::LoadOptional(archive, "frameTexture", component.frameTexture);
        Tsukino::IO::LoadOptional(archive, "pillTexture", component.pillTexture);
        Tsukino::IO::LoadOptional(archive, "barTexture", component.barTexture);
        Tsukino::IO::LoadOptional(archive, "starTexture", component.starTexture);
        Tsukino::IO::LoadOptional(archive, "starOutlineTexture", component.starOutlineTexture);
        Tsukino::IO::LoadOptional(archive, "glowTexture", component.glowTexture);
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
        Tsukino::IO::LoadOptional(archive, "switchWidth", component.switchWidth);
        Tsukino::IO::LoadOptional(archive, "switchHeight", component.switchHeight);
        Tsukino::IO::LoadOptional(archive, "closeButtonWidth", component.closeButtonWidth);
        Tsukino::IO::LoadOptional(archive, "closeButtonHeight", component.closeButtonHeight);
        Tsukino::IO::LoadOptional(archive, "openButtonX", component.openButtonX);
        Tsukino::IO::LoadOptional(archive, "openButtonY", component.openButtonY);
        Tsukino::IO::LoadOptional(archive, "openButtonWidth", component.openButtonWidth);
        Tsukino::IO::LoadOptional(archive, "openButtonHeight", component.openButtonHeight);
        Tsukino::IO::LoadOptional(archive, "actionBarX", component.actionBarX);
        Tsukino::IO::LoadOptional(archive, "actionBarY", component.actionBarY);
        Tsukino::IO::LoadOptional(archive, "actionButtonWidth", component.actionButtonWidth);
        Tsukino::IO::LoadOptional(archive, "actionButtonHeight", component.actionButtonHeight);
        Tsukino::IO::LoadOptional(archive, "actionButtonGap", component.actionButtonGap);
        Tsukino::IO::LoadOptional(archive, "tooltipX", component.tooltipX);
        Tsukino::IO::LoadOptional(archive, "tooltipWidth", component.tooltipWidth);
        Tsukino::IO::LoadOptional(archive, "tooltipHeight", component.tooltipHeight);
        Tsukino::IO::LoadOptional(archive, "badgeColor", component.badgeColor);
        Tsukino::IO::LoadOptional(archive, "dimColor", component.dimColor);
        Tsukino::IO::LoadOptional(archive, "panelColor", component.panelColor);
        Tsukino::IO::LoadOptional(archive, "frameColor", component.frameColor);
        Tsukino::IO::LoadOptional(archive, "glowColor", component.glowColor);
        Tsukino::IO::LoadOptional(archive, "accentColor", component.accentColor);
        Tsukino::IO::LoadOptional(archive, "trackColor", component.trackColor);
        Tsukino::IO::LoadOptional(archive, "fillColor", component.fillColor);
        Tsukino::IO::LoadOptional(archive, "onColor", component.onColor);
        Tsukino::IO::LoadOptional(archive, "buttonColor", component.buttonColor);
        Tsukino::IO::LoadOptional(archive, "hoverColor", component.hoverColor);
        Tsukino::IO::LoadOptional(archive, "textColor", component.textColor);
        Tsukino::IO::LoadOptional(archive, "titleColor", component.titleColor);
    }

}    // namespace StardustFever::ECS

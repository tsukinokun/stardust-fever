//-------------------------------------------------------------
//! @file   StarMapConfigComponentSerialization.hpp
//! @brief  StarMapConfigComponent の cereal シリアライズ定義
//! @note   全ての項目を省略可能にしてある（書かれていない項目は既定値のまま）。
//!         項目を足しても既存のJSONを書き直さずに済み、ホットリロードで
//!         一部だけ書き換えた途中の状態でも読み込みが止まらないようにするため
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/StarMapConfigComponent.hpp>

#include <cereal/types/string.hpp>
#include <Tsukino/Core/Math/Serialization/HlslppSerialization.hpp>
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    template <class Archive>
    void save(Archive& archive, const StarMapConfigComponent& component) {
        archive(cereal::make_nvp("backgroundTexture", component.backgroundTexture));
        archive(cereal::make_nvp("backgroundPixels", component.backgroundPixels));
        archive(cereal::make_nvp("starTexture", component.starTexture));
        archive(cereal::make_nvp("starPixels", component.starPixels));
        archive(cereal::make_nvp("backgroundColor", component.backgroundColor));
        archive(cereal::make_nvp("mapColor", component.mapColor));
        archive(cereal::make_nvp("panelColor", component.panelColor));
        archive(cereal::make_nvp("mapX", component.mapX));
        archive(cereal::make_nvp("mapY", component.mapY));
        archive(cereal::make_nvp("mapWidth", component.mapWidth));
        archive(cereal::make_nvp("mapHeight", component.mapHeight));
        archive(cereal::make_nvp("pixelsPerDegree", component.pixelsPerDegree));
        archive(cereal::make_nvp("minZoom", component.minZoom));
        archive(cereal::make_nvp("maxZoom", component.maxZoom));
        archive(cereal::make_nvp("zoomStep", component.zoomStep));
        archive(cereal::make_nvp("keyZoomSpeed", component.keyZoomSpeed));
        archive(cereal::make_nvp("keyPanSpeed", component.keyPanSpeed));
        archive(cereal::make_nvp("maxDec", component.maxDec));
        archive(cereal::make_nvp("brightStarSize", component.brightStarSize));
        archive(cereal::make_nvp("faintStarSize", component.faintStarSize));
        archive(cereal::make_nvp("starZoomGrowth", component.starZoomGrowth));
        archive(cereal::make_nvp("missingStarColor", component.missingStarColor));
        archive(cereal::make_nvp("lineColor", component.lineColor));
        archive(cereal::make_nvp("selectedLineColor", component.selectedLineColor));
        archive(cereal::make_nvp("collectedLineColor", component.collectedLineColor));
        archive(cereal::make_nvp("lineThickness", component.lineThickness));
        archive(cereal::make_nvp("collectedLineThickness", component.collectedLineThickness));
        archive(cereal::make_nvp("labelColor", component.labelColor));
        archive(cereal::make_nvp("collectedLabelColor", component.collectedLabelColor));
        archive(cereal::make_nvp("selectedLabelColor", component.selectedLabelColor));
        archive(cereal::make_nvp("labelScale", component.labelScale));
        archive(cereal::make_nvp("labelMinZoom", component.labelMinZoom));
        archive(cereal::make_nvp("panelX", component.panelX));
        archive(cereal::make_nvp("panelWidth", component.panelWidth));
        archive(cereal::make_nvp("titleScale", component.titleScale));
        archive(cereal::make_nvp("textScale", component.textScale));
        archive(cereal::make_nvp("rowScale", component.rowScale));
        archive(cereal::make_nvp("rowSpacing", component.rowSpacing));
        archive(cereal::make_nvp("maxRows", component.maxRows));
        archive(cereal::make_nvp("clickRadius", component.clickRadius));
        archive(cereal::make_nvp("dragThreshold", component.dragThreshold));
        archive(cereal::make_nvp("sortOrder", component.sortOrder));
    }

    template <class Archive>
    void load(Archive& archive, StarMapConfigComponent& component) {
        Tsukino::IO::LoadOptional(archive, "backgroundTexture", component.backgroundTexture);
        Tsukino::IO::LoadOptional(archive, "backgroundPixels", component.backgroundPixels);
        Tsukino::IO::LoadOptional(archive, "starTexture", component.starTexture);
        Tsukino::IO::LoadOptional(archive, "starPixels", component.starPixels);
        Tsukino::IO::LoadOptional(archive, "backgroundColor", component.backgroundColor);
        Tsukino::IO::LoadOptional(archive, "mapColor", component.mapColor);
        Tsukino::IO::LoadOptional(archive, "panelColor", component.panelColor);
        Tsukino::IO::LoadOptional(archive, "mapX", component.mapX);
        Tsukino::IO::LoadOptional(archive, "mapY", component.mapY);
        Tsukino::IO::LoadOptional(archive, "mapWidth", component.mapWidth);
        Tsukino::IO::LoadOptional(archive, "mapHeight", component.mapHeight);
        Tsukino::IO::LoadOptional(archive, "pixelsPerDegree", component.pixelsPerDegree);
        Tsukino::IO::LoadOptional(archive, "minZoom", component.minZoom);
        Tsukino::IO::LoadOptional(archive, "maxZoom", component.maxZoom);
        Tsukino::IO::LoadOptional(archive, "zoomStep", component.zoomStep);
        Tsukino::IO::LoadOptional(archive, "keyZoomSpeed", component.keyZoomSpeed);
        Tsukino::IO::LoadOptional(archive, "keyPanSpeed", component.keyPanSpeed);
        Tsukino::IO::LoadOptional(archive, "maxDec", component.maxDec);
        Tsukino::IO::LoadOptional(archive, "brightStarSize", component.brightStarSize);
        Tsukino::IO::LoadOptional(archive, "faintStarSize", component.faintStarSize);
        Tsukino::IO::LoadOptional(archive, "starZoomGrowth", component.starZoomGrowth);
        Tsukino::IO::LoadOptional(archive, "missingStarColor", component.missingStarColor);
        Tsukino::IO::LoadOptional(archive, "lineColor", component.lineColor);
        Tsukino::IO::LoadOptional(archive, "selectedLineColor", component.selectedLineColor);
        Tsukino::IO::LoadOptional(archive, "collectedLineColor", component.collectedLineColor);
        Tsukino::IO::LoadOptional(archive, "lineThickness", component.lineThickness);
        Tsukino::IO::LoadOptional(archive, "collectedLineThickness", component.collectedLineThickness);
        Tsukino::IO::LoadOptional(archive, "labelColor", component.labelColor);
        Tsukino::IO::LoadOptional(archive, "collectedLabelColor", component.collectedLabelColor);
        Tsukino::IO::LoadOptional(archive, "selectedLabelColor", component.selectedLabelColor);
        Tsukino::IO::LoadOptional(archive, "labelScale", component.labelScale);
        Tsukino::IO::LoadOptional(archive, "labelMinZoom", component.labelMinZoom);
        Tsukino::IO::LoadOptional(archive, "panelX", component.panelX);
        Tsukino::IO::LoadOptional(archive, "panelWidth", component.panelWidth);
        Tsukino::IO::LoadOptional(archive, "titleScale", component.titleScale);
        Tsukino::IO::LoadOptional(archive, "textScale", component.textScale);
        Tsukino::IO::LoadOptional(archive, "rowScale", component.rowScale);
        Tsukino::IO::LoadOptional(archive, "rowSpacing", component.rowSpacing);
        Tsukino::IO::LoadOptional(archive, "maxRows", component.maxRows);
        Tsukino::IO::LoadOptional(archive, "clickRadius", component.clickRadius);
        Tsukino::IO::LoadOptional(archive, "dragThreshold", component.dragThreshold);
        Tsukino::IO::LoadOptional(archive, "sortOrder", component.sortOrder);
    }

}    // namespace StardustFever::ECS

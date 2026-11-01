//-------------------------------------------------------------
//! @file   BoardLayoutData.hpp
//! @brief  盤面の配置データ（Assets/Data/BoardLayout.json）
//! @note   釘の位置は Tools/GenerateBoardLayout.py が生成する
//-------------------------------------------------------------
#pragma once
#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct BoardPointDef
    //! @brief  位置だけを持つ配置（釘）
    //-------------------------------------------------------------
    struct BoardPointDef {
        float x = 0.0f;
        float y = 0.0f;
    };

    //-------------------------------------------------------------
    //! @struct BoardWallDef
    //! @brief  壁1枚。Wall Prefab の大きさと向きを上書きして置く
    //-------------------------------------------------------------
    struct BoardWallDef {
        float x          = 0.0f;    //!< 中心X
        float y          = 0.0f;    //!< 中心Y
        float halfWidth  = 1.0f;    //!< 長さの半分
        float halfHeight = 0.5f;    //!< 厚みの半分
        float angleDeg   = 0.0f;    //!< Z軸回りの回転（度）
    };

    //-------------------------------------------------------------
    //! @struct BoardObjectDef
    //! @brief  任意のPrefabを位置だけ上書きして置く（入賞口・アウト口・発射台など）
    //-------------------------------------------------------------
    struct BoardObjectDef {
        std::string prefab;    //!< Prefab（目次）JSONのパス
        float       x = 0.0f;
        float       y = 0.0f;
    };

    //-------------------------------------------------------------
    //! @struct BoardLayoutData
    //! @brief  盤面1枚分の配置
    //-------------------------------------------------------------
    struct BoardLayoutData {
        std::string                 pinPrefab;     //!< 釘のPrefab
        std::string                 wallPrefab;    //!< 壁のPrefab
        std::vector<BoardPointDef>  pins;
        std::vector<BoardWallDef>   walls;
        std::vector<BoardObjectDef> objects;
    };

    template <class Archive>
    void serialize(Archive& archive, BoardPointDef& point) {
        archive(cereal::make_nvp("x", point.x), cereal::make_nvp("y", point.y));
    }

    template <class Archive>
    void serialize(Archive& archive, BoardWallDef& wall) {
        archive(cereal::make_nvp("x", wall.x),
                cereal::make_nvp("y", wall.y),
                cereal::make_nvp("halfWidth", wall.halfWidth),
                cereal::make_nvp("halfHeight", wall.halfHeight),
                cereal::make_nvp("angleDeg", wall.angleDeg));
    }

    template <class Archive>
    void serialize(Archive& archive, BoardObjectDef& object) {
        archive(cereal::make_nvp("prefab", object.prefab), cereal::make_nvp("x", object.x), cereal::make_nvp("y", object.y));
    }

    template <class Archive>
    void serialize(Archive& archive, BoardLayoutData& layout) {
        archive(cereal::make_nvp("pinPrefab", layout.pinPrefab),
                cereal::make_nvp("wallPrefab", layout.wallPrefab),
                cereal::make_nvp("pins", layout.pins),
                cereal::make_nvp("walls", layout.walls),
                cereal::make_nvp("objects", layout.objects));
    }

}    // namespace StardustFever::Data

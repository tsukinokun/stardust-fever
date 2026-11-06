//-------------------------------------------------------------
//! @file   SkyObjectData.hpp
//! @brief  ビッグバンで手に入る天体のマスターデータ（Assets/Data/SkyObjects.json）
//! @note   Tools/BuildSkyObjects.py が作る
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Data/ConstellationData.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct SkyObjectDef
    //! @brief  天体1つの定義
    //-------------------------------------------------------------
    struct SkyObjectDef {
        std::string            id;                    //!< 参照用のID
        std::string            name;                  //!< 表示名（UTF-8）
        std::string            nameEn;                //!< 英語の名前
        std::string            kind      = "star";    //!< 種類（star / cluster / nebula / galaxy / other）
        double                 ra        = 0.0;       //!< 赤経（時）
        double                 dec       = 0.0;       //!< 赤緯（度）
        double                 magnitude = 5.0;       //!< 見かけの等級
        int                    rarity    = 0;         //!< レア度（0 がコモン）
        std::string            caption;               //!< 図鑑の文章
        std::string            texture;               //!< 絵のパス
        float                  size = 44.0f;          //!< 星図での大きさ（UI のピクセル）
        ConstellationEffectDef effect;                //!< 持っている間の効果
    };

    //-------------------------------------------------------------
    //! @struct SkyObjectTable
    //! @brief  天体の一覧
    //-------------------------------------------------------------
    struct SkyObjectTable {
        std::vector<SkyObjectDef> objects;

        //-------------------------------------------------------------
        //! @brief  IDで天体を探す
        //! @param  id [in] 天体のID
        //! @return 見つかった天体（無ければ nullptr）
        //-------------------------------------------------------------
        [[nodiscard]] const SkyObjectDef* Find(const std::string& id) const {
            for(const SkyObjectDef& object : objects) {
                if(object.id == id)
                    return &object;
            }
            return nullptr;
        }
    };

    template <class Archive>
    void serialize(Archive& archive, SkyObjectDef& object) {
        archive(cereal::make_nvp("id", object.id), cereal::make_nvp("name", object.name));
        Tsukino::IO::LoadOptional(archive, "nameEn", object.nameEn);
        Tsukino::IO::LoadOptional(archive, "kind", object.kind);
        Tsukino::IO::LoadOptional(archive, "ra", object.ra);
        Tsukino::IO::LoadOptional(archive, "dec", object.dec);
        Tsukino::IO::LoadOptional(archive, "magnitude", object.magnitude);
        Tsukino::IO::LoadOptional(archive, "rarity", object.rarity);
        Tsukino::IO::LoadOptional(archive, "caption", object.caption);
        Tsukino::IO::LoadOptional(archive, "texture", object.texture);
        Tsukino::IO::LoadOptional(archive, "size", object.size);
        Tsukino::IO::LoadOptional(archive, "effect", object.effect);
    }

    template <class Archive>
    void serialize(Archive& archive, SkyObjectTable& table) {
        archive(cereal::make_nvp("objects", table.objects));
    }
}    // namespace StardustFever::Data

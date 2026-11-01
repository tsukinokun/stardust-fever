//-------------------------------------------------------------
//! @file   ConstellationData.hpp
//! @brief  星座のマスターデータ（Assets/Data/Constellations.json）
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct ConstellationStarDef
    //! @brief  星座を構成する星1つと、フィーバー中に盤面へ出す位置
    //! @note   x / y は Tools/GenerateConstellations.py が ra / dec から計算し、役物と重ならないことを確かめて書く。
    //!         手で書き換えると役物と重なり、玉が先に入賞してつなげない星ができるので注意
    //-------------------------------------------------------------
    struct ConstellationStarDef {
        std::string starId;        //!< StarDef::id
        double      ra  = 0.0;     //!< 赤経（時）。盤面の位置の計算元で、実行時は使わない
        double      dec = 0.0;     //!< 赤緯（度）。盤面の位置の計算元で、実行時は使わない
        float       x   = 0.0f;    //!< 盤面上のX（ワールド単位）
        float       y   = 0.0f;    //!< 盤面上のY（ワールド単位）
    };

    //-------------------------------------------------------------
    //! @struct ConstellationLineDef
    //! @brief  星座の線。両端の星がつながると光の線が引かれる
    //-------------------------------------------------------------
    struct ConstellationLineDef {
        int from = 0;    //!< stars の添字
        int to   = 0;    //!< stars の添字
    };

    //-------------------------------------------------------------
    //! @struct ConstellationDef
    //! @brief  星座1つの定義
    //-------------------------------------------------------------
    struct ConstellationDef {
        std::string                       id;                                //!< 参照用のID（例: "orion"）
        std::string                       name;                              //!< 表示名（UTF-8）
        std::vector<ConstellationStarDef> stars;                             //!< 構成する星
        std::vector<ConstellationLineDef> lines;                             //!< 星座の線
        double                            completionPayout     = 10000.0;    //!< 完成したときの基本の星屑
        double                            completionMultiplier = 2.0;        //!< 完成するたびに全体倍率へ掛ける量（永続）
        std::string                       bonusDescription;                  //!< 完成ボーナスの説明（UTF-8）
    };

    //-------------------------------------------------------------
    //! @struct ConstellationTable
    //! @brief  全ての星座
    //-------------------------------------------------------------
    struct ConstellationTable {
        std::vector<ConstellationDef> constellations;

        //-------------------------------------------------------------
        //! @brief  IDで星座を探す
        //! @param  id [in] 星座のID
        //! @return 見つかった星座（無ければ nullptr）
        //-------------------------------------------------------------
        [[nodiscard]] const ConstellationDef* Find(const std::string& id) const {
            for(const ConstellationDef& constellation : constellations) {
                if(constellation.id == id)
                    return &constellation;
            }
            return nullptr;
        }
    };

    template <class Archive>
    void serialize(Archive& archive, ConstellationStarDef& star) {
        archive(cereal::make_nvp("starId", star.starId));
        Tsukino::IO::LoadOptional(archive, "ra", star.ra);
        Tsukino::IO::LoadOptional(archive, "dec", star.dec);
        archive(cereal::make_nvp("x", star.x), cereal::make_nvp("y", star.y));
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationLineDef& line) {
        archive(cereal::make_nvp("from", line.from), cereal::make_nvp("to", line.to));
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationDef& constellation) {
        archive(cereal::make_nvp("id", constellation.id),
                cereal::make_nvp("name", constellation.name),
                cereal::make_nvp("stars", constellation.stars),
                cereal::make_nvp("lines", constellation.lines));
        Tsukino::IO::LoadOptional(archive, "completionPayout", constellation.completionPayout);
        Tsukino::IO::LoadOptional(archive, "completionMultiplier", constellation.completionMultiplier);
        Tsukino::IO::LoadOptional(archive, "bonusDescription", constellation.bonusDescription);
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationTable& table) {
        archive(cereal::make_nvp("constellations", table.constellations));
    }

}    // namespace StardustFever::Data

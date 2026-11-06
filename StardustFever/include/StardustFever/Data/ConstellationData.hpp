//-------------------------------------------------------------
//! @file   ConstellationData.hpp
//! @brief  星座のマスターデータ（Assets/Data/Constellations.json）
//! @note   Tools/BuildSkyCatalog.py が公開データ（Stellarium の星座線）から作る。手で書き換えない
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
    //! @brief  星座の線をつくる星1つ
    //-------------------------------------------------------------
    struct ConstellationStarDef {
        std::string starId;    //!< StarDef::id
    };

    //-------------------------------------------------------------
    //! @struct ConstellationLineDef
    //! @brief  星座の線（星の添字の組）
    //-------------------------------------------------------------
    struct ConstellationLineDef {
        int from = 0;    //!< 片方の端の星の添字
        int to   = 0;    //!< もう片方の端の星の添字
    };

    //-------------------------------------------------------------
    //! @struct ConstellationBoardStarDef
    //! @brief  星ゲットチャンスで盤面に出す星1つと、その位置
    //! @note   x / y は役物と重ならないことを確かめて書いてある
    //-------------------------------------------------------------
    struct ConstellationBoardStarDef {
        std::string starId;      //!< StarDef::id
        float       x = 0.0f;    //!< 盤面上のX（ワールド単位）
        float       y = 0.0f;    //!< 盤面上のY（ワールド単位）
    };

    //-------------------------------------------------------------
    //! @struct ConstellationBoardDef
    //! @brief  星ゲットチャンスで盤面に出す形
    //! @note   星が多い星座は、明るい星から線でつながる順に絞ってある（lines の添字は stars のもの）
    //-------------------------------------------------------------
    struct ConstellationBoardDef {
        std::vector<ConstellationBoardStarDef> stars;
        std::vector<ConstellationLineDef>      lines;
    };

    //-------------------------------------------------------------
    //! @struct ConstellationEffectDef
    //! @brief  星座の星を全部そろえたときの固有の効果
    //! @note   type は global / pinPayout / pocketPayout / lotteryChance / starChance / feverDuration
    //-------------------------------------------------------------
    struct ConstellationEffectDef {
        std::string type  = "global";    //!< 効果の種類
        double      value = 0.0;         //!< 効果の量（★1 のとき）
    };

    //-------------------------------------------------------------
    //! @struct ConstellationDef
    //! @brief  星座1つの定義
    //-------------------------------------------------------------
    struct ConstellationDef {
        std::string                       id;         //!< 参照用のID（IAU の略符の小文字。例: "ori"）
        std::string                       name;       //!< 表示名（UTF-8）
        std::string                       nameEn;     //!< ラテン語名
        std::string                       caption;    //!< 図鑑の文章（神話・見つけ方・天文の話題）
        std::vector<ConstellationStarDef> stars;      //!< 線をつくる星すべて（そろえる対象）
        std::vector<ConstellationLineDef> lines;      //!< 星座の線
        ConstellationEffectDef            effect;     //!< そろえたときの効果
        ConstellationBoardDef             board;      //!< 星ゲットチャンスで盤面に出す形
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
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationLineDef& line) {
        archive(cereal::make_nvp("from", line.from), cereal::make_nvp("to", line.to));
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationBoardStarDef& star) {
        archive(cereal::make_nvp("starId", star.starId), cereal::make_nvp("x", star.x), cereal::make_nvp("y", star.y));
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationBoardDef& board) {
        archive(cereal::make_nvp("stars", board.stars), cereal::make_nvp("lines", board.lines));
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationEffectDef& effect) {
        Tsukino::IO::LoadOptional(archive, "type", effect.type);
        Tsukino::IO::LoadOptional(archive, "value", effect.value);
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationDef& constellation) {
        archive(cereal::make_nvp("id", constellation.id),
                cereal::make_nvp("name", constellation.name),
                cereal::make_nvp("stars", constellation.stars),
                cereal::make_nvp("lines", constellation.lines));
        Tsukino::IO::LoadOptional(archive, "nameEn", constellation.nameEn);
        Tsukino::IO::LoadOptional(archive, "caption", constellation.caption);
        Tsukino::IO::LoadOptional(archive, "effect", constellation.effect);
        Tsukino::IO::LoadOptional(archive, "board", constellation.board);
    }

    template <class Archive>
    void serialize(Archive& archive, ConstellationTable& table) {
        archive(cereal::make_nvp("constellations", table.constellations));
    }

}    // namespace StardustFever::Data

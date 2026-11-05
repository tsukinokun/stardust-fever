//-------------------------------------------------------------
//! @file   StarData.hpp
//! @brief  星のマスターデータ（Assets/Data/Stars.json）
//! @note   Tools/BuildSkyCatalog.py が公開データ（HYG Database / IAU の星名）から作る。手で書き換えない
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <unordered_map>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct StarDef
    //! @brief  星1つの定義
    //-------------------------------------------------------------
    struct StarDef {
        std::string id;                      //!< 参照用のID（"hip_" + ヒッパルコス番号）
        std::string name;                    //!< 表示名（UTF-8）。固有名が無い星は designation と同じ
        std::string nameEn;                  //!< IAU の固有名（無ければ空）
        std::string designation;             //!< バイエル符号などの呼び名（例: "オリオン座α"）
        std::string constellationId;         //!< この星が属する星座（星座の境界で決まる。線は別の星座にも使われることがある）
        double      ra           = 0.0;      //!< 赤経（時）
        double      dec          = 0.0;      //!< 赤緯（度）
        double      magnitude    = 2.0;      //!< 見かけの等級。小さいほど明るく珍しい
        std::string spectralType = "A";      //!< スペクトル型（O/B/A/F/G/K/M）
        double      basePayout   = 100.0;    //!< この星の値打ち（最大レベルで重なったときに換える星屑の元）
        double      levelBonus   = 0.01;     //!< レベル1つごとに全体倍率へ足す量
    };

    //-------------------------------------------------------------
    //! @struct StarAliasDef
    //! @brief  古いIDから今のIDへの対応（古いセーブを読むため）
    //-------------------------------------------------------------
    struct StarAliasDef {
        std::string from;    //!< 古いID
        std::string to;      //!< 今のID
    };

    //-------------------------------------------------------------
    //! @struct StarTable
    //! @brief  全ての星
    //-------------------------------------------------------------
    struct StarTable {
        std::vector<StarDef>      stars;
        std::vector<StarAliasDef> aliases;

        //-------------------------------------------------------------
        //! @brief  IDで星を探す
        //! @param  id [in] 星のID
        //! @return 見つかった星（無ければ nullptr）
        //! @note   星は数百あり毎フレーム引くので、読み込んだときに作る索引を使う
        //-------------------------------------------------------------
        [[nodiscard]] const StarDef* Find(const std::string& id) const {
            // 読み込みに失敗して stars だけ変わったときも外へ出ないよう、添字と ID を確かめる
            const auto found = m_index.find(id);
            if(found == m_index.end() || found->second >= stars.size() || stars[found->second].id != id)
                return nullptr;
            return &stars[found->second];
        }

        //-------------------------------------------------------------
        //! @brief  古いIDを今のIDへ置き換える
        //! @param  id [in] 星のID
        //! @return 対応があれば今のID、無ければそのまま
        //-------------------------------------------------------------
        [[nodiscard]] std::string ResolveAlias(const std::string& id) const {
            for(const StarAliasDef& alias : aliases) {
                if(alias.from == id)
                    return alias.to;
            }
            return id;
        }

        //-------------------------------------------------------------
        //! @brief  ID の索引を作り直す（読み込んだ直後に呼ぶ）
        //-------------------------------------------------------------
        void BuildIndex() {
            m_index.clear();
            for(size_t i = 0; i < stars.size(); ++i) {
                m_index.emplace(stars[i].id, i);
            }
        }

    private:
        std::unordered_map<std::string, size_t> m_index;
    };

    template <class Archive>
    void serialize(Archive& archive, StarDef& star) {
        archive(cereal::make_nvp("id", star.id), cereal::make_nvp("name", star.name));
        Tsukino::IO::LoadOptional(archive, "nameEn", star.nameEn);
        Tsukino::IO::LoadOptional(archive, "designation", star.designation);
        Tsukino::IO::LoadOptional(archive, "constellationId", star.constellationId);
        Tsukino::IO::LoadOptional(archive, "ra", star.ra);
        Tsukino::IO::LoadOptional(archive, "dec", star.dec);
        Tsukino::IO::LoadOptional(archive, "magnitude", star.magnitude);
        Tsukino::IO::LoadOptional(archive, "spectralType", star.spectralType);
        Tsukino::IO::LoadOptional(archive, "basePayout", star.basePayout);
        Tsukino::IO::LoadOptional(archive, "levelBonus", star.levelBonus);
    }

    template <class Archive>
    void serialize(Archive& archive, StarAliasDef& alias) {
        archive(cereal::make_nvp("from", alias.from), cereal::make_nvp("to", alias.to));
    }

    template <class Archive>
    void serialize(Archive& archive, StarTable& table) {
        archive(cereal::make_nvp("stars", table.stars));
        Tsukino::IO::LoadOptional(archive, "aliases", table.aliases);
    }

}    // namespace StardustFever::Data

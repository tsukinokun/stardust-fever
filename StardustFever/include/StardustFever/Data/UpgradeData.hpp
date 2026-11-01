//-------------------------------------------------------------
//! @file   UpgradeData.hpp
//! @brief  強化のマスターデータ（Assets/Data/Upgrades.json）
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/UpgradeFormula.hpp>

#include <Tsukino/Core/IO/CerealOptional.hpp>

#include <cereal/cereal.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <string>
#include <vector>

// 名前空間 : StardustFever::Data
namespace StardustFever::Data {
    //-------------------------------------------------------------
    //! @struct UpgradeDef
    //! @brief  強化1つの定義
    //! @note   並び順がそのまま購入キー（1〜9）になる
    //-------------------------------------------------------------
    struct UpgradeDef {
        std::string id;                       //!< 参照用のID（UpgradeLevelsComponent のキー）
        std::string name;                     //!< 表示名（UTF-8）
        std::string description;              //!< 効果の説明（UTF-8。HUD に効果量と並べて出す）
        std::string effect;                   //!< 効果の種類（UpgradeEffectSystem が見る。例: "pinPayout"）
        std::string stacking      = "add";    //!< "add"（値×Lv）か "multiply"（(1+値)^Lv）
        double      valuePerLevel = 0.0;      //!< 1レベルあたりの効果量
        double      baseCost      = 100.0;    //!< Lv0 → Lv1 の値段
        double      costGrowth    = 1.5;      //!< レベルごとに値段へ掛ける率
        int         maxLevel      = 0;        //!< 上限（0 なら無制限）
        std::string displayUnit;              //!< 足し算の効果の表示の単位（"%" なら100倍して%表示、"秒" など）

        //-------------------------------------------------------------
        //! @brief  積み上がり方を取得する
        //! @return "multiply" なら Multiply、それ以外は Add
        //-------------------------------------------------------------
        [[nodiscard]] Core::UpgradeFormula::Stacking Stacking() const {
            return stacking == "multiply" ? Core::UpgradeFormula::Stacking::Multiply : Core::UpgradeFormula::Stacking::Add;
        }

        //-------------------------------------------------------------
        //! @brief  指定レベルの効果量を計算する
        //! @param  level [in] レベル
        //! @return Add なら足す量、Multiply なら掛ける倍率
        //-------------------------------------------------------------
        [[nodiscard]] double EffectAt(int level) const { return Core::UpgradeFormula::EffectValue(Stacking(), valuePerLevel, level); }

        //-------------------------------------------------------------
        //! @brief  指定レベルから1つ上げる値段を計算する
        //! @param  level [in] 今のレベル
        //! @return 値段
        //-------------------------------------------------------------
        [[nodiscard]] Core::BigNumber CostAt(int level) const { return Core::UpgradeFormula::Cost(baseCost, costGrowth, level); }
    };

    //-------------------------------------------------------------
    //! @struct UpgradeTable
    //! @brief  全ての強化
    //-------------------------------------------------------------
    struct UpgradeTable {
        std::vector<UpgradeDef> upgrades;
    };

    template <class Archive>
    void serialize(Archive& archive, UpgradeDef& upgrade) {
        archive(cereal::make_nvp("id", upgrade.id), cereal::make_nvp("name", upgrade.name), cereal::make_nvp("effect", upgrade.effect));
        Tsukino::IO::LoadOptional(archive, "description", upgrade.description);
        Tsukino::IO::LoadOptional(archive, "stacking", upgrade.stacking);
        Tsukino::IO::LoadOptional(archive, "valuePerLevel", upgrade.valuePerLevel);
        Tsukino::IO::LoadOptional(archive, "baseCost", upgrade.baseCost);
        Tsukino::IO::LoadOptional(archive, "costGrowth", upgrade.costGrowth);
        Tsukino::IO::LoadOptional(archive, "maxLevel", upgrade.maxLevel);
        Tsukino::IO::LoadOptional(archive, "displayUnit", upgrade.displayUnit);
    }

    template <class Archive>
    void serialize(Archive& archive, UpgradeTable& table) {
        archive(cereal::make_nvp("upgrades", table.upgrades));
    }

}    // namespace StardustFever::Data

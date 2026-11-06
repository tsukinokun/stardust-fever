//-------------------------------------------------------------
//! @file   UpgradeText.hpp
//! @brief  強化の効果を文字にする処理
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/UpgradeFormula.hpp>
#include <StardustFever/Data/UpgradeData.hpp>

#include <cstdio>
#include <string>

// 名前空間 : StardustFever::ECS::UpgradeText
namespace StardustFever::ECS::UpgradeText {

    //-------------------------------------------------------------
    //! @brief  強化の効果量を短い文字にする
    //! @param  upgrade [in] 強化
    //! @param  level   [in] レベル
    //! @return 例: "×1.95"、"+3%"、"+10秒"
    //-------------------------------------------------------------
    inline std::string Effect(const Data::UpgradeDef& upgrade, int level) {
        const double value = upgrade.EffectAt(level);
        char         buffer[48];
        if(upgrade.Stacking() == Core::UpgradeFormula::Stacking::Multiply)
            std::snprintf(buffer, sizeof(buffer), "×%.2f", value);
        else if(upgrade.displayUnit == "%")
            std::snprintf(buffer, sizeof(buffer), "+%g%%", value * 100.0);
        else
            std::snprintf(buffer, sizeof(buffer), "+%g%s", value, upgrade.displayUnit.c_str());
        return buffer;
    }

    //-------------------------------------------------------------
    //! @brief  自動発射の間隔を文字にする
    //! @param  upgrade      [in] 強化
    //! @param  level        [in] レベル
    //! @param  baseInterval [in] 間隔の元（秒）
    //! @return 例: "2.0秒"
    //-------------------------------------------------------------
    inline std::string Interval(const Data::UpgradeDef& upgrade, int level, float baseInterval) {
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%.1f秒", baseInterval * upgrade.EffectAt(level));
        return buffer;
    }

}    // namespace StardustFever::ECS::UpgradeText

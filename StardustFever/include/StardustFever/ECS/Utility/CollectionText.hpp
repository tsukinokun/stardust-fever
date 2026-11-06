//-------------------------------------------------------------
//! @file   CollectionText.hpp
//! @brief  星と星座の効果や性質を、画面に出す文に直す
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>
#include <StardustFever/Data/ConstellationData.hpp>
#include <StardustFever/Data/StarData.hpp>

#include <cstdio>
#include <string>

// 名前空間 : StardustFever::ECS::CollectionText
namespace StardustFever::ECS::CollectionText {

    //-------------------------------------------------------------
    //! @brief  星座の効果を文にする
    //! @param  effect [in] 効果
    //! @param  value  [in] 効果の量（星のレベルで伸びた後の値）
    //! @return 例: "釘の星屑 ×1.15"
    //-------------------------------------------------------------
    inline std::string Effect(const Data::ConstellationEffectDef& effect, double value) {
        char buffer[64];
        if(effect.type == "pinPayout")
            std::snprintf(buffer, sizeof(buffer), "釘の星屑 ×%.2f", 1.0 + value);
        else if(effect.type == "pocketPayout")
            std::snprintf(buffer, sizeof(buffer), "入賞の星屑 ×%.2f", 1.0 + value);
        else if(effect.type == "lotteryChance")
            std::snprintf(buffer, sizeof(buffer), "フィーバー当選 +%.1f%%", value * 100.0);
        else if(effect.type == "starChance")
            std::snprintf(buffer, sizeof(buffer), "星ゲットチャンス +%.1f%%", value * 100.0);
        else if(effect.type == "feverDuration")
            std::snprintf(buffer, sizeof(buffer), "フィーバー +%.1f秒", value);
        else
            std::snprintf(buffer, sizeof(buffer), "全体倍率 ×%.2f", 1.0 + value);
        return buffer;
    }

    //-------------------------------------------------------------
    //! @brief  倍率を文にする
    //! @param  value [in] 倍率
    //! @return 桁が増えるほど小数を減らす（"×1.35"、"×106.8"、"×1068"）。1万以上は和風の単位（"×12.3万"）
    //! @note   BigNumber::ToJapaneseString は1万未満を整数にするため、倍率の小数が消えてしまう
    //-------------------------------------------------------------
    inline std::string Multiplier(const Core::BigNumber& value) {
        if(value.Exponent() >= 4)
            return "×" + value.ToJapaneseString();
        char         buffer[32];
        const double number = value.ToDouble();
        std::snprintf(buffer, sizeof(buffer), number < 100.0 ? "×%.2f" : (number < 1000.0 ? "×%.1f" : "×%.0f"), number);
        return buffer;
    }

    //-------------------------------------------------------------
    //! @brief  天体の種類を言葉にする
    //! @param  kind [in] star / cluster / nebula / galaxy / other
    //! @return 例: "星雲"
    //-------------------------------------------------------------
    inline const char* SkyObjectKind(const std::string& kind) {
        if(kind == "cluster")
            return "星団";
        if(kind == "nebula")
            return "星雲";
        if(kind == "galaxy")
            return "銀河";
        if(kind == "other")
            return "ブラックホール";
        return "恒星";
    }

    //-------------------------------------------------------------
    //! @brief  スペクトル型を色の言葉にする
    //! @param  spectralType [in] O/B/A/F/G/K/M
    //! @return 例: "赤い星"
    //-------------------------------------------------------------
    inline const char* SpectralColor(const std::string& spectralType) {
        const char type = spectralType.empty() ? 'A' : spectralType[0];
        switch(type) {
        case 'O':
        case 'B':
            return "青白い星";
        case 'F':
            return "黄白色の星";
        case 'G':
            return "黄色い星";
        case 'K':
            return "橙色の星";
        case 'M':
            return "赤い星";
        case 'A':
        default:
            return "白い星";
        }
    }

    //-------------------------------------------------------------
    //! @brief  星の性質を一言にする（教育のため、呼び名・等級・色を出す）
    //! @param  star [in] 星
    //! @return 例: "オリオン座α  0.45等  M型・赤い星"
    //-------------------------------------------------------------
    inline std::string StarFacts(const Data::StarDef& star) {
        char buffer[96];
        std::snprintf(buffer, sizeof(buffer), "%.2f等  %s型・%s", star.magnitude, star.spectralType.c_str(), SpectralColor(star.spectralType));
        // 固有名のある星は、呼び名（バイエル符号）も添える
        if(!star.designation.empty() && star.designation != star.name)
            return star.designation + "  " + buffer;
        return buffer;
    }

}    // namespace StardustFever::ECS::CollectionText

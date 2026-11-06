//-------------------------------------------------------------
//! @file   StarColor.hpp
//! @brief  星の色の処理
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/HudLayoutConfigComponent.hpp>

#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    //! @brief  スペクトル型（の頭文字）から星の色を選ぶ
    //! @param  layout       [in] HUD の調整値（無ければ白）
    //! @param  spectralType [in] スペクトル型
    //! @return 色
    //-------------------------------------------------------------
    inline hlslpp::float4 SpectralColor(const HudLayoutConfigComponent* layout, const std::string& spectralType) {
        if(!layout)
            return hlslpp::float4(1.0f, 1.0f, 1.0f, 1.0f);
        switch(spectralType.empty() ? 'A' : spectralType.front()) {
        case 'O':
            return layout->colorO;
        case 'B':
            return layout->colorB;
        case 'F':
            return layout->colorF;
        case 'G':
            return layout->colorG;
        case 'K':
            return layout->colorK;
        case 'M':
            return layout->colorM;
        default:
            return layout->colorA;
        }
    }

}    // namespace StardustFever::ECS

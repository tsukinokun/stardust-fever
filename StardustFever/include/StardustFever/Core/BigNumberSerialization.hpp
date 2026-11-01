//-------------------------------------------------------------
//! @file   BigNumberSerialization.hpp
//! @brief  BigNumber の cereal シリアライズ定義
//-------------------------------------------------------------
#pragma once
#include <StardustFever/Core/BigNumber.hpp>

#include <cereal/cereal.hpp>

// 名前空間 : StardustFever::Core
namespace StardustFever::Core {

    template <class Archive>
    void save(Archive& archive, const BigNumber& value) {
        archive(cereal::make_nvp("mantissa", value.Mantissa()), cereal::make_nvp("exponent", value.Exponent()));
    }

    template <class Archive>
    void load(Archive& archive, BigNumber& value) {
        double  mantissa = 0.0;
        int64_t exponent = 0;
        archive(cereal::make_nvp("mantissa", mantissa), cereal::make_nvp("exponent", exponent));
        value.SetParts(mantissa, exponent);
    }

}    // namespace StardustFever::Core

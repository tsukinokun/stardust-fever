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

    //-------------------------------------------------------------
    //! @note   一時変数は 0 ではなく今の値から始める。PrefabFactory::InstantiateGroup は生成後に
    //!         EntityRefResolverArchive で全ての load() をもう一度通すが、そのアーカイブは値を書かない。
    //!         0 から始めると、その2回目の通過で JSON から読んだ値が 0 に戻ってしまう
    //!         （所持星屑の初期値が効かない不具合の原因だった）
    //-------------------------------------------------------------
    template <class Archive>
    void load(Archive& archive, BigNumber& value) {
        double  mantissa = value.Mantissa();
        int64_t exponent = value.Exponent();
        archive(cereal::make_nvp("mantissa", mantissa), cereal::make_nvp("exponent", exponent));
        value.SetParts(mantissa, exponent);
    }

}    // namespace StardustFever::Core

//-------------------------------------------------------------
//! @file   CollectionEffectSystem.hpp
//! @brief  集めた星座とグループの効果を CollectionModifiersComponent にまとめる System
//-------------------------------------------------------------
#pragma once
#include <StardustFever/ECS/Component/CollectionModifiersComponent.hpp>

#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <set>
#include <string>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  CollectionEffectSystem
    //! @brief  星座の星を全部持っていれば、その星座の固有の効果を足す（星の平均レベルで少し伸びる）。
    //!         グループ（季節の星座・黄道12星座など）の星座を全部そろえるとグループの効果を、
    //!         88星座が全部そろうと全体倍率をさらに掛ける
    //! @note   毎フレーム最初から計算し直すので、星や星座のデータを書き換えるとすぐ反映される。
    //!         星1つごとの全体倍率は ScoreMultiplierSystem が計算する
    //-------------------------------------------------------------
    class CollectionEffectSystem final : public Tsukino::ECS::ISystem {
    public:
        //-------------------------------------------------------------
        //! @brief  効果を計算し直す
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  効果を1つ足す
        //! @param  result [in,out] まとめている効果
        //! @param  type   [in] 効果の種類
        //! @param  value  [in] 効果の量
        //! @param  owner  [in] 効果の持ち主（知らない種類のときの警告に出す）
        //-------------------------------------------------------------
        void ApplyEffect(CollectionModifiersComponent& result, const std::string& type, double value, const std::string& owner);

        std::set<std::string> m_warnedTypes;    //!< 一度警告した、知らない効果の種類（毎フレーム出さないため）
    };
}    // namespace StardustFever::ECS

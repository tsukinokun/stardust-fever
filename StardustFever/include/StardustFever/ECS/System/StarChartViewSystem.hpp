//-------------------------------------------------------------
//! @file   StarChartViewSystem.hpp
//! @brief  星図（集めた星と星座）を並べて動かす System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarChartViewSystem
    //! @brief  星座ごとの見出しの下に、星を1行ずつ「スペクトル型の色の星＋名前＋★の段数」で並べる。
    //!         まだ見つけていない星は暗い輪郭だけにし、レベルが上がった星はアイコンを弾ませる
    //! @note   星と星座の数が変わったら（データのホットリロード）作り直す
    //-------------------------------------------------------------
    class StarChartViewSystem final : public Tsukino::ECS::ISystem {
    public:
        StarChartViewSystem();

        //-------------------------------------------------------------
        //! @brief  行を作り、見た目を更新する
        //! @param  registry  [in] レジストリ
        //! @param  deltaTime [in] 経過時間（秒）
        //-------------------------------------------------------------
        void Update(Tsukino::ECS::Registry& registry, float deltaTime) override;

    private:
        //-------------------------------------------------------------
        //! @brief  行を全て作り直す
        //! @param  registry [in] レジストリ
        //-------------------------------------------------------------
        void Rebuild(Tsukino::ECS::Registry& registry);

        std::mt19937 m_random;
    };
}    // namespace StardustFever::ECS

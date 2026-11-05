//-------------------------------------------------------------
//! @file   StarChartViewSystem.hpp
//! @brief  星図（集めた星と星座）を並べて動かす System
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/System/ISystem.hpp>

#include <random>
#include <string>
#include <vector>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @class  StarChartViewSystem
    //! @brief  右の星図パネル。そろった星座と集めた星の数を出し、その下に星を1行ずつ
    //!         「スペクトル型の色の星＋名前＋★の段数」で並べる。
    //!         星ゲットチャンス中は盤面の星（つないだ星を明るく）、それ以外は最近手に入れた星を出す
    //! @note   88星座すべては並べきれないので、一覧は星図の画面（別の画面）で見る。並べる星が変わったら作り直す
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
        //! @param  starIds  [in] 並べる星
        //! @param  chanceId [in] チャンス中なら星座のID（それ以外は空）
        //-------------------------------------------------------------
        void Rebuild(Tsukino::ECS::Registry& registry, const std::vector<std::string>& starIds, const std::string& chanceId);

        std::mt19937 m_random;
        std::string  m_rowsKey;    //!< 今並べている行の中身（変わったら作り直す）
    };
}    // namespace StardustFever::ECS

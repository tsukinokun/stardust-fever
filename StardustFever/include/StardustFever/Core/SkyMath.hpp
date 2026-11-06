//-------------------------------------------------------------
//! @file   SkyMath.hpp
//! @brief  星図の画面で空の座標（赤経・赤緯）を平面へ写す計算
//! @note   エンジンに依存させず、単体テストできるようにしてある
//-------------------------------------------------------------
#pragma once
#include <vector>

// 名前空間 : StardustFever::Core::SkyMath
namespace StardustFever::Core::SkyMath {

    //-------------------------------------------------------------
    //! @brief  角度を -180 以上 180 未満へ回し込む
    //! @param  degrees [in] 角度（度）
    //! @return 同じ向きを表す -180〜180 の角度
    //-------------------------------------------------------------
    double Wrap180(double degrees);

    //-------------------------------------------------------------
    //! @struct Direction
    //! @brief  天球の中心から見た向き（長さ1。x は右、y は上、z は前が正の左手系）
    //-------------------------------------------------------------
    struct Direction {
        double x = 0.0;
        double y = 0.0;
        double z = 1.0;
    };

    //-------------------------------------------------------------
    //! @struct Rotation
    //! @brief  回転を表す四元数
    //-------------------------------------------------------------
    struct Rotation {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
        double w = 1.0;
    };

    //-------------------------------------------------------------
    //! @struct Point
    //! @brief  画面の中央からのずれ（ピクセル。x は右、y は下が正）
    //-------------------------------------------------------------
    struct Point {
        double x = 0.0;
        double y = 0.0;
    };

    //-------------------------------------------------------------
    //! @brief  赤経・赤緯を、天球の中心から見た向きにする
    //! @param  raDeg  [in] 赤経（度）
    //! @param  decDeg [in] 赤緯（度）
    //! @return 向き
    //! @note   内側から空を見上げるので、北を上にしたとき赤経が大きいほど左へ置く
    //-------------------------------------------------------------
    Direction ToDirection(double raDeg, double decDeg);

    //-------------------------------------------------------------
    //! @brief  向きを赤経・赤緯へ戻す
    //! @param  direction [in] 向き（長さは問わない）
    //! @param  raDeg     [out] 赤経（0〜360度）
    //! @param  decDeg    [out] 赤緯（度）
    //-------------------------------------------------------------
    void ToRaDec(const Direction& direction, double& raDeg, double& decDeg);

    //-------------------------------------------------------------
    //! @brief  +X を指定の向きへ回す回転を求める
    //! @param  to [in] 向かせる先（長さは問わない）
    //! @return 回転
    //! @note   星座の線のモデルは X 方向に長いので、2つの星の間へ向けるのに使う
    //-------------------------------------------------------------
    Rotation ArcFromXAxis(const Direction& to);

    //-------------------------------------------------------------
    //! @brief  空の座標を、見ている向きの画面へ写す
    //! @param  raCenter   [in] 画面の中央の赤経（度）
    //! @param  decCenter  [in] 画面の中央の赤緯（度）
    //! @param  fovDeg     [in] 縦の視野（度）
    //! @param  halfHeight [in] 画面の高さの半分（ピクセル）
    //! @param  raDeg      [in] 写す赤経（度）
    //! @param  decDeg     [in] 写す赤緯（度）
    //! @param  point      [out] 画面の中央からのずれ
    //! @return 視線より前にあれば true。後ろなら false で point は書かない
    //-------------------------------------------------------------
    bool ProjectToScreen(double raCenter, double decCenter, double fovDeg, double halfHeight, double raDeg, double decDeg, Point& point);

    //-------------------------------------------------------------
    //! @brief  角度の平均（0度と360度をまたいでも正しく求める）
    //! @param  degrees [in] 角度（度）
    //! @return 平均の向き（0〜360）。空なら 0
    //-------------------------------------------------------------
    double CircularMeanDegrees(const std::vector<double>& degrees);

}    // namespace StardustFever::Core::SkyMath

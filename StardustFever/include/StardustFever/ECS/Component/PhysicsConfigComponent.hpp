//-------------------------------------------------------------
//! @file   PhysicsConfigComponent.hpp
//! @brief  PhysicsConfigComponent の宣言
//-------------------------------------------------------------
#pragma once

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {
    //-------------------------------------------------------------
    //! @struct PhysicsConfigComponent
    //! @brief  物理の調整値
    //! @note   設定エンティティに付く。unitsPerMeter は最初の更新でだけ反映される
    //-------------------------------------------------------------
    struct PhysicsConfigComponent {
        float unitsPerMeter              = 100.0f;    //!< 1メートルが何ワールド単位か（盤面は cm 単位で作ってある）
        int   substeps                   = 8;         //!< 1フレームを何回に分けて物理を進めるか。速い玉が細い釘をすり抜けないようにする
        float penetrationSlop            = 0.05f;     //!< めり込みの許容量（ワールド単位）
        float speculativeContactDistance = 1.0f;      //!< 離れていても接触を作り始める距離（ワールド単位）
    };
}    // namespace StardustFever::ECS

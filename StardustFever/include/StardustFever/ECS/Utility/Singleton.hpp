//-------------------------------------------------------------
//! @file   Singleton.hpp
//! @brief  ゲーム中に1つだけ存在するコンポーネントの取得
//-------------------------------------------------------------
#pragma once
#include <Tsukino/Core/ECS/Registry/Registry.hpp>

// 名前空間 : StardustFever::ECS
namespace StardustFever::ECS {

    //-------------------------------------------------------------
    //! @brief  1つだけ存在するはずのコンポーネントを探す
    //! @tparam T 探すコンポーネント
    //! @param  registry [in] レジストリ
    //! @return 見つかったコンポーネント（無ければ nullptr）
    //! @note   設定やゲーム状態は Registry::SetContext ではなく、Prefab から生成した
    //!         エンティティに付けている。値を JSON に書けて、ホットリロードもできるため。
    //!         Prefab の読み込みに失敗すると無いことがあるので、呼び出し側は nullptr を確かめること
    //-------------------------------------------------------------
    template <class T>
    T* FindSingleton(Tsukino::ECS::Registry& registry) {
        for(const auto entity : registry.View<T>()) {
            return &registry.GetComponent<T>(entity);
        }
        return nullptr;
    }

}    // namespace StardustFever::ECS

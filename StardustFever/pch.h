//------------------------------------------------------------
//! @file   pch.h
//! @brief  プリコンパイル済みヘッダー
//! @note   ビルド時間短縮用。ヘッダはこれに依存せず自己完結に書くこと
//------------------------------------------------------------
#pragma once
#define NOMINMAX
#include <Windows.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>
#include <hlsl++.h>

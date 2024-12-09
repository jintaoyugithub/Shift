#pragma once

#include <utils/common.hpp>

// thread num that used in compute shader
constexpr int kThreadGroupSizeX = 32;
constexpr int kThreadGroupSizeY = 32;
// take the 3rd dim into account, because we gonna do it later
constexpr int kThreadGroupSizeZ = 1;

inline void swap(bgfx::DynamicVertexBufferHandle &prev, bgfx::DynamicVertexBufferHandle &cur)
{
    bgfx::DynamicVertexBufferHandle temp = prev;
    prev = cur;
    cur = temp;
}

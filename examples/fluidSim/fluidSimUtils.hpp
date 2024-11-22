#pragma once

#include <utils/common.hpp>

// thread num that used in compute shader
constexpr int kThreadGroupSizeX = 8;
constexpr int kThreadGroupSizeY = 8;
// take the 3rd dim into account, because we gonna do it later
constexpr int kThreadGroupSizeZ = 8;

inline void swap(bgfx::DynamicVertexBufferHandle &prev, bgfx::DynamicVertexBufferHandle &cur)
{
    bgfx::DynamicVertexBufferHandle temp = cur;
    prev = cur;
    cur = temp;
}

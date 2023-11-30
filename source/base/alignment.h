#pragma once

#include "base/types.h"

namespace base {

template <typename T> constexpr T AlignUp(T value, size_t size) {
    auto mod{static_cast<T>(value % size)};
    value -= mod;
    return static_cast<T>(mod == T{0} ? value : value + size);
}

template <typename T> constexpr T AlignDown(T value, size_t size) {
    return static_cast<T>(value - value % size);
}

template <typename T> constexpr bool IsAligned(T value, size_t alignment) {
    const T mask = static_cast<T>(alignment - 1);
    return (value & mask) == 0;
}

template <typename T> constexpr T DivUp(T value, size_t divisor) {
    return static_cast<T>((value + divisor - 1) / divisor);
}

constexpr bool IsPowerOfTwo(size_t x) {
    // From Hacker's Delight.  x & (x - 1) turns off (zeroes) the
    // rightmost 1-bit in the word x.  If x is a power of two, then the
    // result is, of course, 0.
    return x > 0 && ((x & (x - 1)) == 0);
}

}  // namespace base
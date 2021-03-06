#pragma once

#include <iostream>
#include <vector>

template<typename T>
const T get_bit(const T& number, const uint8_t& nth) {
    return (number & (1 << nth)) >> nth;
}

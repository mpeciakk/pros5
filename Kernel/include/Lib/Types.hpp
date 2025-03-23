#pragma once

using i8 = signed char;
using u8 = unsigned char;

using i16 = signed short int;
using u16 = unsigned short int;

using i32 = signed int;
using u32 = unsigned int;

using i64 = signed long long;
using u64 = unsigned long long;

constexpr u32 operator"" _KB(unsigned long long n) {
    return n * 1024;
}

constexpr u32 operator"" _MB(unsigned long long n) {
    return n * (1024_KB);
}

constexpr u32 operator"" _GB(unsigned long long n) {
    return n * (1024_MB);
}

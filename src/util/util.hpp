#pragma once

constexpr auto operator""_KiB(unsigned long long x) -> unsigned long long {
    return x * 1024;
}

constexpr auto operator""_MiB(unsigned long long x) -> unsigned long long {
    return x * 1024 * 1024;
}


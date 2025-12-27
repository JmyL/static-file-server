#pragma once

#include <charconv>
#include <ranges>
#include <stdexcept>
#include <string_view>

template <std::ranges::contiguous_range R> std::string_view to_sv(const R &r) {
    return {std::ranges::data(r), std::ranges::size(r)};
}

class TokenSplitter {
    using SplitView = decltype(std::declval<std::string_view>() |
                               std::ranges::views::split(' '));
    std::string_view original;
    SplitView split_view;
    decltype(split_view.begin()) it, end;

  public:
    TokenSplitter(std::string_view input, char delim = ' ')
        : original(input), split_view(input | std::ranges::views::split(delim)),
          it(split_view.begin()), end(split_view.end()) {}

    std::string_view next() {
        if (it == end)
            throw std::runtime_error("No more tokens");
        auto token = to_sv(*it);
        ++it;
        return token;
    }

    bool has_next() const { return it != end; }

    std::string_view rest() const {
        if (it == end)
            return {};
        auto pos = std::ranges::distance(original.begin(), (*it).begin());
        return original.substr(pos);
    }
};

inline int svtoi(std::string_view sv) {
    int value = 0;
    auto result = std::from_chars(sv.data(), sv.data() + sv.size(), value);
    if (result.ec != std::errc()) {
        throw std::invalid_argument("Invalid string view: " + std::string(sv));
    }
    return value;
}
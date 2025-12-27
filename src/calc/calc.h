#pragma once

#include <algorithm>
#include <deque>
#include <ranges>
#include <set>
#include <string>
#include <vector>

#include <boost/type_index.hpp>
#include <iostream>

template <typename T> void print_type_name() {
    std::cout << boost::typeindex::type_id_with_cvr<T>().pretty_name()
              << std::endl;
}

class UseSet {
  public:
    static auto dedupe(std::vector<std::string> input) {
        using It = decltype(input)::const_iterator;
        struct Cmp {
            bool operator()(const It &a, const It &b) const { return *a < *b; }
        };
        std::set<It, Cmp> set;

        for (auto it = input.cbegin(); it != input.cend(); ++it) {
            set.emplace(it);
        }

        std::vector<std::string> ret;
        ret.reserve(set.size());
        for (auto it = input.crbegin(); it != input.crend(); ++it) {
            if (auto sit = set.find(std::prev(it.base())); sit != set.end()) {
                ret.push_back(std::move(*it));
                set.erase(sit);
            }
        }
        std::reverse(ret.begin(), ret.end());
        return ret;
    }
};

class UseRuntimeConfigurableSet {
  public:
    static auto dedupe(std::vector<std::string> input) {
        using It = decltype(input)::const_iterator;
        auto cmp = [](auto a, auto b) -> bool { return *a < *b; };
        std::set<It, decltype(cmp)> set(cmp);

        for (auto it = input.cbegin(); it != input.cend(); ++it) {
            set.emplace(it);
        }

        std::vector<std::string> ret;
        ret.reserve(set.size());
        for (auto it = input.crbegin(); it != input.crend(); ++it) {
            if (auto sit = set.find(std::prev(it.base())); sit != set.end()) {
                ret.push_back(std::move(*it));
                set.erase(sit);
            }
        }
        std::reverse(ret.begin(), ret.end());
        return ret;
    }
};
#include <boost/asio.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::NiceMock;

#include <algorithm>
#include <ranges>
#include <string_view>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

size_t get_total_memory_bytes() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string key, unit;
            size_t mem_kb;
            iss >> key >> mem_kb >> unit;
            return mem_kb * 1024;
        }
    }
    return 0;
}

size_t get_available_memory_bytes() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    while (std::getline(meminfo, line)) {
        if (line.find("MemAvailable:") == 0) {
            std::istringstream iss(line);
            std::string key, unit;
            size_t mem_kb;
            iss >> key >> mem_kb >> unit;
            return mem_kb * 1024;
        }
    }
    return 0;
}

// available이 10% 이하가 되면 eviction하도록...

namespace {

TEST(Cache, KnowsHowMuchMemoryIsAvailable) {
    std::cout << get_available_memory_bytes() << std::endl;
}

} // namespace
#define BOOST_ASIO_HAS_IO_URING 1
#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>
#include <util/util.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string_view>

using namespace std::string_view_literals;
using namespace std::string_literals;

namespace net = boost::asio;

namespace {

void handleRead(boost::system::error_code ec, size_t n) {
    std::cout << ec.message() << ": " << n << " bytes" << std::endl;
}

struct AStreamFile : public ::testing::Test {
    void SetUp() override {
        file.open(filename);
        file << to_write;
        file.flush();
    }

    void TearDown() override { //
        std::filesystem::remove(filename);
    }

    net::io_context ioc;
    std::ofstream file;
    static constexpr auto to_write = "Hello, World!"sv;
    static constexpr auto filename = ".tmp"s;
};

TEST_F(AStreamFile, ReturnsCorrectSize) {
    net::stream_file file(ioc, filename, net::stream_file::flags::read_only);

    EXPECT_EQ(file.size(), to_write.size());
} // namespace

TEST_F(AStreamFile, SupportsRead) {
    net::stream_file file(ioc, filename, net::stream_file::flags::read_only);

    std::vector<char> buffer(file.size());
    net::read(file, net::buffer(buffer));

    EXPECT_EQ(buffer.size(), to_write.size());
    EXPECT_EQ(std::string_view(buffer.data(), buffer.size()), to_write);
} // namespace

} // namespace
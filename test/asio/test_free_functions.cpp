#include <boost/asio.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::NiceMock;

#include <algorithm>
#include <ranges>
#include <string_view>

using namespace std::string_view_literals;
namespace net = boost::asio;

namespace {

template <typename BufferSequence>
auto bufferseq_to_string_view(const BufferSequence &bufseq)
    -> std::string_view {
    return std::string_view(static_cast<const char *>(bufseq.data()),
                            bufseq.size());
}

struct MockSocket {
    MOCK_METHOD(size_t, read_some,
                (const net::mutable_buffer &buffer,
                 boost::system::error_code &ec),
                ());
    MOCK_METHOD(size_t, write_some,
                (const net::const_buffer &buffer,
                 boost::system::error_code &ec),
                ());
};

TEST(ReadUntil, UsesConsecutiveReadSomeCalls) {
    NiceMock<MockSocket> mock_socket;
    EXPECT_CALL(mock_socket, read_some(_, _))
        .WillOnce([](const net::mutable_buffer &buffer,
                     boost::system::error_code &ec) {
            constexpr const auto data = "world"sv;
            std::ranges::copy(data, static_cast<char *>(buffer.data()));
            ec = boost::system::error_code();
            return data.size();
        })
        .WillOnce([](const net::mutable_buffer &buffer,
                     boost::system::error_code &ec) {
            constexpr const auto data = "\n"sv;
            std::ranges::copy(data, static_cast<char *>(buffer.data()));
            ec = boost::system::error_code();
            return data.size();
        });

    net::streambuf sbuf;
    auto len = net::read_until(mock_socket, sbuf, "\n");
    EXPECT_EQ(bufferseq_to_string_view(sbuf.data()), "world\n"sv);
    sbuf.consume(len);
}

} // namespace

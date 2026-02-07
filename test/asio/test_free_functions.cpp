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
auto bufferseq_to_string_view(const BufferSequence &bufseq, size_t len)
    -> std::string_view {
    return std::string_view(static_cast<const char *>(bufseq.data()), len);
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
            constexpr const auto data = "\ndummy"sv;
            std::ranges::copy(data, static_cast<char *>(buffer.data()));
            ec = boost::system::error_code();
            return data.size();
        });

    net::streambuf sbuf;
    auto len = net::read_until(mock_socket, sbuf, "\n");
    EXPECT_EQ(bufferseq_to_string_view(sbuf.data(), len), "world\n"sv);
    sbuf.consume(len);
}

TEST(ReadUntil, HandlesMultiCharDelimiter) {
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
            constexpr const auto data = "\r\ndummy"sv;
            std::ranges::copy(data, static_cast<char *>(buffer.data()));
            ec = boost::system::error_code();
            return data.size();
        });

    net::streambuf sbuf;
    auto len = net::read_until(mock_socket, sbuf, "\r\n");
    EXPECT_EQ(bufferseq_to_string_view(sbuf.data(), len), "world\r\n"sv);
    sbuf.consume(len);
}

TEST(MultilineString, CanRepresentHttpStyleNewLine) {

    constexpr auto response = "HTTP/1.1 200 OK\r\n"
                              "Server: nginx/1.29.4\r\n"
                              "Date: Sat, 03 Jan 2026 15:22:53 GMT\r\n"
                              "Content-Type: text/plain\r\n"
                              "Content-Length: 1192\r\n"
                              "Last-Modified: Sun, 28 Dec 2025 17:25:28 GMT\r\n"
                              "Connection: keep-alive\r\n"
                              "ETag: \"69516808-4a8\"\r\n"
                              "Accept-Ranges: bytes\r\n"
                              "\r\n"sv;

    EXPECT_EQ(response.back(), '\n');
    EXPECT_EQ(response[response.size() - 2], '\r');
    EXPECT_EQ(response[response.size() - 3], '\n');
    EXPECT_EQ(response[response.size() - 4], '\r');
}

} // namespace
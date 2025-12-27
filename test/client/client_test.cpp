#include "gmock/gmock.h"
#include <client/client.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::HasSubstr;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Return;

#include <string_view>
using namespace std::literals::string_view_literals;

class MockConnection {
  public:
    MOCK_METHOD(std::string_view, peek_line, ());
    MOCK_METHOD(void, consume, ());
    MOCK_METHOD(void, write, (const std::string &buffers));

    template <ConstBufferSequence Buffers> //
    void write(const Buffers &buffers) {
        write(beast::buffers_to_string(buffers));
    }
};

static_assert(ConnectionType<MockConnection>,
              "MockConnection does not satisfy Connection concept");

TEST(AClient, HandlesHeloAndEnd) {
    NiceMock<MockConnection> mock_conn;

    EXPECT_CALL(mock_conn, peek_line())
        .WillOnce(Return("HELO"))
        .WillOnce(Return("END"));

    {
        InSequence seq;
        EXPECT_CALL(mock_conn, write("EHLO\n"));
        EXPECT_CALL(mock_conn, write("OK\n"));
    }

    run_client(mock_conn);
}

TEST(AClient, HandlesPowAndName) {
    NiceMock<MockConnection> mock_conn;

    EXPECT_CALL(mock_conn, peek_line())
        .WillOnce(Return("HELO"))
        .WillOnce(Return("POW authdata 0"))
        .WillOnce(Return("NAME somearg"))
        .WillOnce(Return("END"));

    {
        InSequence seq;
        EXPECT_CALL(mock_conn, write("EHLO\n"));
        EXPECT_CALL(mock_conn, write("\n"));
        EXPECT_CALL(mock_conn, write(HasSubstr("Sungsik Nam")));
        EXPECT_CALL(mock_conn, write("OK\n"));
    }

    run_client(mock_conn);
}

TEST(AClient, HandlesError) {
    NiceMock<MockConnection> mock_conn;

    EXPECT_CALL(mock_conn, peek_line())
        .WillOnce(Return("HELO"))
        .WillOnce(Return("ERROR Something went wrong"));

    EXPECT_CALL(mock_conn, write("EHLO\n"));

    run_client(mock_conn);
}

class MockStream {
  public:
    MOCK_METHOD(size_t, read_some,
                (net::mutable_buffer, boost::system::error_code &));
    MOCK_METHOD(size_t, write_some,
                (const std::string &, boost::system::error_code &));

    template <ConstBufferSequence Buffers> //
    size_t write_some(const Buffers &buffers, boost::system::error_code &ec) {
        return write_some(beast::buffers_to_string(buffers), ec);
    }
};

TEST(AConnection, WriteLineCallsWriteSome) {
    NiceMock<MockStream> mock_stream;

    auto expected = std::string("hello\n");
    EXPECT_CALL(mock_stream, write_some(expected, _))
        .WillOnce(Return(expected.size()));

    Connection<NiceMock<MockStream>> conn(&mock_stream);
    write_line(conn, "hello"sv);
}

TEST(AConnection, PeekLineReadsUntilNewline) {
    NiceMock<MockStream> mock_stream;

    // Simulate reading data in chunks
    std::string data = "first line\nsecond line\n";

    EXPECT_CALL(mock_stream, read_some)
        .WillOnce([&](boost::asio::mutable_buffer buf,
                      boost::system::error_code &ec) {
            memcpy(buf.data(), data.data(), data.size());
            return data.size();
        });

    Connection<NiceMock<MockStream>> conn(&mock_stream);
    EXPECT_EQ(conn.peek_line(), "first line");
    EXPECT_EQ(conn.peek_line(), "first line");
    conn.consume();
    EXPECT_EQ(conn.peek_line(), "second line");
    conn.consume();
}

#include <client/socket_connection.hpp>
#include <client/tls_connection.hpp>
#include <server/socket_server.hpp>
#include <server/tls_server.hpp>

#include "config.h"

TEST(ASocketConnection, ConnectsToServer) {
    TestTcpServer server(12345);
    boost::asio::io_context ioc;
    EXPECT_NO_THROW({ SocketConnection conn(ioc, "localhost", "12345"); });
}

TEST(ATlsConnection, ConnectsToServer) {
    TestTlsServer server(12346, TEST_CERT_PATH "client.crt",
                         TEST_CERT_PATH "client.key");
    boost::asio::io_context ioc;
    EXPECT_NO_THROW({
        TlsConnection conn(ioc, "localhost", "12346", TEST_CERT_PATH "ca.pem",
                           TEST_CERT_PATH "client.crt",
                           TEST_CERT_PATH "client.key");
    });
}
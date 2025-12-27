#pragma once

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/core/detail/stream_traits.hpp>
#include <boost/beast/ssl.hpp>

#include <concepts>
#include <stdexcept>
#include <string>
#include <string_view>

template <typename T>
concept ConstBufferSequence =
    std::destructible<T> && std::copy_constructible<T> && requires(T x) {
        {
            boost::asio::buffer_sequence_begin(x)
        } -> std::bidirectional_iterator;
        { boost::asio::buffer_sequence_end(x) } -> std::bidirectional_iterator;
        requires std::convertible_to<
            std::iter_value_t<decltype(boost::asio::buffer_sequence_begin(x))>,
            boost::asio::const_buffer>;
    };

template <typename T, typename Buffers = std::vector<boost::asio::const_buffer>>
concept ConnectionType = //
    ConstBufferSequence<Buffers> &&
    requires(T conn, const Buffers &buffers) {
        { conn.peek_line() } -> std::same_as<std::string_view>;
        { conn.consume() } -> std::same_as<void>;
        { conn.write(buffers) } -> std::same_as<void>;
    };

namespace beast = boost::beast;
namespace net = boost::asio;
namespace ssl = net::ssl;
using tcp = net::ip::tcp;

template <typename T>
concept SafeString = std::is_same_v<std::decay_t<T>, std::string> ||
                     std::is_same_v<std::decay_t<T>, std::string_view>;

template <ConnectionType Conn, SafeString... Strings>
void write_line(Conn &conn, const Strings &...strs) {
    using namespace std::literals::string_view_literals;
    constexpr size_t N = sizeof...(strs);
    std::array<net::const_buffer, N + 1> buffers{net::buffer(strs)...,
                                                 net::buffer("\n"sv)};
    conn.write(buffers);
}

template <typename T, typename Buffers = std::vector<boost::asio::const_buffer>>
concept StreamType = //
    ConstBufferSequence<Buffers> &&
    requires(T stream, net::mutable_buffer buf, boost::system::error_code &ec,
             Buffers buffers) {
        { stream.read_some(buf, ec) } -> std::same_as<size_t>;
        { stream.write_some(buffers, ec) } -> std::same_as<size_t>;
    };

template <StreamType Stream> class Connection {
  public:
    using stream_type = Stream;

    Connection(Stream *stream) : stream_ptr_(stream), buffer_(4096) {}
    virtual ~Connection() = default;

    std::string_view peek_line() {
        while (true) {
            auto buf = buffer_.data();
            const char *begin = static_cast<const char *>(buf.data());
            const char *end = begin + buf.size();
            const char *newline =
                static_cast<const char *>(memchr(begin, '\n', buf.size()));
            if (newline) {
                line_len_ = newline - begin + 1;
                return std::string_view(begin, line_len_ - 1);
            }
            boost::system::error_code ec;
            size_t bytes = stream_ptr_->read_some(buffer_.prepare(1024), ec);
            if (ec)
                throw std::runtime_error("Read error: " + ec.message());
            buffer_.commit(bytes);
        }
    }

    void consume() noexcept {
        buffer_.consume(line_len_);
        line_len_ = 0;
    }

    template <ConstBufferSequence Buffers> //
    void write(const Buffers &buffers) {
        boost::system::error_code ec;
        boost::asio::write(*stream_ptr_, buffers, ec);
        if (ec)
            throw std::runtime_error("Write error: " + ec.message());
    }

  protected:
    Stream *stream_ptr_;
    beast::flat_buffer buffer_;
    size_t line_len_{0};
};

static_assert(ConnectionType<Connection<beast::tcp_stream>>,
              "Connection does not satisfy ConnectionType concept");
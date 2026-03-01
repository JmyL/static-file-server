#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/stream_file.hpp>
#include <chrono>
#include <cstdlib>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace net = boost::asio;
using net::awaitable;
using net::co_spawn;
using net::detached;
using net::use_awaitable;
using net::ip::tcp;
namespace this_coro = boost::asio::this_coro;

using namespace std::string_view_literals;

const int max_length = 1024;

struct LogContext {
    std::string trace_id;
    std::string span_id;
};

thread_local LogContext log_context;

void clear_log_context() {
    log_context.trace_id.clear();
    log_context.span_id.clear();
}

template <size_t N>
void append_literal(spdlog::memory_buf_t &dest, const char (&text)[N]) {
    dest.append(text, text + N - 1);
}

template <typename StringView>
void json_escape_to(StringView input, spdlog::memory_buf_t &dest) {
    auto data = input.data();
    auto size = input.size();
    for (size_t i = 0; i < size; ++i) {
        auto c = data[i];
        switch (c) {
        case '"':
            append_literal(dest, "\\\"");
            break;
        case '\\':
            append_literal(dest, "\\\\");
            break;
        case '\n':
            append_literal(dest, "\\n");
            break;
        case '\r':
            append_literal(dest, "\\r");
            break;
        case '\t':
            append_literal(dest, "\\t");
            break;
        default:
            dest.push_back(c);
            break;
        }
    }
}

class json_formatter final : public spdlog::formatter {
  public:
    void format(const spdlog::details::log_msg &msg,
                spdlog::memory_buf_t &dest) override {
        auto ts = msg.time;
        auto epoch_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                            ts.time_since_epoch())
                            .count();

        append_literal(dest, "{");
        append_literal(dest, "\"ts\":");
        dest.append(std::to_string(epoch_ms));

        append_literal(dest, ",\"level\":\"");
        auto level_view = spdlog::level::to_string_view(msg.level);
        json_escape_to(std::string_view(level_view.data(), level_view.size()),
                       dest);
        append_literal(dest, "\"");

        append_literal(dest, ",\"msg\":\"");
        json_escape_to(std::string_view(msg.payload.data(), msg.payload.size()),
                       dest);
        append_literal(dest, "\"");

        if (!log_context.trace_id.empty()) {
            append_literal(dest, ",\"trace_id\":\"");
            json_escape_to(log_context.trace_id, dest);
            append_literal(dest, "\"");
        }

        if (!log_context.span_id.empty()) {
            append_literal(dest, ",\"span_id\":\"");
            json_escape_to(log_context.span_id, dest);
            append_literal(dest, "\"");
        }

        append_literal(dest, "}\n");
    }

    std::unique_ptr<spdlog::formatter> clone() const override {
        return std::make_unique<json_formatter>();
    }
};

spdlog::level::level_enum parse_log_level(std::string_view level) {
    if (level == "trace") {
        return spdlog::level::trace;
    }
    if (level == "debug") {
        return spdlog::level::debug;
    }
    if (level == "info") {
        return spdlog::level::info;
    }
    if (level == "warn" || level == "warning") {
        return spdlog::level::warn;
    }
    if (level == "error") {
        return spdlog::level::err;
    }
    if (level == "critical") {
        return spdlog::level::critical;
    }
    if (level == "off") {
        return spdlog::level::off;
    }
    return spdlog::level::err;
}

std::shared_ptr<spdlog::logger>
init_logger(const std::optional<std::string> &log_file,
            spdlog::level::level_enum level) {
    auto queue_size = 8192;
    auto thread_count = 1;
    spdlog::init_thread_pool(queue_size, thread_count);

    std::shared_ptr<spdlog::sinks::sink> sink;
    if (log_file && !log_file->empty()) {
        sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(*log_file,
                                                                   true);
    } else {
        sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    }

    auto logger = std::make_shared<spdlog::async_logger>(
        "awaitable_server_logger", sink, spdlog::thread_pool(),
        spdlog::async_overflow_policy::overrun_oldest);

    logger->set_level(level);
    logger->set_formatter(std::make_unique<json_formatter>());
    spdlog::set_default_logger(logger);
    return logger;
}

template <typename Executor> struct ReadmeContent {

    ReadmeContent(Executor &executor) {
        auto file = net::stream_file(executor, "README.md",
                                     net::stream_file::flags::read_only);
        buffer.resize(file.size());

        net::read(file, net::buffer(buffer));
    }

    const std::vector<char> &get() { return buffer; }

    std::vector<char> buffer;
};

template <typename Executor>
awaitable<void> session(tcp::socket sock, ReadmeContent<Executor> &content) {
    try {
        auto sbuf = net::streambuf(max_length);
        for (;;) {
            auto len = co_await net::async_read_until(sock, sbuf, "\r\n\r\n",
                                                      use_awaitable);
            SPDLOG_INFO("Received {} bytes", len);
            sbuf.consume(len);

            try {
                auto executor = co_await this_coro::executor;
                // open_file(io_context
                constexpr auto header_template =
                    "HTTP/1.1 200 OK\r\n"
                    "Server: nginx/1.29.4\r\n"
                    "Date: Sat, 03 Jan 2026 15:22:53 GMT\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: {}\r\n"
                    "Last-Modified: Sun, 28 Dec 2025 17:25:28 GMT\r\n"
                    "Connection: keep-alive\r\n"
                    "ETag: \"69516808-4a8\"\r\n"
                    "Accept-Ranges: bytes\r\n"
                    "\r\n"sv;

                auto buffer = content.get();
                auto header = std::format(header_template, buffer.size());

                auto write_len = co_await net::async_write(
                    sock,
                    std::array<net::const_buffer, 2>{
                        net::buffer(header.data(), header.size()),
                        net::buffer(buffer.data(), buffer.size())},
                    use_awaitable);
                SPDLOG_INFO("Sent {} bytes", write_len);
            } catch (const boost::system::system_error &e) {
                SPDLOG_ERROR("Error opening file: {}", e.what());
                // TODO: Do proper error handling here,
                // e.g., send a 404 Not Found response to the client.
                co_return;
            }
        }
    } catch (boost::system::system_error &e) {
        if (e.code() == net::error::eof) {
            SPDLOG_INFO("Connection closed by peer.");
            co_return;
        } else {
            SPDLOG_ERROR("Boost System Error in thread: {}", e.what());
            co_return;
        }
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception in thread: {}", e.what());
    }
}

awaitable<void> server(unsigned short port) {
    auto executor = co_await this_coro::executor;
    auto content = ReadmeContent(executor);
    tcp::acceptor a(executor, tcp::endpoint(tcp::v4(), port));
    for (;;) {
        auto socket = co_await a.async_accept(use_awaitable);
        co_spawn(executor, session(std::move(socket), content), detached);
    }
}

int main(int argc, char *argv[]) {
    try {
        if (argc < 2) {
            std::cerr
                << "Usage: blocking_tcp_echo_server <port> [--log-file <path>] "
                   "[--log-level <level>]\n";
            return 1;
        }

        std::optional<std::string> log_file;
        auto log_level = spdlog::level::err;

        for (int i = 2; i < argc; ++i) {
            std::string_view arg = argv[i];
            if (arg == "--log-file" && i + 1 < argc) {
                log_file = argv[++i];
                continue;
            }
            if (arg == "--log-level" && i + 1 < argc) {
                log_level = parse_log_level(argv[++i]);
                continue;
            }
        }

        init_logger(log_file, log_level);

        net::io_context io_context;

        std::atomic<bool> shutdown_requested{false};

        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto) {
            if (!shutdown_requested.exchange(true)) {
                SPDLOG_INFO("Shutdown signal received");
                io_context.stop();
                spdlog::shutdown();
            }
        });

        co_spawn(io_context, server(std::atoi(argv[1])), detached);

        std::vector<std::thread> threads;
        auto thread_count = std::thread::hardware_concurrency();
        for (unsigned i = 0; i < thread_count; ++i) {
            threads.emplace_back([&io_context]() { io_context.run(); });
        }
        for (auto &t : threads)
            t.join();
    } catch (std::exception &e) {
        SPDLOG_ERROR("Exception: {}", e.what());
    }

    return 0;
}

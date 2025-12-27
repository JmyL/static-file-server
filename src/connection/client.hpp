#pragma once

#include "connection.hpp"
#include <util/string_view.hpp>

#include <iostream>

template <ConnectionType Conn> //
void run_client(Conn &conn) {
    using namespace std::literals::string_view_literals;
    std::string authdata;
    while (true) {
        try {
            auto line = conn.peek_line();
            auto tokens = TokenSplitter(line);
            auto cmd = tokens.next();

            if (cmd == "HELO") {
                if (tokens.has_next()) {
                    std::cerr << "Unexpected arguments for HELO command: "
                              << tokens.rest() << std::endl;
                    break;
                }
                write_line(conn, "EHLO"sv);

            } else if (cmd == "ERROR") {
                std::cerr << "ERROR: " << tokens.rest() << std::endl;
                break;
            }

        } catch (boost::system::system_error &e) {
            if (e.code() == boost::asio::error::eof ||
                e.code() == boost::asio::ssl::error::stream_truncated) {
                std::cout << "Connection closed by peer." << std::endl;
                break;
            } else {
                std::cerr << "Boost System Error: " << e.what() << std::endl;
                break;
            }
        } catch (std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
            break;
        }
        conn.consume();
    }
}
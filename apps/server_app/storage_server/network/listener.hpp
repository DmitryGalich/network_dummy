#pragma once

#include <memory>
#include <string>
#include <bits/shared_ptr.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>

#include "sessions_manager.hpp"

class Listener : public std::enable_shared_from_this<Listener>
{
public:
    Listener() = delete;
    Listener(
        boost::asio::io_context &io_context);
    ~Listener() = default;

    bool run(const boost::asio::ip::tcp::endpoint &endpoint);

private:
    void prepare_for_accept();
    void process_accept(const boost::system::error_code &error_code);
    void process_fail(const boost::system::error_code &error_code,
                      char const *reason);

private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;

    SessionsManager sessions_manager_;
};
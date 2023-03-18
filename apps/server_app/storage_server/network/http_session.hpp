#pragma once

#include <memory>
#include <bits/shared_ptr.h>

#include <boost/asio/ip/tcp.hpp>
#include <boost/system/error_code.hpp>

#include "sessions_manager.hpp"

class HttpSession : public std::enable_shared_from_this<HttpSession>
{
public:
    HttpSession() = delete;
    HttpSession(SessionsManager &sessions_manager,
                boost::asio::ip::tcp::socket &socket);
    ~HttpSession() = default;

    void run();

private:
    void process_fail(const boost::system::error_code &error_code,
                      char const *reason);
    void process_read(const boost::system::error_code &eerror_codec,
                      std::size_t);
    void process_write(const boost::system::error_code &error_code,
                       std::size_t,
                       bool is_need_close);

private:
    SessionsManager &sessions_manager_;
    boost::asio::ip::tcp::socket &socket_;
};
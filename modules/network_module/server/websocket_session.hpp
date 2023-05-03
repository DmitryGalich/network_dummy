#pragma once

#include <vector>
#include <memory>
#include <functional>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/websocket/stream.hpp>
#include <boost/beast/http/dynamic_body.hpp>
#include <boost/beast/http/fields.hpp>
#include <boost/beast/websocket/impl/accept.hpp>

class WebSocketSession : public std::enable_shared_from_this<WebSocketSession>
{
public:
    WebSocketSession() = delete;
    WebSocketSession(boost::asio::ip::tcp::socket socket);
    ~WebSocketSession();

    template <class Body, class Allocator>
    void run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> request);

    void send(std::shared_ptr<std::string const> const &data);

private:
    void do_accept(boost::system::error_code error_code);
    void prepare_for_reading();
    void do_read(boost::system::error_code error_code, std::size_t bytes_transferred);
    void do_write(boost::system::error_code error_code, std::size_t bytes_transferred);

private:
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> websocket_;
    boost::beast::flat_buffer buffer_;
    std::vector<std::shared_ptr<std::string const>> queue_;
};

template <class Body, class Allocator>
void WebSocketSession::run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> request)
{
    websocket_.async_accept(
        request,
        std::bind(
            &WebSocketSession::do_accept,
            this,
            std::placeholders::_1));
}
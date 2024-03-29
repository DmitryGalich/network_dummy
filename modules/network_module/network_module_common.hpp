#pragma once

#include <string>
#include <functional>

namespace network_module
{
    typedef std::string Url;
    typedef std::function<std::string()> HttpCallback;

    typedef std::function<void()> SignalToStop;

    namespace web_sockets
    {
        typedef std::function<void(const std::string &)> ReceivingCallback;
        typedef std::function<std::string()> SendingCallback;

        typedef std::function<void()> OnStartCallback;
    }

    struct Urls
    {
        static const Url kPageNotFound_;
    };
}
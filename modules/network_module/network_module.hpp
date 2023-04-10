#pragma once

#include <memory>
#include <string>

namespace network_module
{
    // Server

    class Server
    {
    public:
        struct Config
        {
            std::string host_{"127.0.0.1"};
            int port_{8080};
            bool is_ip_v6_{false};
        };

    public:
        Server(const int &available_processors_cores);
        ~Server();

        bool start(const Config &config);
        void stop();

    private:
        class ServerImpl;
        std::unique_ptr<ServerImpl> server_impl_;
    };

    // Client

    // class Client
    // {
    // public:
    //     struct Config
    //     {
    //         std::string host_{"127.0.0.1"};
    //         int port_{8080};
    //         bool is_ip_v6_{false};
    //     };

    // public:
    //     Client();
    //     ~Client();

    //     bool start(const Config &config);
    //     void stop();

    // private:
    //     class ClientImpl;
    //     std::unique_ptr<ClientImpl> client_impl_;
    // };
}
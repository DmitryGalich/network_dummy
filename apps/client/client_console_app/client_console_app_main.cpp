#include <future>
#include <thread>
#include <chrono>

#include "configs/cmake_config.h"

#include "easylogging++.h"
#define ELPP_THREAD_SAFE

#include "dummy_client/dummy_client.hpp"

INITIALIZE_EASYLOGGINGPP

void configure_logger()
{
    el::Configurations config(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/log_config.conf"});
    el::Loggers::reconfigureAllLoggers(config);
}

void wait_for_user_command(dummy::client::Client &client)
{
    while (true)
    {
        std::string input;
        getline(std::cin, input);

        if (input == "q" ||
            input == "Q" ||
            input == "c" ||
            input == "C")
        {
            return;
        }
        else
        {
            try
            {
                if (!client.send(input))
                {
                    LOG(ERROR) << "Error while sending message to server";
                }
            }
            catch (...)
            {
                LOG(ERROR) << "Can't send message to server";
            }
        }
    }
}

int main()
{
    try
    {
        configure_logger();

        LOG(INFO) << "================================";
        LOG(INFO) << PROJECT_NAME;
        LOG(INFO) << "version " << PROJECT_VERSION;

        std::promise<void> client_promise;
        auto client_future = client_promise.get_future();

        dummy::client::Client client(std::move(client_promise));

        try
        {
            if (!client.start(CMAKE_CURRENT_SOURCE_DIR + std::string{"/configs/client_config.json"}))
            {
                LOG(ERROR) << "Can't start client";
                LOG(INFO) << "Shutting down the application";
                client.stop();
                return -1;
            }
        }
        catch (const std::exception &e)
        {
            LOG(ERROR) << e.what();
            LOG(INFO) << "Shutting down the application";
            client.stop();
            return -1;
        }

        std::future<void> user_command_future = std::async(&wait_for_user_command, std::ref(client));
        while (true)
        {
            if (user_command_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
            {
                LOG(INFO) << "Signal to stop from user command";
                break;
            }

            if (client_future.wait_for(std::chrono::seconds(1)) == std::future_status::ready)
            {
                LOG(INFO) << "Signal to stop from client command";
                break;
            }
        }

        client.stop();
    }
    catch (const std::exception &e)
    {
        LOG(ERROR) << "Global error: " << e.what();
    }

    exit(0);
}

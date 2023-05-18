#include "storage_client.hpp"

#include "easylogging++.h"

#include "network_module.hpp"

namespace storage
{
    namespace client
    {
        class Client::ClientImpl
        {
        public:
            ClientImpl();
            ~ClientImpl();

            bool start(const std::string &config_path);
            void stop() noexcept;

        private:
            void configure_callbacks(network_module::client::Client::Config &config);

            void start_communication();

            void send(const std::string &data);
            void receive(const std::string &data);

        private:
            std::unique_ptr<network_module::client::Client> network_module_;
        };

        Client::ClientImpl::ClientImpl() {}

        Client::ClientImpl::~ClientImpl() {}

        bool Client::ClientImpl::start(const std::string &config_path)
        {
            LOG(INFO) << "Starting...";

            network_module_.reset(new network_module::client::Client());
            if (!network_module_)
            {
                LOG(ERROR) << "Can't create network module";
                return false;
            }

            auto config =
                network_module::client::Client::Config::load_config(config_path);

            configure_callbacks(config);

            if (!network_module_->start(config))
                return false;

            return true;
        }

        void Client::ClientImpl::stop() noexcept
        {
            LOG(INFO) << "Stopping...";

            if (network_module_)
            {
                network_module_->stop();
            }
            network_module_.reset();

            LOG(INFO) << "Stopped";
        }

        void Client::ClientImpl::configure_callbacks(network_module::client::Client::Config &config)
        {
            config.callbacks_.on_start_ = std::bind(&Client::ClientImpl::start_communication, this);
            config.callbacks_.process_receiving_ = std::bind(&Client::ClientImpl::receive, this, std::placeholders::_1);
        }

        void Client::ClientImpl::start_communication()
        {
            LOG(INFO) << "Started";

            // send("Hello from client");
        }

        void Client::ClientImpl::send(const std::string &data)
        {
            if (!network_module_)
            {
                LOG(ERROR) << "network_module is not created";
                return;
            }

            network_module_->send(data);
        }

        void Client::ClientImpl::receive(const std::string &data)
        {
            LOG(INFO) << "Received data: " << data;
        }

    }
}

namespace storage
{
    namespace client
    {
        Client::Client() : client_impl_(std::make_unique<storage::client::Client::ClientImpl>()) {}

        Client::~Client() {}

        bool Client::start(const std::string &config_path)
        {
            if (!client_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return client_impl_->start(config_path);
        }

        void Client::stop() noexcept
        {
            if (!client_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            client_impl_->stop();
        }
    }
}

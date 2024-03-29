#include "dummy_client.hpp"

#include "easylogging++.h"

#include "network_module.hpp"

namespace dummy
{
    namespace client
    {
        class Client::ClientImpl
        {
        public:
            ClientImpl() = delete;
            ClientImpl(std::promise<void> signal_to_stop);
            ~ClientImpl();

            bool start(const std::string &config_path);
            void stop() noexcept;

            bool send(const std::string &data);

        private:
            void configure_callbacks(network_module::client::Client::Config &config);

            void process_signal_to_stop();

            void start_communication();

            void receive(const std::string &data);

        private:
            std::unique_ptr<network_module::client::Client> network_module_;

            std::atomic_bool is_stop_signal_called_{false};
            std::promise<void> signal_to_stop_;
        };

        Client::ClientImpl::ClientImpl(std::promise<void> signal_to_stop)
            : signal_to_stop_(std::move(signal_to_stop)) {}

        Client::ClientImpl::~ClientImpl()
        {
            stop();
        }

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

            LOG(INFO) << "Started";
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
            is_stop_signal_called_ = false;
            config.callbacks_.signal_to_stop_ = std::bind(&Client::ClientImpl::process_signal_to_stop, this);

            config.callbacks_.on_start_ = std::bind(&Client::ClientImpl::start_communication, this);
            config.callbacks_.process_receiving_ = std::bind(&Client::ClientImpl::receive, this, std::placeholders::_1);
        }

        void Client::ClientImpl::process_signal_to_stop()
        {
            if (is_stop_signal_called_)
                return;

            is_stop_signal_called_ = true;
            signal_to_stop_.set_value();
        }

        void Client::ClientImpl::start_communication()
        {
            LOG(INFO) << "Ready to work";
            send("Hello from client");
        }

        bool Client::ClientImpl::send(const std::string &data)
        {
            if (!network_module_)
            {
                LOG(ERROR) << "Network module is not created";
                return false;
            }

            return network_module_->send(data);
        }

        void Client::ClientImpl::receive(const std::string &data)
        {
            LOG(INFO) << "Received data: " << data;
        }
    }
}

namespace dummy
{
    namespace client
    {
        Client::Client(std::promise<void> signal_to_stop) : client_impl_(std::make_unique<dummy::client::Client::ClientImpl>(std::move(signal_to_stop))) {}

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

        bool Client::send(const std::string &message)
        {
            if (!client_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return client_impl_->send(message);
        }

    }
}

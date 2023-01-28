#include "client.h"

#include "easylogging++.h"

#include "internal/client_fabric.h"
#include "internal/abstract_client.h"

namespace cloud
{
    class Client::ClientImpl
    {
    public:
        ClientImpl();
        ClientImpl(const std::string &config_path);
        ~ClientImpl() = default;

        bool start();
        void stop() noexcept;

    private:
        std::unique_ptr<cloud::internal::AbstractClient> client_;
    };

    Client::ClientImpl::ClientImpl() {}

    Client::ClientImpl::ClientImpl(const std::string &config_path)
    {
        LOG(INFO) << config_path;
    }

    bool Client::ClientImpl::start()
    {
        LOG(INFO) << "Starting...";

        client_.reset(cloud::internal::create_client());
        if (!client_)
        {
            static const std::string kErrorText("Client not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        return client_->start();
    }

    void Client::ClientImpl::stop() noexcept
    {
        LOG(INFO) << "Stopping...";

        if (!client_)
        {
            LOG(WARNING) << "Client already null";
            return;
        }

        try
        {
            client_->stop();
        }
        catch (const std::exception &e)
        {
            LOG(ERROR) << "Error while stopping client. " << e.what();
            return;
        }

        LOG(INFO) << "Stopped";
    }
}

namespace cloud
{
    Client::Client() : client_impl_(std::make_unique<Client::ClientImpl>())
    {
    }

    Client::Client(const std::string &config_path) : client_impl_(std::make_unique<Client::ClientImpl>(config_path)) {}

    Client::~Client()
    {
    }

    bool Client::start()
    {
        if (!client_impl_)
        {
            static const std::string kErrorText("Implementation is not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        return client_impl_->start();
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
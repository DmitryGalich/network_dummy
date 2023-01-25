#include "client.h"

#include "easylogging++.h"

#include "internal/client_fabric.h"
#include "internal/abstract_client.h"

namespace cloud
{
    class ClientImpl
    {
    public:
        ClientImpl();
        ~ClientImpl();

        void start();
        void stop();

    private:
        std::unique_ptr<internal::AbstractClient> client_;
    };

    ClientImpl::ClientImpl()
    {
        LOG(DEBUG) << "Constructor";
    }

    ClientImpl::~ClientImpl()
    {
        LOG(DEBUG) << "Destructor";
    }

    void ClientImpl::start()
    {
        client_.reset(internal::create_client());
        if (!client_)
        {
            static const std::string kErrorText("Client not created");
            LOG(ERROR) << kErrorText;
            throw std::runtime_error(kErrorText);
        }

        client_->start();
        LOG(INFO) << "Started";
    }

    void ClientImpl::stop()
    {
        if (!client_)
        {
            LOG(WARNING) << "Client already null";
            return;
        }

        client_->stop();
        LOG(INFO) << "Stopped";
    }

    // Outside

    Client::Client() : client_impl_(std::make_unique<ClientImpl>())
    {
        LOG(DEBUG) << "Constructor";
    }

    Client::~Client()
    {
        LOG(DEBUG) << "Destructor";
    }

    void Client::start()
    {
        client_impl_->start();
        LOG(INFO) << "Started";
    }

    void Client::stop()
    {
        client_impl_->stop();
        LOG(INFO) << "Stopped";
    }
}
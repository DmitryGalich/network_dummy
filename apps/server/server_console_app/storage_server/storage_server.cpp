#include "storage_server.hpp"

#include "easylogging++.h"

#include "network_module.hpp"
#include "pages_manager/pages_manager.hpp"

namespace storage
{
    namespace server
    {
        class Server::ServerImpl
        {
        public:
            ServerImpl() = default;
            ~ServerImpl() = default;

            bool start(const int available_processors_cores,
                       const std::string &config_path,
                       const std::string &html_folder_path);
            void stop() noexcept;

        private:
            std::unique_ptr<network_module::server::Server> network_module_;
            PagesManager pages_manager_;
        };

        bool Server::ServerImpl::start(const int available_processors_cores,
                                       const std::string &config_path,
                                       const std::string &html_folder_path)
        {
            LOG(INFO) << "Starting...";

            network_module_.reset(new network_module::server::Server());
            if (!network_module_)
                return false;

            auto config =
                network_module::server::Server::Config::load_config(config_path);

            config.http_callbacks_.push_back({"/", [&]()
                                              {
                                                  return pages_manager_.getHomePage(html_folder_path + "index.html");
                                              }});

            if (!network_module_->start(available_processors_cores,
                                        config))
                return false;

            return true;
        }

        void Server::ServerImpl::stop() noexcept
        {
            LOG(INFO) << "Stopping...";

            if (network_module_)
            {
                network_module_->stop();
            }
            network_module_.reset();

            LOG(INFO) << "Stopped";
        }
    }
}

namespace storage
{
    namespace server
    {
        Server::Server() : server_impl_(std::make_unique<storage::server::Server::ServerImpl>()) {}

        Server::~Server() {}

        bool Server::start(const int available_processors_cores,
                           const std::string &config_path,
                           const std::string &html_folder_path)
        {
            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start(available_processors_cores,
                                       config_path,
                                       html_folder_path);
        }

        void Server::stop() noexcept
        {
            if (!server_impl_)
            {
                LOG(ERROR) << "Implementation is not created";
                return;
            }

            server_impl_->stop();
        }
    }
}

#include "oatpp_server.h"

#include "easylogging++.h"

#include "oatpp/core/base/Environment.hpp"
#include "oatpp/core/async/Executor.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/parser/json/mapping/ObjectMapper.hpp"
#include "oatpp/web/server/AsyncHttpConnectionHandler.hpp"
#include "oatpp-websocket/AsyncConnectionHandler.hpp"
#include "oatpp/network/ConnectionProvider.hpp"
#include "oatpp/network/Server.hpp"

// #include "oatpp/network/ConnectionHandler.hpp"
// #include "oatpp-websocket/AsyncWebSocket.hpp"

#include "../abstract_server.h"
#include "websocket_listener.hpp"
#include "server_api_controller.hpp"

namespace cloud
{
    namespace internal
    {
        class OatppServer::OatppServerImpl
        {
        public:
            OatppServerImpl() = delete;
            OatppServerImpl(const ServerConfig &config);
            ~OatppServerImpl() = default;

            bool start();
            void stop();

        private:
            bool init();
            bool run();

        private:
            const ServerConfig kConfig_;

            std::shared_ptr<oatpp::async::Executor> async_executor_;
            std::shared_ptr<oatpp::network::tcp::server::ConnectionProvider> connection_provider_;
            std::shared_ptr<oatpp::web::server::HttpRouter> router_;
            std::shared_ptr<oatpp::network::ConnectionHandler> connection_handler_;
            std::shared_ptr<oatpp::data::mapping::ObjectMapper> object_mapper_;
            std::shared_ptr<oatpp::websocket::AsyncConnectionHandler> websocket_connection_handler_;
            std::shared_ptr<WSInstanceListener> websocket_instance_listener_;
            std::shared_ptr<ServerApiController> server_api_controller_;
            std::shared_ptr<oatpp::network::Server> server_;
        };

        OatppServer::OatppServerImpl::OatppServerImpl(const ServerConfig &config) : kConfig_(config) {}

        bool OatppServer::OatppServerImpl::init()
        {
            oatpp::base::Environment::init();

            async_executor_.reset();
            async_executor_ = std::make_shared<oatpp::async::Executor>(
                kConfig_.executor_data_processing_threads_,
                kConfig_.executor_io_threads_,
                kConfig_.executor_timer_threads_);
            if (!async_executor_)
            {
                LOG(ERROR) << "Executor is not created";
                return false;
            }

            connection_provider_.reset();
            connection_provider_ = oatpp::network::tcp::server::ConnectionProvider::createShared(
                {kConfig_.host_,
                 static_cast<v_uint16>(kConfig_.port_),
                 (kConfig_.is_ip_v6_family_ ? oatpp::network::Address::IP_6 : oatpp::network::Address::IP_4)});
            if (!connection_provider_)
            {
                LOG(ERROR) << "ConnectionProvider is not created";
                return false;
            }

            router_.reset();
            router_ = oatpp::web::server::HttpRouter::createShared();
            if (!router_)
            {
                LOG(ERROR) << "HttpRouter is not created";
                return false;
            }

            connection_handler_.reset();
            connection_handler_ = oatpp::web::server::AsyncHttpConnectionHandler::createShared(router_, async_executor_);
            if (!connection_handler_)
            {
                LOG(ERROR) << "AsyncHttpConnectionHandler is not created";
                return false;
            }

            object_mapper_.reset();
            object_mapper_ = oatpp::parser::json::mapping::ObjectMapper::createShared();
            if (!object_mapper_)
            {
                LOG(ERROR) << "ObjectMapper is not created";
                return false;
            }

            websocket_connection_handler_.reset();
            websocket_connection_handler_ = oatpp::websocket::AsyncConnectionHandler::createShared(async_executor_);
            if (!websocket_connection_handler_)
            {
                LOG(ERROR) << "websocket::AsyncConnectionHandler is not created";
                return false;
            }

            websocket_instance_listener_.reset();
            websocket_instance_listener_ = std::make_shared<WSInstanceListener>();
            if (!websocket_instance_listener_)
            {
                LOG(ERROR) << "WSInstanceListener is not created";
                return false;
            }

            websocket_connection_handler_->setSocketInstanceListener(websocket_instance_listener_);

            server_api_controller_.reset();
            server_api_controller_ = std::make_shared<ServerApiController>();
            if (!server_api_controller_)
            {
                LOG(ERROR) << "ServerApiController is not created";
                return false;
            }

            // server_.reset();
            // server_ = oatpp::network::Server::createShared(connection_provider_, connection_handler_);
            // if (!server_)
            // {
            //     LOG(ERROR) << "Server is not created";
            //     return false;
            // }

            return true;
        }

        bool OatppServer::OatppServerImpl::run()
        {
            server_->run();

            return true;
        }

        bool OatppServer::OatppServerImpl::start()
        {
            if (!init())
                return false;

            if (!run())
                return false;

            return true;
        }

        void OatppServer::OatppServerImpl::stop()
        {
            if (server_)
                server_->stop();

            server_.reset();
            server_api_controller_.reset();
            websocket_instance_listener_.reset();
            websocket_connection_handler_.reset();
            object_mapper_.reset();
            connection_handler_.reset();
            router_.reset();
            connection_provider_.reset();

            async_executor_->waitTasksFinished();
            async_executor_->stop();
            async_executor_->join();
            async_executor_.reset();

            oatpp::base::Environment::destroy();
        }
    }
}

namespace cloud
{
    namespace internal
    {
        OatppServer::OatppServer(const ServerConfig &config) : server_impl_(std::make_unique<OatppServer::OatppServerImpl>(config))
        {
        }
        OatppServer::~OatppServer()
        {
        }

        bool OatppServer::start()
        {
            if (!server_impl_)
            {
                static const std::string kErrorText("Implementation is not created");
                LOG(ERROR) << kErrorText;
                throw std::runtime_error(kErrorText);
            }

            return server_impl_->start();
        }
        void OatppServer::stop()
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
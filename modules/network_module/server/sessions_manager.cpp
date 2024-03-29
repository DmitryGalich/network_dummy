#include "sessions_manager.hpp"

#include "easylogging++.h"

#include "websocket_session.hpp"
#include "http_session.hpp"

bool SessionsManager::add(std::shared_ptr<WebSocketSession> session)
{
    bool status = false;

    {
        std::lock_guard<std::mutex> lock(websocket_mutex_);
        status = websocket_sessions_.insert(session).second;
    }

    return status;
}

void SessionsManager::remove(WebSocketSession *session)
{
    std::lock_guard<std::mutex> lock(websocket_mutex_);

    for (auto iter = websocket_sessions_.begin(); iter != websocket_sessions_.end(); ++iter)
    {
        if ((*iter).get() == session)
        {
            websocket_sessions_.erase(*iter);
            break;
        }
    }
}

bool SessionsManager::send(const std::string &message)
{
    if (websocket_sessions_.empty())
    {
        LOG(ERROR) << "Connections list is empty";
        return false;
    }

    auto const ss = std::make_shared<std::string const>(std::move(message));

    for (auto iter = websocket_sessions_.begin(); iter != websocket_sessions_.end(); ++iter)
    {
        (*iter)->send(ss);
    }

    return true;
}

void SessionsManager::clear()
{
    websocket_sessions_.clear();
}

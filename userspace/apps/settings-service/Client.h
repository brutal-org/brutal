#pragma once

#include <libipc/Peer.h>
#include <libsettings/Protocol.h>
#include <libutils/Vec.h>

namespace Settings
{

struct Client : public IPC::Peer<Protocol>
{
private:
    Vec<Path> _subscriptions;

public:
    Func<void(Client &, const Message &message)> on_message;
    Func<void()> on_disconnect;

    Client(IO::Connection connection) : Peer{std::move(connection)}
    {
    }

    void handle_message(const Message &message) override
    {
        on_message(*this, message);
    }

    void handle_disconnect() override
    {
        on_disconnect();
    }

    void subscribe(const Settings::Path &path)
    {
        _subscriptions.push_back(path);
    }

    void unsubscribe(const Settings::Path &path)
    {
        _subscriptions.remove_all_value(path);
    }

    bool is_subscribe(const Settings::Path &path)
    {
        for (size_t i = 0; i < _subscriptions.count(); i++)
        {
            if (path.match(_subscriptions[i]))
            {
                return true;
            }
        }

        return false;
    }
};

} // namespace Settings

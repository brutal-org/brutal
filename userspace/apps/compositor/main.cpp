#include <abi/Paths.h>
#include <assert.h>

#include <libasync/Loop.h>
#include <libasync/Notifier.h>
#include <libasync/Timer.h>
#include <libio/Connection.h>
#include <libio/File.h>
#include <libio/Socket.h>
#include <libio/Streams.h>
#include <libsystem/io/Stream.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

#include "compositor/Client.h"
#include "compositor/Cursor.h"
#include "compositor/Manager.h"
#include "compositor/Renderer.h"
#include "compositor/Window.h"

static Widget::EventType key_motion_to_event_type(KeyMotion motion)
{
    if (motion == KEY_MOTION_DOWN)
    {
        return Widget::Event::KEYBOARD_KEY_PRESS;
    }

    if (motion == KEY_MOTION_UP)
    {
        return Widget::Event::KEYBOARD_KEY_RELEASE;
    }

    if (motion == KEY_MOTION_TYPED)
    {
        return Widget::Event::KEYBOARD_KEY_TYPED;
    }

    ASSERT_NOT_REACHED();
}

bool acquire_lock()
{
    Stream *socket_stream = stream_open("/Session/compositor.lock", HJ_OPEN_READ);
    if (!handle_has_error(socket_stream))
    {
        stream_close(socket_stream);
        return false;
    }
    else
    {
        Stream *lock_stream = stream_open("/Session/compositor.lock", HJ_OPEN_READ | HJ_OPEN_CREATE);
        stream_close(lock_stream);
        return true;
    }
}

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    if (!acquire_lock())
    {
        IO::errln("The compositor is already running.");
        return PROCESS_FAILURE;
    }

    IO::File keyboard_stream{KEYBOARD_DEVICE_PATH, HJ_OPEN_READ};
    IO::File mouse_stream{MOUSE_DEVICE_PATH, HJ_OPEN_READ};

    IO::Socket socket{"/Session/compositor.ipc", HJ_OPEN_CREATE};

    auto keyboard_notifier = own<Async::Notifier>(keyboard_stream, POLL_READ, [&]() {
        KeyboardPacket packet;
        size_t size = keyboard_stream.read(&packet, sizeof(KeyboardPacket)).unwrap();

        if (size == sizeof(KeyboardPacket))
        {
            Window *window = manager_focus_window();

            if (window)
            {
                Widget::Event event = {
                    .type = key_motion_to_event_type(packet.motion),
                    .accepted = false,
                    .mouse = {},
                    .keyboard = {
                        .key = packet.key,
                        .modifiers = packet.modifiers,
                        .rune = packet.rune,
                    },
                };

                window->send_event(event);
            }
        }
        else
        {
            IO::logln("Invalid keyboard packet with size={}!", size);
        }

        client_destroy_disconnected();
    });

    auto mouse_notifier = own<Async::Notifier>(mouse_stream, POLL_READ, [&]() {
        MousePacket packet;
        size_t size = mouse_stream.read(&packet, sizeof(MousePacket)).unwrap();

        if (size == sizeof(MousePacket))
        {
            cursor_handle_packet(packet);
        }
        else
        {
            IO::logln("Invalid mouse packet with size={}!", size);
        }

        client_destroy_disconnected();
    });

    auto socker_notifier = own<Async::Notifier>(socket, POLL_ACCEPT, [&]() {
        IO::Connection incoming_connection = socket.accept().unwrap();
        Client::connect(incoming_connection);
        client_destroy_disconnected();
    });

    auto repaint_timer = own<Async::Timer>(1000 / 60, []() {
        renderer_repaint_dirty();
        client_destroy_disconnected();
    });

    repaint_timer->start();

    manager_initialize();
    cursor_initialize();
    renderer_initialize();

    return Async::Loop::the()->run();
}

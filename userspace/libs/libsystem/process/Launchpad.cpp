
#include <assert.h>
#include <skift/Environment.h>
#include <string.h>

#include <libio/Path.h>
#include <libsystem/core/Plugs.h>
#include <libsystem/process/Launchpad.h>
#include <libsystem/process/Process.h>

Launchpad *launchpad_create(const char *name, const char *executable)
{
    Launchpad *launchpad = CREATE(Launchpad);

    strcpy(launchpad->name, name);
    strcpy(launchpad->executable, executable);

    for (int &handle : launchpad->handles)
    {
        handle = HANDLE_INVALID_ID;
    }

    launchpad->handles[0] = 0;
    launchpad->handles[1] = 1;
    launchpad->handles[2] = 2;
    launchpad->handles[3] = 3;

    auto executable_path = IO::Path::parse(executable);
    launchpad_argument(launchpad, executable_path.basename().cstring());

#ifndef __KERNEL__
    auto env_copy = environment_copy();
    launchpad_environment(launchpad, strdup(env_copy.cstring()));
#endif

    return launchpad;
}

void launchpad_destroy(Launchpad *launchpad)
{
    for (int i = 0; i < launchpad->argc; i++)
    {
        free(launchpad->argv[i].buffer);
    }

    if (launchpad->env)
    {
        free(launchpad->env);
    }

    free(launchpad);
}

void launchpad_flags(Launchpad *launchpad, TaskFlags flags)
{
    launchpad->flags = flags;
}

void launchpad_argument(Launchpad *launchpad, const char *argument)
{
    assert(launchpad->argc < PROCESS_ARG_COUNT);

    if (argument)
    {
        launchpad->argv[launchpad->argc].buffer = strdup(argument);
        launchpad->argv[launchpad->argc].size = strlen(argument);
    }
    else
    {
        launchpad->argv[launchpad->argc].buffer = strdup("");
        launchpad->argv[launchpad->argc].size = strlen("");
    }

    launchpad->argv[launchpad->argc + 1].buffer = nullptr;

    launchpad->argc++;
}

void launchpad_arguments(Launchpad *launchpad, const Vec<String> &arguments)
{
    for (size_t i = 0; i < arguments.count(); i++)
    {
        launchpad_argument(launchpad, arguments[i].cstring());
    }
}

void launchpad_handle(Launchpad *launchpad, IO::RawHandle &handle_to_pass, int destination)
{
    assert(destination >= 0 && destination < PROCESS_ARG_COUNT);

    launchpad->handles[destination] = handle_to_pass.handle()->id();
}

void launchpad_handle(Launchpad *launchpad, IO::Handle &handle, int destination)
{
    assert(destination >= 0 && destination < PROCESS_ARG_COUNT);

    launchpad->handles[destination] = handle.id();
}

void launchpad_handle(Launchpad *launchpad, Handle *handle_to_pass, int destination)
{
    assert(destination >= 0 && destination < PROCESS_ARG_COUNT);

    launchpad->handles[destination] = handle_to_pass->id;
}

void launchpad_environment(Launchpad *launchpad, const char *buffer)
{
    if (launchpad->env)
    {
        free(launchpad->env);
    }

    launchpad->env = strdup(buffer);
    launchpad->env_size = strlen(buffer);
}

HjResult launchpad_launch(Launchpad *launchpad, int *pid)
{
    int discard;

    if (pid == nullptr)
    {
        pid = &discard;
    }

    strcpy(launchpad->executable, process_resolve(launchpad->executable).cstring());

    HjResult result = __plug_process_launch(launchpad, pid);

    launchpad_destroy(launchpad);

    return result;
}

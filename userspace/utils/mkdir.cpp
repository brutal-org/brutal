#include <libio/Directory.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>

#include <libsystem/io/Filesystem.h>

HjResult mkdir(IO::Path path)
{
    IO::Directory parent{path};

    if (!parent.exist())
    {
        IO::errln("mkdir: created directory {}", path.string());
        return filesystem_mkdir(path.string().cstring());
    }

    return SUCCESS;
}

HjResult mkdir_parent(IO::Path path)
{
    for (size_t i = 0; i < path.length(); i++)
    {
        TRY(mkdir(path.parent(i)));
    }

    return SUCCESS;
}

int main(int argc, const char *argv[])
{
    Shell::ArgParse args;

    args.prologue("Create the DIRECTORY(ies), if they do not already exist.");
    args.usage("[OPTION]... DIRECTORY...");

    bool parent;
    args.option(parent, 'p', "parents", "Make parent directories as needed.");

    bool verbose;
    args.option(verbose, 'v', "verbose", "Print a message for each created directory.");

    args.epiloge("Options can be combined.");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH
                   ? PROCESS_SUCCESS
                   : PROCESS_FAILURE;
    }

    for (auto directory : args.argv())
    {
        if (parent)
        {
            if (mkdir_parent(IO::Path::parse(directory)) != SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
        else
        {
            if (mkdir(IO::Path::parse(directory)) != SUCCESS)
            {
                return PROCESS_FAILURE;
            }
        }
    }

    return PROCESS_SUCCESS;
}

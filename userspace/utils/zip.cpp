
#include <libfile/ZipArchive.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>

int main(int argc, char const *argv[])
{
    Shell::ArgParse args;

    args.show_help_if_no_operand_given();
    args.should_abort_on_failure();

    args.usage("ARCHIVE FILES...");
    args.usage("OPTION... ARCHIVE FILES...");

    auto parse_result = args.eval(argc, argv);
    if (parse_result != Shell::ArgParseResult::SHOULD_CONTINUE)
    {
        return parse_result == Shell::ArgParseResult::SHOULD_FINISH ? PROCESS_SUCCESS : PROCESS_FAILURE;
    }

    if (args.argc() < 2)
    {
        args.help();
        args.fail();
    }

    IO::File file{args.argv()[0]};
    if (file.exist())
    {
        IO::errln("{}: Destination archive already exists '{}'", argv[0], args.argv()[0]);
        return PROCESS_FAILURE;
    }

    auto archive = make<ZipArchive>(IO::Path::parse(args.argv()[0]), false);

    // Pack all files that were passed as arguments
    for (unsigned int i = 1; i < args.argc(); i++)
    {
        IO::outln("{}: Entry: {} is being inserted...", argv[0], args.argv()[i]);

        IO::File src_file(args.argv()[i], HJ_OPEN_READ);
        if (!src_file.exist())
        {
            IO::errln("{}: File '{}' does not exist", argv[0], args.argv()[i]);
            return PROCESS_FAILURE;
        }

        auto result = archive->insert(args.argv()[i].cstring(), src_file);

        if (result != HjResult::SUCCESS)
        {
            IO::errln("{}: Failed to insert entry '{}' with error '{}'", argv[0], args.argv()[i], get_result_description(result));
            return PROCESS_FAILURE;
        }
    }

    return PROCESS_SUCCESS;
}

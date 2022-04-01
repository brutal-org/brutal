#include <abi/Keyboard.h>
#include <abi/Paths.h>

#include <libio/Copy.h>
#include <libio/Directory.h>
#include <libio/File.h>
#include <libio/Streams.h>
#include <libshell/ArgParse.h>

Shell::ArgParseResult loadkey_list_keymap()
{
    IO::Directory keymap_directory{"/Files/Keyboards"};

    if (!keymap_directory.exist())
    {
        IO::errln("keyboardctl: Failed to query keymaps from /Files/Keyboards");
        return Shell::ArgParseResult::FAILURE;
    }

    for (auto entry : keymap_directory.entries())
    {
        IO::Path keymap_path = IO::Path::parse(entry.name);
        IO::outln("- {}", keymap_path.basename_without_extension());
    }

    return Shell::ArgParseResult::SHOULD_FINISH;
}

Shell::ArgParseResult loadkey_set_keymap(Ref<IO::Handle> keyboard_device, String keymap_path)
{
    IO::File file{keymap_path, HJ_OPEN_READ};

    if (!file.exist())
    {
        return Shell::ArgParseResult::FAILURE;
    }

    auto read_all_result = IO::read_all(file);

    if (!read_all_result.success())
    {
        IO::errln("keyboardctl: Failed to open the keymap file: {}", read_all_result.description());

        return Shell::ArgParseResult::FAILURE;
    }

    KeyMap *keymap = (KeyMap *)read_all_result.unwrap().start();
    size_t keymap_size = read_all_result.unwrap().size();

    if (keymap_size < sizeof(KeyMap) ||
        keymap->magic[0] != 'k' ||
        keymap->magic[1] != 'm' ||
        keymap->magic[2] != 'a' ||
        keymap->magic[3] != 'p')
    {
        IO::errln("keyboardctl: Invalid keymap file format!");

        return Shell::ArgParseResult::FAILURE;
    }

    IOCallKeyboardSetKeymapArgs args = {
        .keymap = keymap,
        .size = keymap_size,
    };

    auto call_result = keyboard_device->call(IOCALL_KEYBOARD_SET_KEYMAP, &args);

    if (call_result != SUCCESS)
    {
        IO::errln("keyboardctl: Failed to open the keymap file: {}", get_result_description(call_result));

        return Shell::ArgParseResult::FAILURE;
    }

    IO::outln("Keymap set to {}({})", keymap->language, keymap->region);

    return Shell::ArgParseResult::SHOULD_FINISH;
}

Shell::ArgParseResult loadkey_get_keymap(Ref<IO::Handle> keyboard_device)
{
    KeyMap keymap;

    if (keyboard_device->call(IOCALL_KEYBOARD_GET_KEYMAP, &keymap) != SUCCESS)
    {
        IO::errln("keyboardctl: Failed to retrived the current keymap");
        return Shell::ArgParseResult::FAILURE;
    }

    IO::outln("Current keymap is {}({})", keymap.language, keymap.region);

    return Shell::ArgParseResult::SHOULD_FINISH;
}

int main(int argc, const char *argv[])
{
    Shell::ArgParse args{};
    args.should_abort_on_failure();
    args.show_help_if_no_option_given();

    args.prologue("Get or set the current keyboard keymap");

    args.usage("");
    args.usage("OPTION...");

    args.epiloge("Options can be combined.");

    auto keyboard_handle = make<IO::Handle>(KEYBOARD_DEVICE_PATH, HJ_OPEN_READ);

    if (!keyboard_handle->valid())
    {
        IO::errln("keyboardctl: Failed to open the keyboard device");

        return PROCESS_FAILURE;
    }

    args.option('l', "list", "List all installed keymap on this system.", [&](auto &)
        { return loadkey_list_keymap(); });

    args.option('g', "get", "Get the current keyboard keymap.", [&](auto &)
        { return loadkey_get_keymap(keyboard_handle); });

    args.option_string('s', "set", "Set the current keyboard keymap.", [&](auto &keymap_name)
        {
            auto kaymap_path = IO::format("/Files/Keyboards/{}.kmap", keymap_name);
            return loadkey_set_keymap(keyboard_handle, kaymap_path);
        });

    return args.eval(argc, argv) == Shell::ArgParseResult::FAILURE
               ? PROCESS_FAILURE
               : PROCESS_SUCCESS;
}

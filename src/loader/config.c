#include <json/parser.h>
#include <brutal-alloc>
#include <brutal-debug>
#include "loader/config.h"

LoaderEntry config_entry_parse(Json json)
{
    LoaderEntry entry;

    entry.name = json_get(json, str$("name")).string;
    entry.kernel = json_get(json, str$("kernel")).string;

    Json modules;
    if (json_try_get(json, str$("modules"), &modules))
    {
        vec_init(&entry.modules, alloc_global());

        for (int i = 0; i < json_len(modules); i++)
        {
            Json module = json_at(modules, i);
            LoaderModule current_module = {};

            current_module.name = json_get(module, str$("name")).string;
            current_module.path = json_get(module, str$("path")).string;

            vec_push(&entry.modules, current_module);

            log$("Detected module: {} path: {}", current_module.name, current_module.path);
        }
    }

    Json framebuffer;

    if (json_try_get(json, str$("framebuffer"), &framebuffer))
    {
        entry.framebuffer.width = json_get(framebuffer, str$("width")).number;
        entry.framebuffer.height = json_get(framebuffer, str$("height")).number;
    }

    return entry;
}

LoaderConfig config_load(Str path, Alloc *alloc)
{
    LoaderConfig config = {};
    vec_init(&config.entries, alloc);

    Json config_json = json_parse_file(path, alloc_global());

    Json entries_json;
    if (!json_try_get(config_json, str$("entries"), &entries_json))
    {
        panic$("No entries found!");
    }

    for (int i = 0; i < json_len(entries_json); i++)
    {
        LoaderEntry entry = config_entry_parse(json_at(entries_json, i));

        vec_push(&config.entries, entry);

        log$("Detected entry: {}", entry.name);
    }

    return config;
}

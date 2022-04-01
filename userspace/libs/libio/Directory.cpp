#include <libio/Directory.h>

namespace IO
{

HjResult Directory::read_entries()
{
    HjDirEntry entry;

    auto read = TRY(_handle->read(&entry, sizeof(entry)));

    while (read > 0)
    {
        _entries.push_back({entry.name, entry.stat});
        read = TRY(_handle->read(&entry, sizeof(entry)));
    }

    _entries.sort([](auto &left, auto &right)
        { return strcmp(left.name.cstring(), right.name.cstring()); });

    return SUCCESS;
}

Directory::Directory(const char *path)
    : _handle{make<Handle>(path, HJ_OPEN_READ | HJ_OPEN_DIRECTORY)},
      _path{IO::Path::parse(path)}
{
    read_entries();
}

Directory::Directory(String path)
    : _handle{make<Handle>(path, HJ_OPEN_READ | HJ_OPEN_DIRECTORY)},
      _path{IO::Path::parse(path)}
{
    read_entries();
}

Directory::Directory(const IO::Path &path)
    : _handle{make<Handle>(path.string(), HJ_OPEN_READ | HJ_OPEN_DIRECTORY)},
      _path{path}
{
    read_entries();
}

Directory::Directory(Ref<Handle> handle)
    : _handle{handle}
{
    read_entries();
}

bool Directory::exist()
{
    return _handle->valid();
}

} // namespace IO
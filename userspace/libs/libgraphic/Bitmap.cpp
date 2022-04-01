#include <libgraphic/Bitmap.h>
#include <libgraphic/svg/Svg.h>
#include <libio/Copy.h>
#include <libio/File.h>
#include <libio/MemoryReader.h>
#include <libio/Path.h>
#include <libio/Streams.h>
#include <libpng/Png.h>
#include <libsystem/system/Memory.h>
namespace Graphic
{

static Color _placeholder_buffer[] = {
    Colors::MAGENTA,
    Colors::BLACK,
    Colors::BLACK,
    Colors::MAGENTA,
};

ResultOr<Ref<Bitmap>> Bitmap::create_shared(int width, int height)
{
    Color *pixels = nullptr;
    TRY(memory_alloc(width * height * sizeof(Color), reinterpret_cast<uintptr_t *>(&pixels)));

    int handle = -1;
    memory_get_handle(reinterpret_cast<uintptr_t>(pixels), &handle);

    auto bitmap = make<Bitmap>(handle, BITMAP_SHARED, width, height, pixels);
    bitmap->clear(Colors::BLACK);

    return bitmap;
}

ResultOr<Ref<Bitmap>> Bitmap::create_shared_from_handle(int handle, Math::Vec2i width_and_height)
{
    Color *pixels = nullptr;
    size_t size = 0;

    TRY(memory_include(handle, reinterpret_cast<uintptr_t *>(&pixels), &size));

    if (size < width_and_height.x() * width_and_height.y() * sizeof(Color))
    {
        memory_free(reinterpret_cast<uintptr_t>(pixels));
        return ERR_BAD_IMAGE_FILE_FORMAT;
    }

    memory_get_handle(reinterpret_cast<uintptr_t>(pixels), &handle);

    return make<Bitmap>(handle, BITMAP_SHARED, width_and_height.x(), width_and_height.y(), pixels);
}

Ref<Bitmap> Bitmap::create_static(int width, int height, Color *pixels)
{
    return make<Bitmap>(-1, BITMAP_STATIC, width, height, pixels);
}

Ref<Bitmap> Bitmap::placeholder()
{
    return create_static(2, 2, _placeholder_buffer);
}

ResultOr<Ref<Bitmap>> Bitmap::load_from(String path, int size_hint)
{
    IO::File file{path, HJ_OPEN_READ};

    if (!file.exist())
    {
        return ERR_NO_SUCH_FILE_OR_DIRECTORY;
    }

    IO::Path p = IO::Path::parse(path);
    if (p.extension() == ".png")
    {
        return Png::load(file);
    }
    else if (p.extension() == ".svg")
    {
        return Svg::render(file, size_hint);
    }
    IO::logln("Unknown bitmap extension: {}", p.extension());
    return ERR_NOT_IMPLEMENTED;
}

Ref<Bitmap> Bitmap::load_from_or_placeholder(String path, int size_hint)
{
    auto result = load_from(path, size_hint);

    if (!result.success())
    {
        return placeholder();
    }

    return result.unwrap();
}

HjResult Bitmap::save_to(String path)
{
    UNUSED(path);
    return ERR_NOT_IMPLEMENTED;
}

Bitmap::~Bitmap()
{
    if (_storage == BITMAP_SHARED)
    {
        memory_free(reinterpret_cast<uintptr_t>(_pixels));
    }
}

} // namespace Graphic

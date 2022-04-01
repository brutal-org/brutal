#pragma once

#include <libutils/Assert.h>
#include <libutils/Std.h>
#include <libutils/Traits.h>

namespace Utils
{

template <typename... TStack>
struct Dispatch;

template <>
struct Dispatch<>
{
    static void func(int, auto, auto &)
    {
        Assert::unreachable();
    }
};

template <typename TFirst, typename... TStack>
struct Dispatch<TFirst, TStack...>
{
    static void func(int index, auto storage, auto &visitor)
    {
        if (index == 0)
        {
            using ConstT = CopyConst<decltype(storage), TFirst>;
            visitor(*reinterpret_cast<ConstT *>(storage));
        }
        else
        {
            Dispatch<TStack...>::func(index - 1, storage, visitor);
        }
    }
};

template <typename... Ts>
static void resolve(int index, auto storage, auto visitor)
{
    Dispatch<Ts...>::func(index, storage, visitor);
}

template <typename... Ts>
struct VarOperations
{
    inline static void destroy(int index, void *storage)
    {
        resolve<Ts...>(index, storage, []<typename T>(T &t)
            { t.~T(); });
    }

    inline static void move(int index, void *source, void *destination)
    {
        resolve<Ts...>(index, source, [&]<typename T>(T &t)
            { new (destination) T(::std::move(t)); });
    }

    inline static void copy(int index, const void *source, void *destination)
    {
        resolve<Ts...>(index, source, [&]<typename T>(const T &t)
            { new (destination) T(t); });
    }

    /* --- Visit ------------------------------------------------------------ */

    template <typename TVisitor>
    inline static void visit(int index, void *storage, TVisitor &visitor)
    {
        resolve<Ts...>(index, storage, visitor);
    }
};

template <size_t Len, size_t Align>
struct AlignedStorage
{
    alignas(Align) unsigned char data[Len];
};

template <typename... Ts>
struct Var
{
private:
    static const size_t data_size = Max<sizeof(Ts)...>::value;
    static const size_t data_align = Max<alignof(Ts)...>::value;

    using Operations = VarOperations<Ts...>;

    int _index = -1;
    AlignedStorage<data_size, data_align> _storage;

public:
    Var()
    {
        _index = 0;
        new (&_storage) typename First<Ts...>::Type();
    }

    template <typename T>
    requires In<T, Ts...>
    Var(T value)
    {
        _index = IndexOf<T, Ts...>();
        new (&_storage) T(value);
    }

    Var(const Var<Ts...> &other)
    {
        _index = other._index;
        Operations::copy(other._index, &other._storage, &_storage);
    }

    Var(Var<Ts...> &&other)
    {
        _index = other._index;
        Operations::move(other._index, &other._storage, &_storage);
    }

    ~Var()
    {
        Operations::destroy(_index, &_storage);
    }

    Var<Ts...> &operator=(const Var<Ts...> &other)
    {
        Operations::destroy(_index, &_storage);
        _index = other._index;
        Operations::copy(_index, &_storage, &other._storage);

        return *this;
    }

    Var<Ts...> &operator=(Var<Ts...> &&other)
    {
        if (this != &other)
        {
            std::swap(_index, other._index);
            std::swap(_storage, other._storage);
        }

        return *this;
    }

    template <typename T>
    bool is()
    {
        return (_index == IndexOf<T, Ts...>());
    }

    template <typename T, typename... Args>
    void set(Args &&...args)
    {
        Operations::destroy(_index, &_storage);
        new (&_storage) T(std::forward<Args>(args)...);
        _index = IndexOf<T, Ts...>();
    }

    template <typename T>
    T &get()
    {
        Assert::equal(_index, IndexOf<T, Ts...>());
        return *reinterpret_cast<T *>(&_storage);
    }

    template <typename T, typename Callback>
    void with(Callback callback)
    {
        if (is<T>())
        {
            callback(get<T>());
        }
    }

    template <typename TVisitor>
    void visit(TVisitor &&visitor)
    {
        Operations::visit(_index, &_storage, visitor);
    }
};

template <typename... Ts>
struct Visitor : Ts...
{
    using Ts::operator()...;
};

template <typename... Ts>
Visitor(Ts...) -> Visitor<Ts...>;

} // namespace Utils

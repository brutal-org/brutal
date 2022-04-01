#pragma once

#include <string.h>

#include <libmath/MinMax.h>
#include <libutils/Iter.h>
#include <libutils/Ref.h>
#include <libutils/Std.h>

namespace Utils
{

template <typename T>
void typed_copy(T *destination, T *source, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        new (&destination[i]) T(source[i]);
    }
}

template <typename T>
void typed_move(T *destination, T *source, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        if (destination <= source)
        {
            new (&destination[i]) T(std::move(source[i]));
        }
        else
        {
            new (&destination[count - i - 1]) T(std::move(source[count - i - 1]));
        }
    }
}

template <typename T>
struct Vec
{
private:
    T *_storage = nullptr;
    size_t _count = 0;
    size_t _capacity = 0;

public:
    size_t count() const { return _count; }

    bool empty() const { return _count == 0; }

    bool any() const { return !empty(); }

    constexpr T *raw_storage() { return _storage; }
    constexpr const T *raw_storage() const { return _storage; }

    T &at(size_t index)
    {
        assert(index < _count);
        return _storage[index];
    }

    const T &at(size_t index) const
    {
        assert(index < _count);
        return _storage[index];
    }

    Vec() : Vec(16) {}

    Vec(size_t capacity)
    {
        ensure_capacity(capacity);
    }

    Vec(std::initializer_list<T> data)
    {
        ensure_capacity(data.size());

        for (const auto &el : data)
        {
            push_back(el);
        }
    }

    Vec(AdoptTag, T *storage, size_t size)
        : _storage(storage),
          _count(size),
          _capacity(size)
    {
    }

    Vec(const Vec &other)
    {
        ensure_capacity(other.count());

        _count = other.count();
        typed_copy(_storage, other._storage, _count);
    }

    Vec(Vec &&other)
    {
        std::swap(_storage, other._storage);
        std::swap(_count, other._count);
        std::swap(_capacity, other._capacity);
    }

    ~Vec()
    {
        clear();

        if (_storage)
            free(_storage);
    }

    Vec &operator=(const Vec &other)
    {
        if (this != &other)
        {
            clear();

            ensure_capacity(other.count());
            _count = other.count();
            typed_copy(_storage, other._storage, _count);
        }

        return *this;
    }

    Vec &operator=(Vec &&other)
    {
        if (this != &other)
        {
            std::swap(_storage, other._storage);
            std::swap(_count, other._count);
            std::swap(_capacity, other._capacity);
        }

        return *this;
    }

    T &operator[](size_t index)
    {
        assert(index < _count);

        return _storage[index];
    }

    const T &operator[](size_t index) const
    {
        assert(index < _count);

        return _storage[index];
    }

    bool operator!=(const Vec &other) const
    {
        return !(*this == other);
    }

    bool operator==(const Vec &other) const
    {
        if (this == &other)
        {
            return true;
        }

        if (_count != other._count)
        {
            return false;
        }

        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] != other._storage[i])
            {
                return false;
            }
        }

        return true;
    }

    void clear()
    {
        if (!_storage)
        {
            return;
        }

        for (size_t i = 0; i < _count; i++)
        {
            _storage[i].~T();
        }

        _count = 0;
    }

    template <typename Callback>
    Iter foreach(Callback callback) const
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (callback(_storage[i]) == Iter::STOP)
            {
                return Iter::STOP;
            }
        }

        return Iter::CONTINUE;
    }

    template <typename Callback>
    Iter foreach_reversed(Callback callback) const
    {
        for (size_t i = _count; i > 0; i--)
        {
            if (callback(_storage[i - 1]) == Iter::STOP)
            {
                return Iter::STOP;
            }
        }

        return Iter::CONTINUE;
    }

    template <typename Comparator>
    void sort(Comparator comparator)
    {
        for (size_t i = 0; i + 1 < _count; i++)
        {
            for (size_t j = i + 1; j < _count; j++)
            {
                if (comparator(_storage[i], _storage[j]) > 0)
                {
                    std::swap(_storage[i], _storage[j]);
                }
            }
        }
    }

    void resize(size_t new_count)
    {
        ensure_capacity(new_count);

        if (_count < new_count)
        {
            for (size_t i = _count; i < new_count; i++)
            {
                new (&_storage[i]) T();
            }
        }
        else if (_count > new_count)
        {
            for (size_t i = new_count; i < _count; i++)
            {
                _storage[i].~T();
            }
        }

        _count = new_count;
    }

    void ensure_capacity(size_t capacity)
    {
        capacity = MAX(capacity, 16);

        if (!_storage)
        {
            _storage = reinterpret_cast<T *>(calloc(capacity, sizeof(T)));
            _capacity = capacity;
            _count = 0;

            return;
        }

        capacity = MAX(_count, capacity);

        if (capacity <= _capacity)
        {
            return;
        }

        size_t new_capacity = _capacity;

        while (new_capacity <= capacity)
        {
            new_capacity += new_capacity / 4;
        }

        T *new_storage = reinterpret_cast<T *>(calloc(new_capacity, sizeof(T)));

        for (size_t i = 0; i < _count; i++)
        {
            new (&new_storage[i]) T(std::move(_storage[i]));
        }

        free(_storage);
        _storage = new_storage;
        _capacity = new_capacity;
    }

    void grow()
    {
        if (_count + 1 >= _capacity)
        {
            size_t new_capacity = _capacity + _capacity / 4;
            T *new_storage = reinterpret_cast<T *>(calloc(new_capacity, sizeof(T)));

            for (size_t i = 0; i < _count; i++)
            {
                new (&new_storage[i]) T(std::move(_storage[i]));
            }

            free(_storage);
            _storage = new_storage;
            _capacity = new_capacity;
        }

        _count++;
    }

    void shrink()
    {
        _count--;

        size_t new_capacity = _capacity - _capacity / 4;

        if (MAX(_count, 16) < new_capacity)
        {
            T *new_storage = reinterpret_cast<T *>(calloc(new_capacity, sizeof(T)));

            for (size_t i = 0; i < _count; i++)
            {
                new (&new_storage[i]) T(std::move(_storage[i]));
                _storage[i].~T();
            }

            free(_storage);
            _storage = new_storage;
            _capacity = new_capacity;
        }
    }

    T &insert(size_t index, const T &value)
    {
        return insert(index, T(value));
    }

    T &insert(size_t index, T &&value)
    {
        assert(index <= _count);

        grow();

        for (size_t j = _count - 1; j > index; j--)
        {
            new (&_storage[j]) T(std::move(_storage[j - 1]));
            at(j - 1).~T();
        }

        new (&_storage[index]) T(std::move(value));

        return _storage[index];
    }

    void insert_sorted(T value)
    {
        size_t insert_index = 0;

        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] < value)
            {
                insert_index = i;
            }
            else
            {
                break;
            }
        }

        insert(insert_index, value);
    }

    template <typename Comparator>
    void insert_sorted(T value, Comparator comparator)
    {
        size_t insert_index = 0;

        for (size_t i = 0; i < _count; i++)
        {
            if (comparator(_storage[i], value))
            {
                insert_index = i;
            }
            else
            {
                break;
            }
        }

        insert(insert_index, value);
    }

    void remove_index(size_t index)
    {
        assert(index < _count);

        _storage[index].~T();

        for (size_t i = index; i < _count - 1; ++i)
        {
            new (&_storage[i]) T(std::move(_storage[i + 1]));
            _storage[i + 1].~T();
        }

        shrink();
    }

    void remove_value(const T &value)
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] == value)
            {
                remove_index(i);
                return;
            }
        }
    }

    void remove_all_value(const T &value)
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] == value)
            {
                remove_index(i);
            }
        }
    }

    template <typename MatchCallback>
    bool remove_all_match(MatchCallback match)
    {
        bool has_removed_something = false;

        for (size_t i = 0; i < _count; i++)
        {
            while (i < _count && match(_storage[i]))
            {
                remove_index(i);
                has_removed_something = true;
            }
        }

        return has_removed_something;
    }

    T take_at(size_t index)
    {
        assert(index < _count);

        T copy = std::move(_storage[index]);
        remove_index(index);
        return copy;
    }

    T &push(const T &value)
    {
        return push(T(value));
    }

    T &push(T &&value)
    {
        return insert(0, std::move(value));
    }

    template <typename... Args>
    T &emplace(Args &&...args)
    {
        grow();

        for (size_t j = _count - 1; j > 0; j--)
        {
            new (&_storage[j]) T(std::move(_storage[j - 1]));
            at(j - 1).~T();
        }

        new (&_storage[0]) T(std::forward<Args>(args)...);
        return _storage[0];
    }

    T &push_back(const T &value)
    {
        return push_back(T(value));
    }

    T &push_back(T &&value)
    {
        return insert(_count, std::move(value));
    }

    template <typename... Args>
    T &emplace_back(Args &&...args)
    {
        grow();

        new (&_storage[_count - 1]) T(std::forward<Args>(args)...);
        return _storage[_count - 1];
    }

    void push_back_many(const Vec<T> &values)
    {
        for (size_t i = 0; i < values.count(); i++)
        {
            push_back(values[i]);
        }
    }

    void push_back_many(const T *data, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            push_back(data[i]);
        }
    }

    T pop()
    {
        assert(_count > 0);

        T value = std::move(_storage[0]);
        remove_index(0);

        return value;
    }

    T pop_back()
    {
        assert(_count > 0);

        T value = std::move(_storage[_count - 1]);
        remove_index(_count - 1);

        return value;
    }

    T &peek() const
    {
        assert(_count > 0);
        return _storage[0];
    }

    T &peek_back() const
    {
        assert(_count > 0);
        return _storage[_count - 1];
    }

    bool contains(const T &value) const
    {
        for (size_t i = 0; i < _count; i++)
        {
            if (_storage[i] == value)
            {
                return true;
            }
        }

        return false;
    }

    ContiguousIterator<T> begin() const { return _storage; }
    ContiguousIterator<T> end() const { return _storage + _count; }
};

template <typename T>
struct IsVec : public FalseType
{
};

template <typename T>
struct IsVec<Vec<T>> : public TrueType
{
};

template <typename T>
struct TrimVec;

template <typename T>
struct TrimVec<Vec<T>>
{
    typedef T type;
};

} // namespace Utils

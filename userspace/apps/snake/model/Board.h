#pragma once

#include <libmath/Random.h>
#include <libutils/Vec.h>

#include "snake/model/Fruit.h"
#include "snake/model/Snake.h"

namespace Snake
{

struct Board :
    public Ref<Board>,
    public Shared<Board>
{
private:
    int _width = 16;
    int _height = 16;

    Snake _snake;
    Vec<Fruit> _fruits;
    Math::Random _random;

public:
    int width() { return _width; }
    int height() { return _height; }

    Snake &snake() { return _snake; }
    Vec<Fruit> &fruits() { return _fruits; }

    Board(int width, int height)
        : _width{width},
          _height{height}
    {
        reset();
    }

    void reset()
    {
        _snake = {Math::Vec2i{_width, _height} / 2};
        _fruits.clear();
        spawn();
    }

    void spawn()
    {
        for (size_t i = _fruits.count(); i < 5; i++)
        {
            Math::Vec2i position{_random.next_u8() % _width, _random.next_u8() % _height};

            bool obstructed = false;

            if (_snake.colide_with(position))
            {
                obstructed = true;
            }

            for (auto &fruit : _fruits)
            {
                if (fruit.position == position)
                {
                    obstructed = true;
                }
            }

            if (!obstructed)
            {
                _fruits.push_back(Fruit{.position = position});
            }
        }
    }

    void update()
    {
        _snake.move();

        if (_snake.colide_with_body(_snake.head()))
        {
            reset();
        }

        if (!Math::Recti{_width, _height}.contains(_snake.head()))
        {
            reset();
        }

        int old_fruit_count = _fruits.count();

        _fruits.remove_all_match([&](auto &fruit)
            { return _snake.colide_with(fruit.position); });

        int new_fruit_count = _fruits.count();

        if (old_fruit_count != new_fruit_count)
        {
            _snake.eat();
        }

        spawn();
    }
};

} // namespace Snake

#pragma once

#include <libasync/Observable.h>
#include <libgraphic/Font.h>
#include <libtext/Rune.h>
#include <libutils/Assert.h>
#include <libutils/Box.h>
#include <libutils/Shared.h>
#include <libutils/Vec.h>
#include <libwidget/Theme.h>

namespace Widget
{

struct TextCursor;

struct TextModelLine
{
private:
    Vec<Text::Rune> _runes{};

public:
    Text::Rune operator[](size_t index)
    {
        Assert::lower_than(index, length());

        return _runes[index];
    }

    size_t length()
    {
        return _runes.count();
    }

    void append(Text::Rune rune)
    {
        _runes.push_back(rune);
    }

    void append(const TextModelLine &line)
    {
        _runes.push_back_many(line._runes);
    }

    void append_at(size_t index, Text::Rune rune)
    {
        _runes.insert(index, rune);
    }

    void backspace_at(size_t index)
    {
        _runes.remove_index(index - 1);
    }

    void delete_at(size_t index)
    {
        _runes.remove_index(index);
    }

    Math::Recti bound(const Graphic::Font &font)
    {
        int width = 0;

        for (size_t i = 0; i < _runes.count(); i++)
        {
            width += font.mesure(_runes[i]).width();
        }

        return {width, font.metrics().fulllineheight()};
    }
};

struct TextModelSpan
{
private:
    size_t _line;
    size_t _start;
    size_t _end;

    ThemeColorRole _foreground;
    ThemeColorRole _background;

public:
    size_t line() { return _line; }

    size_t start() { return _start; }

    size_t end() { return _end; }

    ThemeColorRole foreground() { return _foreground; }

    ThemeColorRole background() { return _background; }

    TextModelSpan(size_t line, size_t start, size_t end) : TextModelSpan(line, start, end, THEME_FOREGROUND, THEME_BACKGROUND)
    {
    }

    TextModelSpan(size_t line, size_t start, size_t end, ThemeColorRole foreground, ThemeColorRole background)
        : _line(line),
          _start(start),
          _end(end),
          _foreground(foreground),
          _background(background)
    {
    }

    ~TextModelSpan()
    {
    }
};

struct TextModel :
    public Shared<TextModel>,
    public Async::Observable<TextModel>
{
private:
    Vec<Box<TextModelLine>> _lines{1024};
    Vec<TextModelSpan> _spans{1024};

public:
    static Ref<TextModel> empty();

    static Ref<TextModel> open(String path);

    static Ref<TextModel> create(String text);

    Math::Recti bound(const Graphic::Font &font)
    {
        int width = 0;
        int height = 0;

        for (size_t i = 0; i < _lines.count(); i++)
        {
            auto line_bound = _lines[i]->bound(font);
            width = MAX(width, line_bound.width());
            height += line_bound.height();
        }

        return {width, height};
    }

    String string();

    ResultOr<size_t> save(String path);

    void clear()
    {
        _lines.clear();
        _spans.clear();

        append_line(own<TextModelLine>());

        did_update();
    }

    /* --- Editing ---------------------------------------------------------- */

    TextModelLine &line(int index) { return *_lines[index]; }

    size_t line_count() const { return _lines.count(); }

    void append_line(Box<TextModelLine> line) { _lines.push_back(line); }

    void append_at(TextCursor &cursor, Text::Rune rune);

    void backspace_at(TextCursor &cursor);

    void delete_at(TextCursor &cursor);

    void newline_at(TextCursor &cursor);

    void move_line_up_at(TextCursor &cursor);

    void move_line_down_at(TextCursor &cursor);

    /* --- Coloration ------------------------------------------------------- */

    void span_add(TextModelSpan span)
    {
        _spans.insert_sorted(span, [](auto &left, auto &right)
            { return left.line() < right.line() &&
                     left.start() < right.start(); });
    }

    TextModelSpan span_at(size_t line, size_t column)
    {
        for (size_t i = 0; i < _spans.count(); i++)
        {
            auto &span = _spans[i];

            if (span.line() == line && column >= span.start() && column < span.end())
            {
                return span;
            }
        }

        return TextModelSpan(line, column, column + 1);
    }

    void span_clear()
    {
        _spans.clear();
    }
};

struct TextCursor
{
private:
    size_t _line = 0;
    size_t _column = 0;
    size_t _prefered_column = 0;

public:
    size_t line() { return _line; }

    size_t column() { return _column; }

    void move_to_within(TextModel &model, size_t line)
    {
        if (line < model.line_count())
        {
            _line = line;
            _column = clamp(_prefered_column, 0, model.line(_line).length());
        }
    }

    void move_to_within(TextModelLine &line, size_t column)
    {
        _column = clamp(column, 0, line.length());
        _prefered_column = _column;
    }

    void move_to_beginning_of_the_line()
    {
        _column = 0;
        _prefered_column = 0;
    }

    void move_up_within(TextModel &model)
    {
        UNUSED(model);

        if (_line > 0)
        {
            _line--;
            _column = clamp(_prefered_column, 0, model.line(_line).length());
        }
    }

    void move_up_within(TextModel &model, size_t lines)
    {
        UNUSED(model);

        for (size_t i = 0; i < lines; i++)
        {
            move_up_within(model);
        }
    }

    void move_down_within(TextModel &model)
    {
        if (_line < model.line_count() - 1)
        {
            _line++;
            _column = clamp(_prefered_column, 0, model.line(_line).length());
        }
    }

    void move_down_within(TextModel &model, size_t lines)
    {
        for (size_t i = 0; i < lines; i++)
        {
            move_down_within(model);
        }
    }

    void move_left_within(TextModel &model)
    {
        UNUSED(model);

        if (_column > 0)
        {
            _column--;
        }
        else if (_column == 0 && _line > 0)
        {
            move_up_within(model);
            _column = model.line(_line).length();
        }

        _prefered_column = _column;
    }

    void move_right_within(TextModel &model)
    {
        if (_column < model.line(_line).length())
        {
            _column++;
        }
        else if (_column == model.line(_line).length() &&
                 _line < model.line_count() - 1)
        {
            move_down_within(model);
            _column = 0;
        }

        _prefered_column = _column;
    }

    void move_home_within(TextModelLine &line)
    {
        UNUSED(line);

        _column = 0;
        _prefered_column = _column;
    }

    void move_end_within(TextModelLine &line)
    {
        _column = line.length();
        _prefered_column = _column;
    }

    void move_home_within(TextModel &model)
    {
        UNUSED(model);

        _line = 0;
        _column = 0;
        _prefered_column = _column;
    }

    void move_end_within(TextModel &model)
    {
        _line = model.line_count() - 1;
        _column = model.line(_line).length();

        _prefered_column = _column;
    }

    void clamp_within(TextModel &model)
    {
        _line = clamp(_line, 0, model.line_count() - 1);
        _column = clamp(_column, 0, model.line(_line).length());
    }
};

} // namespace Widget

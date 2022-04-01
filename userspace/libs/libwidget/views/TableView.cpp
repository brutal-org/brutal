#include <libgraphic/Painter.h>
#include <libwidget/Theme.h>
#include <libwidget/Window.h>
#include <libwidget/views/TableView.h>

namespace Widget
{

Math::Recti Table::header_bound() const
{
    return bound().take_top(TABLE_ROW_HEIGHT);
}

Math::Recti Table::list_bound() const
{
    return bound().shrinked(Insetsi(TABLE_ROW_HEIGHT, 0, 0));
}

Math::Recti Table::scrollbar_bound() const
{
    return list_bound().take_right(16);
}

Math::Recti Table::row_bound(int row) const
{
    return {
        list_bound().x(),
        list_bound().y() + row * TABLE_ROW_HEIGHT - _scroll_offset,
        list_bound().width(),
        TABLE_ROW_HEIGHT,
    };
}

Math::Recti Table::column_bound(int column) const
{
    int column_count = _model->columns();
    int column_width = list_bound().width() / column_count;

    return {
        list_bound().x() + column_width * column,
        list_bound().y(),
        column_width,
        list_bound().height(),
    };
}

Math::Recti Table::cell_bound(int row, int column) const
{
    return {
        row_bound(row).x() + column * column_bound(column).width(),
        row_bound(row).y(),
        column_bound(column).width(),
        row_bound(row).height(),
    };
}

int Table::row_at(Math::Vec2i position) const
{
    if (!list_bound().contains(position))
    {
        return -1;
    }

    position = position - list_bound().position();

    int row = (position.y() + _scroll_offset) / TABLE_ROW_HEIGHT;

    if (row < 0 || row >= _model->rows())
    {
        row = -1;
    }

    return row;
}

void Table::paint_cell(Graphic::Painter &painter, int row, int column)
{
    Math::Recti bound = cell_bound(row, column);
    Var data = _model->data(row, column);

    painter.push();
    painter.clip(bound);

    if (data.has_icon())
    {
        painter.blit(
            *data.icon(),
            Graphic::ICON_18PX,
            Math::Recti(bound.x() + 7, bound.y() + 7, 18, 18),
            color(THEME_FOREGROUND));

        painter.draw_string(
            *font(),
            data.as_string(),
            Math::Vec2i(bound.x() + 7 + 18 + 7, bound.y() + 20),
            color(THEME_FOREGROUND));
    }
    else
    {
        painter.draw_string(
            *font(),
            data.as_string(),
            Math::Vec2i(bound.x() + 7, bound.y() + 20),
            color(THEME_FOREGROUND));
    }

    painter.pop();
}

Table::Table()

{
    _scrollbar = add<ScrollBarElement>();

    _scrollbar->on(Event::VALUE_CHANGE, [this](auto)
        {
            _scroll_offset = _scrollbar->value();
            should_repaint();
        });
}

Table::Table(Ref<TableModel> model)
    : Table()
{
    this->model(model);
}

void Table::paint(Graphic::Painter &painter, const Math::Recti &)
{
    if (!_model)
    {
        return;
    }

    int column_count = _model->columns();
    int column_width = bound().width() / column_count;

    if (_model->rows() == 0)
    {
        painter.draw_string_within(
            *font(),
            _empty_message.cstring(),
            list_bound().take_top(TABLE_ROW_HEIGHT),
            Math::Anchor::CENTER,
            color(THEME_FOREGROUND));
    }
    else
    {
        for (int row = MAX(0, _scroll_offset / TABLE_ROW_HEIGHT - 1);
             row < MIN(_model->rows(), ((_scroll_offset + list_bound().height()) / TABLE_ROW_HEIGHT) + 1);
             row++)
        {

            if (_selected == row)
            {
                painter.fill_rectangle_rounded(row_bound(row).shrinked(2), 4, color(THEME_ACCENT));
            }

            for (int column = 0; column < column_count; column++)
            {
                paint_cell(painter, row, column);
            }
        }
    }

    painter.acrylic(header_bound());
    painter.fill_rectangle(header_bound(), color(THEME_BACKGROUND).with_alpha(0.5));

    for (int column = 0; column < column_count; column++)
    {
        Math::Recti header_bound_cell{
            header_bound().x() + column * column_width,
            header_bound().y(),
            column_width,
            TABLE_ROW_HEIGHT,
        };

        if (column < column_count - 1)
        {
            painter.fill_rectangle(header_bound_cell.take_right(1), color(THEME_BORDER));
        }

        painter.draw_string(*font(), _model->header(column).cstring(), Math::Vec2i(header_bound_cell.x() + 7, header_bound_cell.y() + 20), color(THEME_FOREGROUND));
        painter.draw_string(*font(), _model->header(column).cstring(), Math::Vec2i(header_bound_cell.x() + 7 + 1, header_bound_cell.y() + 20), color(THEME_FOREGROUND));
    }
}

void Table::event(Event *event)
{
    if (!_model)
    {
        return;
    }

    if (event->type == Event::MOUSE_BUTTON_PRESS)
    {
        select(row_at(event->mouse.position));
    }
    else if (event->type == Event::MOUSE_DOUBLE_CLICK)
    {
        Event action_event = {};
        action_event.type = Event::ACTION;
        dispatch_event(&action_event);
    }
    else if (event->type == Event::MOUSE_SCROLL)
    {
        event->accepted = true;
        _scrollbar->dispatch_event(event);
    }
    else if (event->type == Event::KEYBOARD_KEY_TYPED)
    {
        if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            select(_selected - 1);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN)
        {
            select(_selected + 1);
        }
        else if (event->keyboard.key == KEYBOARD_KEY_ENTER)
        {
            Event action_event = {};
            action_event.type = Event::ACTION;
            dispatch_event(&action_event);
        }
    }
}

void Table::layout()
{
    if (!_model)
    {
        return;
    }

    _scrollbar->container(scrollbar_bound());
    _scrollbar->update(TABLE_ROW_HEIGHT * _model->rows(), list_bound().height(), _scroll_offset);
}

} // namespace Widget

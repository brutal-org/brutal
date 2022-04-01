#pragma once

#include <libutils/String.h>

#include <libwidget/Elements.h>
#include <libwidget/model/TableModel.h>

namespace Widget
{

struct Table : public Element
{
private:
    static constexpr int TABLE_ROW_HEIGHT = 32;

    Ref<TableModel> _model = nullptr;
    Box<Async::Observer<TableModel>> _model_observer;

    int _selected = -1;
    int _scroll_offset = 0;
    Ref<ScrollBarElement> _scrollbar;

    String _empty_message{"No data to display"};

    Math::Recti scrollbar_bound() const;
    Math::Recti header_bound() const;
    Math::Recti list_bound() const;
    Math::Recti row_bound(int row) const;
    Math::Recti column_bound(int column) const;
    Math::Recti cell_bound(int row, int column) const;
    int row_at(Math::Vec2i position) const;
    void paint_cell(Graphic::Painter &painter, int row, int column);

public:
    void model(Ref<TableModel> model)
    {
        _model = model;
        _model_observer = model->observe([this](auto &)
            {
                should_repaint();
                should_relayout();
            });
    }

    void empty_message(String message)
    {
        _empty_message = message;
    }

    int selected() { return _selected; }

    void select(int index)
    {
        if (index == _selected)
        {
            return;
        }

        if (index < -1 || index >= _model->rows())
        {
            return;
        }

        _selected = index;
        should_repaint();
    }

    void scroll_to_top()
    {
        _scroll_offset = 0;

        should_repaint();
        should_relayout();
    }

    Table();

    Table(Ref<TableModel> model);

    void paint(Graphic::Painter &painter, const Math::Recti &dirty) override;

    void event(Event *event) override;

    void layout() override;
};

WIDGET_BUILDER(Table, table);

} // namespace Widget

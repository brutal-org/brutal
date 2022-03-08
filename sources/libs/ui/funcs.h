#pragma once

#include <ui/traits.h>

static inline bool ui_begin(Ui *ui)
{
    return ui->begin(ui->context);
}

static inline void ui_end(Ui *ui)
{
    ui->end(ui->context);
}

static inline void *ui_use(Ui *ui, int what)
{
    return ui->use(ui->context, what);
}

static inline void ui_update(Ui *ui, int what)
{
    ui->update(ui->context, what);
}

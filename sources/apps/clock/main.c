#include <brutal/time.h>
#include <ui/ui.h>

typedef struct
{
    enum
    {
        P_TIME,
        P_TIMER,
        P_CHRONO
    } page;

    int timer;
    bool timer_running;

    int chrono;
    bool chrono_running;

    Vec(TimeStamp) laps;
} Clock;

void clock_deinit(void *p)
{
    Clock *c = p;
    vec_deinit(&c->laps);
}

Clock *clock_state(Ui *ui)
{
    Clock *state = ui_use_state$(ui, (Clock){});

    if (ui_use_effect(ui, clock_deinit))
    {
        vec_init(&state->laps, ui_use_alloc(ui));
    }

    if (ui_use_timer(ui, state->timer_running, 1000))
    {
        state->timer--;
        state->timer_running = state->timer > 0;
        ui_update(ui, UI_STATE);
    }

    if (ui_use_timer(ui, state->chrono_running, 100))
    {
        state->chrono++;
        ui_update(ui, UI_STATE);
    }

    return state;
}

void clock_render(Ui *ui)
{
    Clock *state = clock_state(ui);

    switch (state->page)
    {

    default:
    case P_TIME:
    {
        ui_text(ui, "Time");
        Time dt = time_now();
        ui_text_fmt$(ui, "%02d:%02d:%02d", dt.hour, dt.minute, dt.second);
        break;
    }

    case P_TIMER:
        ui_text(ui, "Timer");
        ui_text_fmt$(ui, "%02d:%02d:%02d", state->timer / 3600, (state->timer / 60) % 60, state->timer % 60);

        if (ui_button(ui, "Start/Pause"))
        {
            state->timer_running = !state->timer_running;
            ui_update(ui, UI_STATE);
        }

        if (ui_button(ui, "Reset"))
        {
            state->timer = 60 * 60 * 5;
            state->timer_running = false;
            ui_update(ui, UI_STATE);
        }
        break;

    case P_CHRONO:
        ui_text(ui, "Chrono");
        ui_text_fmt$(ui, "%02d:%02d:%02d", state->chrono / 3600, (state->chrono / 60) % 60, state->chrono % 60);

        vec_foreach_v(lap, &state->laps)
        {
            ui_text_fmt$(ui, "%02d:%02d:%02d", lap / 3600, (lap / 60) % 60, lap % 60);
        }

        if (ui_button(ui, "Start/Pause"))
        {
            state->chrono_running = !state->chrono_running;
            ui_update(ui, UI_STATE);
        }

        if (ui_button(ui, "Reset"))
        {
            state->chrono = 0;
            vec_clear(&state->laps);
            ui_update(ui, UI_STATE);
        }

        if (ui_button(ui, "Lap"))
        {
            vec_push(&state->laps, state->chrono);
            ui_update(ui, UI_STATE);
        }
        break;
    }

    ui_scope$(ui, ui_panel)
    {
        if (ui_button(ui, "Time"))
        {
            state->page = P_TIME;
            ui_update(ui, UI_STATE);
        }

        if (ui_button(ui, "Timer"))
        {
            state->page = P_TIMER;
            ui_update(ui, UI_STATE);
        }

        if (ui_button(ui, "Chrono"))
        {
            state->page = P_CHRONO;
            ui_update(ui, UI_STATE);
        }
    }
}

ui_main$(clock_render)

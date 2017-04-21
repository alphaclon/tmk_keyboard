
#include "animation.h"
#include "animation_utils.h"
#include "timer.h"
#include "../../utils.h"
#include "../eeconfig_backlight.h"
#include "breathing.h"
#include "sweep.h"
#include "type_o_circles.h"
#include "type_o_matic.h"
#include "type_o_raindrops.h"
#include "raindrops.h"
#include "jellybean_raindrops.h"
#include "color_cycle_all.h"
#include "color_cycle_left_right.h"
#include "color_cycle_up_down.h"

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

static animation_names current_animation = animation_sweep;

void initialize_animation(void)
{
    memset(&animation, 0, sizeof(struct _animation_interface));

#ifdef BACKLIGHT_ENABLE
    current_animation = eeconfig_read_animation_current();
    set_animation(current_animation);

    eeconfig_read_animation_hsv_values(0, &animation.hsv.h, &animation.hsv.s, &animation.hsv.v);
    eeconfig_read_animation_hsv_values(1, &animation.hsv2.h, &animation.hsv2.s, &animation.hsv2.v);

    animation.rgb = hsv_to_rgb(animation.hsv);
#endif
}

void animation_save_state(void)
{
#ifdef BACKLIGHT_ENABLE
    eeconfig_write_animation_current(current_animation);
    eeconfig_write_animation_hsv_values(0, animation.hsv.h, animation.hsv.s, animation.hsv.v);
    eeconfig_write_animation_hsv_values(1, animation.hsv2.h, animation.hsv2.s, animation.hsv2.v);
#endif
}

animation_names animation_current(void)
{
	return current_animation;
}

void set_animation(animation_names animation_by_name)
{
    switch (animation_by_name)
    {
    case animation_sweep:
        set_animation_sweep();
        break;
    case animation_breathing:
        set_animation_breathing();
        break;
    case animation_type_o_matic:
        set_animation_type_o_matic();
        break;
    case animation_type_o_raindrops:
        set_animation_type_o_raindrops();
        break;
    case animation_type_o_circles:
        set_animation_type_o_circles();
        break;
    case animation_raindrops:
        set_animation_raindrops();
        break;
    case animation_jellybean_raindrops:
        set_animation_jellybean_raindrops();
        break;
    case animation_cycle_all:
        set_animation_color_cycle_all();
        break;
    case animation_cycle_up_down:
        set_animation_color_cycle_up_down();
        break;
    case animation_cycle_left_right:
        set_animation_color_cycle_left_right();
        break;
    }
}

void animation_next()
{
    current_animation = increment(current_animation, 1, 0, animation_LAST);
    dprintf("animation_next: %u\n", current_animation);

    if (!animation_is_running())
        return;

    stop_animation();

    set_animation(current_animation);
    start_animation();
}

void animation_previous()
{
    current_animation = decrement(current_animation, 1, 0, animation_LAST);
    dprintf("animation_previous: %u\n", current_animation);

    if (!animation_is_running())
        return;

    stop_animation();

    set_animation(current_animation);
    start_animation();
}

void animation_set_speed(uint16_t delay_in_ms)
{
	if (delay_in_ms < 10)
		animation.delay_in_ms = 10;
	animation.delay_in_ms = delay_in_ms;
}

void animation_increase_speed(void)
{
    animation.delay_in_ms = decrement16(animation.delay_in_ms, 25, 10, 1000);
}

void animation_decrease_speed(void)
{
    animation.delay_in_ms = increment16(animation.delay_in_ms, 25, 10, 1000);
}

void toggle_animation(void)
{
    dprintf("toggle_animation\n");

    if (animation_is_running())
    {
        stop_animation();
        return;
    }

    set_animation(current_animation);
    start_animation();
}

bool animation_is_running()
{
    return (animation.is_running);
}

void start_animation()
{
    dprintf("start_animation\n");

    if (animation.animationStart)
        animation.animationStart();

    animation.is_running = true;
    animation.loop_timer = timer_read();
    animation.duration_timer = timer_read32();
}

void stop_animation()
{
    dprintf("stop_animation\n");

    if (animation.animationStop)
        animation.animationStop();

    animation.is_running = false;
    animation.animationStart = 0;
    animation.animationStop = 0;
    animation.animationLoop = 0;
    animation.animation_typematrix_row = 0;
}

void animation_decrease_hsv_color(animation_hsv_names hsv_name, HSVColorName color_name)
{
    switch (hsv_name)
    {
    case animation_hsv_1:
        animation.hsv.hsv[color_name] = decrement(animation.hsv.hsv[color_name], 8, 0, 255);
        animation.rgb = hsv_to_rgb(animation.hsv);
        break;
    case animation_hsv_2:
        animation.hsv2.hsv[color_name] = decrement(animation.hsv2.hsv[color_name], 8, 0, 255);
        break;
    }
}

void animation_increase_hsv_color(animation_hsv_names hsv_name, HSVColorName color_name)
{
    switch (hsv_name)
    {
    case animation_hsv_1:
        animation.hsv.hsv[color_name] = increment(animation.hsv.hsv[color_name], 8, 0, 255);
        animation.rgb = hsv_to_rgb(animation.hsv);
        break;
    case animation_hsv_2:
        animation.hsv2.hsv[color_name] = increment(animation.hsv2.hsv[color_name], 8, 0, 255);
        break;
    }
}

void animate()
{
    if (animation.animationLoop == 0 || !animation.is_running)
        return;

    if (timer_elapsed(animation.loop_timer) < animation.delay_in_ms)
        return;

    if (animation.duration_in_ms > 0 && timer_elapsed32(animation.duration_timer) > animation.duration_in_ms)
    {
        stop_animation();
        return;
    }

    animation.loop_timer = timer_read();
    animation.animationLoop();
}

void animation_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    if (animation.animation_typematrix_row)
    {
        animation.loop_timer = timer_read();
        animation.animation_typematrix_row(row_number, row);
    }
}

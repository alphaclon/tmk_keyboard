
#include "animation.h"
#include "animation_utils.h"
#include "breathing.h"
#include "sweep.h"
#include "timer.h"
#include "type_o_circles.h"
#include "type_o_matic.h"

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

uint8_t current_annimation = ANIMATION_SWEEP;

void set_animation_sweep()
{
	dprintf("sweep\r\n");

    animation.brightness = 0;
    animation.delay_in_ms = 50;
    animation.duration_in_ms = 0;

    animation.animationStart = &sweep_animation_start;
    animation.animationStop = &sweep_animation_stop;
    animation.animationLoop = &sweep_animation_loop;
    animation.animation_typematrix_row = 0;
}

void set_animation_type_o_matic()
{
	dprintf("type_o_matic\r\n");

    animation.brightness = 255;
    animation.delay_in_ms = 250;
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_matic_animation_start;
    animation.animationStop = &type_o_matic_animation_stop;
    animation.animationLoop = &type_o_matic_animation_loop;
    animation.animation_typematrix_row = &type_o_matic_typematrix_row;
}

void set_animation_type_o_circles()
{
	dprintf("type_o_circles\r\n");

    animation.brightness = 255;
    animation.delay_in_ms = 250;
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_circles_animation_start;
    animation.animationStop = &type_o_circles_animation_stop;
    animation.animationLoop = &type_o_circles_animation_loop;
    animation.animation_typematrix_row = &type_o_circles_typematrix_row;
}

void set_animation_breathing()
{
	dprintf("breathing\r\n");

    animation.brightness = 255;
    animation.delay_in_ms = 0;
    animation.duration_in_ms = 0;

    animation.animationStart = &breathing_animation_start;
    animation.animationStop = &breathing_animation_stop;
    animation.animationLoop = 0;
    animation.animation_typematrix_row = 0;
}

void set_animation(uint8_t animation_number)
{
    switch (animation_number)
    {
    case ANIMATION_SWEEP:
        set_animation_sweep();
        break;
    case ANIMATION_BREATHING:
        set_animation_breathing();
        break;
    case ANIMATION_TYPE_O_MATIC:
        set_animation_type_o_matic();
        break;
    case ANIMATION_TYPE_O_CIRCLES:
        set_animation_type_o_circles();
        break;
    }
}

void animation_next()
{
    current_annimation++;
    if (current_annimation >= ANIMATIONS_COUNT)
        current_annimation = 0;

    dprintf("animation_next: %u\r\n", current_annimation);

    if (!animation_is_running())
    	return;

    stop_animation();

    set_animation(current_annimation);
    start_animation();
}

void animation_previous()
{
    if (current_annimation == 0)
        current_annimation = ANIMATIONS_COUNT;
    current_annimation--;

	dprintf("animation_previous: %u\r\n", current_annimation);

    if (!animation_is_running())
    	return;

    stop_animation();

    set_animation(current_annimation);
    start_animation();
}

void animation_increase_speed(void)
{
    if (animation.delay_in_ms > 10)
        animation.delay_in_ms -= 50;
    else
    	animation.delay_in_ms = 10;
}

void animation_decrease_speed(void)
{
    animation.delay_in_ms += 50;
}

void animation_test()
{
    if (animation_is_running())
    {
        stop_animation();
        return;
    }

    set_animation_type_o_matic();
    start_animation();
}

void animation_toggle(void)
{
	dprintf("animation_toggle\r\n");

    if (animation_is_running())
    {
        stop_animation();
        return;
    }

    set_animation(current_annimation);
    start_animation();
}

bool animation_is_running()
{
    return (animation.animationStart && animation.animationStop);
}

void start_animation()
{
	dprintf("start_animation\r\n");

    if (animation.animationStart)
        animation.animationStart();

    animation.loop_timer = timer_read();
    animation.duration_timer = timer_read32();
}

void stop_animation()
{
	dprintf("stop_animation\r\n");

    if (animation.animationStop)
        animation.animationStop();

    animation.animationStart = 0;
    animation.animationStop = 0;
    animation.animationLoop = 0;
}

void animate()
{
    if (animation.animationLoop == 0)
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

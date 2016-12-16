
#include "animation.h"
#include "animation_utils.h"
#include "breathing.h"
#include "sweep.h"
#include "timer.h"
#include "type_o_circles.h"
#include "type_o_matic.h"

uint8_t current_annimation = ANIMATION_TYPE_O_MATIC;

void set_animation_sweep()
{
    animation.brightness = 0;
    animation.delay_in_ms = 500;
    animation.duration_in_ms = 0;

    animation.animationStart = &sweep_animation_start;
    animation.animationStop = &sweep_animation_stop;
    animation.animationLoop = &sweep_animation_loop;
}

void set_animation_type_o_matic()
{
    animation.brightness = 130;
    animation.delay_in_ms = 500;
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_matic_animation_start;
    animation.animationStop = &type_o_matic_animation_stop;
    animation.animationLoop = &type_o_matic_animation_loop;
}

void set_animation_type_o_circles()
{
    animation.brightness = 128;
    animation.delay_in_ms = 500;
    animation.duration_in_ms = 0;

    animation.animationStart = &type_o_circles_animation_start;
    animation.animationStop = &type_o_circles_animation_stop;
    animation.animationLoop = &type_o_circles_animation_loop;
}

void set_animation_breathing()
{
    animation.brightness = 0;
    animation.delay_in_ms = 1000;
    animation.duration_in_ms = 0;

    animation.animationStart = &breathing_animation_start;
    animation.animationStop = &breathing_animation_stop;
    animation.animationLoop = 0;
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

void start_next_animation()
{
    stop_animation();

    current_annimation++;
    if (current_annimation >= ANIMATIONS_COUNT)
        current_annimation = 0;

    set_animation(current_annimation);
    start_animation();
}

void start_previous_animation()
{
    stop_animation();

    if (current_annimation == 0)
        current_annimation = ANIMATIONS_COUNT;
    current_annimation--;

    set_animation(current_annimation);
    start_animation();
}

void increase_animation_speed(void)
{
    if (animation.delay_in_ms > 100)
        animation.delay_in_ms -= 100;
}

void decrease_animation_speed(void)
{
    animation.delay_in_ms += 100;
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
    if (animation.animationStart)
        animation.animationStart();

    animation.loop_timer = timer_read();
    animation.duration_timer = timer_read32();
}

void stop_animation()
{
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

    if (animation.duration_timer > 0 && timer_elapsed32(animation.duration_timer) > animation.duration_timer)
        return;

    if (timer_elapsed(animation.loop_timer) < animation.delay_in_ms)
        return;

    animation.loop_timer = timer_read();
    animation.animationLoop();
}

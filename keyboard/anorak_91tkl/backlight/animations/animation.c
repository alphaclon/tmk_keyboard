
#include "animation.h"
#include "animation_utils.h"
#include "timer.h"
#include "../../utils.h"
#include "../eeconfig_backlight.h"
#include "../key_led_map.h"
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
#include "color_wave.h"
#include "flying_ball.h"
#include "gradient_up_down.h"
#include "gradient_left_right.h"
#include "gradient_full_flicker.h"
#include "conway.h"
#include "floating_plasma.h"
#include "particle_sys_flame.h"
#include <avr/pgmspace.h>
#include <string.h>
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define HSV_COLOR_STEP 8
#define MINIMAL_DELAY_TIME_MS 10
#define ANIMATION_SUSPEND_TIMEOUT (10L * 60L * 1000L)

static animation_names current_animation = animation_type_o_matic;
static uint32_t last_key_pressed_timestamp = 0;
static bool suspend_animation_on_idle = true;

#ifdef DEBUG_ANIMATION
//#define DEBUG_ANIMATION_SPEED
#endif

#ifdef DEBUG_ANIMATION_SPEED
static uint32_t duration_ms = 0;
static uint32_t elapsed_ms = 0;
static uint8_t loop_count = 10;
#endif

const char animation_name_string_cycle_all[] PROGMEM = "cycle all";
const char animation_name_string_cycle_up_down[] PROGMEM = "cycle up down";
const char animation_name_string_cycle_left_right[] PROGMEM = "cycle left right";
const char animation_name_string_gradient_up_down[] PROGMEM = "gradient up down";
const char animation_name_string_gradient_left_right[] PROGMEM = "gradient left right";
const char animation_name_string_gradient_full_flicker[] PROGMEM = "gradient full flicker";
const char animation_name_string_raindrops[] PROGMEM = "raindrops";
const char animation_name_string_jellybean_raindrops[] PROGMEM = "jellybean raindrops";
const char animation_name_string_flying_ball[] PROGMEM = "flying ball";
const char animation_name_string_type_o_matic[] PROGMEM = "type-o-matic";
const char animation_name_string_type_o_raindrops[] PROGMEM = "type-o-raindrops";
const char animation_name_string_type_o_circles[] PROGMEM = "type-o-circles";
const char animation_name_string_sweep[] PROGMEM = "sweep";
const char animation_name_string_wave[] PROGMEM = "wave";
const char animation_name_string_conway[] PROGMEM = "conway";
const char animation_name_string_floating_plasma[] PROGMEM = "floating plasma";
const char animation_name_string_particle_sys_flame[] PROGMEM = "flame";
const char animation_name_string_breathing[] PROGMEM = "breate";

PGM_P animation_name_strings[animation_LAST] = {animation_name_string_cycle_all,
                                                animation_name_string_cycle_up_down,
                                                animation_name_string_cycle_left_right,
                                                animation_name_string_gradient_up_down,
                                                animation_name_string_gradient_left_right,
                                                animation_name_string_gradient_full_flicker,
                                                animation_name_string_raindrops,
                                                animation_name_string_jellybean_raindrops,
                                                animation_name_string_flying_ball,
                                                animation_name_string_type_o_matic,
                                                animation_name_string_type_o_raindrops,
                                                animation_name_string_type_o_circles,
                                                animation_name_string_sweep,
                                                animation_name_string_wave,
                                                animation_name_string_conway,
                                                animation_name_string_floating_plasma,
                                                animation_name_string_particle_sys_flame,
                                                animation_name_string_breathing};

void initialize_animation(void)
{
    initLedPosByMatrix();

    memset(&animation, 0, sizeof(struct _animation_interface));

#ifdef BACKLIGHT_ENABLE
    current_animation = eeconfig_read_animation_current();

    if (current_animation >= animation_LAST)
    {
        current_animation = (animation_names)0;
        eeconfig_write_animation_current(current_animation);
    }

    eeconfig_read_animation_hsv_values(0, &animation.hsv.h, &animation.hsv.s, &animation.hsv.v);
    eeconfig_read_animation_hsv_values(1, &animation.hsv2.h, &animation.hsv2.s, &animation.hsv2.v);

    animation.rgb = hsv_to_rgb(animation.hsv);
#endif
}

void animation_save_state(void)
{
#ifdef BACKLIGHT_ENABLE
	dprintf("animation save\n");
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
    current_animation = animation_by_name;

    if (current_animation >= animation_LAST)
    {
        current_animation = (animation_names)0;
    }

    switch (current_animation)
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
    case animation_flying_ball:
        set_animation_flying_ball();
        break;
    case animation_wave:
        set_animation_color_wave();
        break;
    case animation_gradient_up_down:
        set_animation_gradient_up_down();
        break;
    case animation_gradient_left_right:
        set_animation_gradient_left_right();
        break;
    case animation_gradient_full_flicker:
        set_animation_gradient_full_flicker();
        break;
    case animation_conway:
        set_animation_conway();
        break;
    case animation_floating_plasma:
        set_animation_floating_plasma();
        break;
    case animation_particle_sys_flame:
    	set_animation_particle_sys_flame();
        break;

    case animation_LAST:
        break;
    }
}

void animation_next()
{
    stop_animation();

    current_animation = increment(current_animation, 1, 0, animation_LAST);
    if (current_animation == animation_LAST)
        current_animation = 0;
    dprintf("animation_next: %u\n", current_animation);

    set_animation(current_animation);
    start_animation();
}

void animation_previous()
{
    stop_animation();
    if (current_animation == 0)
        current_animation = animation_LAST;
    current_animation = decrement(current_animation, 1, 0, animation_LAST);
    dprintf("animation_previous: %u\n", current_animation);

    set_animation(current_animation);
    start_animation();
}

void animation_set_speed(uint16_t delay_in_ms)
{
    if (delay_in_ms < MINIMAL_DELAY_TIME_MS)
        animation.delay_in_ms = MINIMAL_DELAY_TIME_MS;
    animation.delay_in_ms = delay_in_ms;
}

void animation_increase_speed(void)
{
    animation.delay_in_ms = decrement16(animation.delay_in_ms, 25, MINIMAL_DELAY_TIME_MS, 1000);
}

void animation_decrease_speed(void)
{
    animation.delay_in_ms = increment16(animation.delay_in_ms, 25, MINIMAL_DELAY_TIME_MS, 1000);
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

    if (animation.is_running || animation.is_suspended)
        return;

    if (animation.animationStart)
        animation.animationStart();

    animation.is_running = true;
    animation.is_suspended = false;
    animation.loop_timer = timer_read();
    animation.duration_timer = timer_read32();
    last_key_pressed_timestamp = timer_read32();
}

void set_and_start_animation(animation_names animation_by_name)
{
    stop_animation();
    set_animation(animation_by_name);
    start_animation();
}

void stop_animation()
{
    dprintf("stop_animation\n");

    if (!animation.is_running && !animation.is_suspended)
        return;

    if (animation.animationStop)
        animation.animationStop();

    animation.is_running = false;
    animation.is_suspended = false;
    animation.animationStart = 0;
    animation.animationStop = 0;
    animation.animationLoop = 0;
    animation.animation_typematrix_row = 0;
}

void suspend_animation()
{
    // dprintf("suspend_animation\n");

    if (!animation.is_running)
        return;

    animation.is_running = false;
    animation.is_suspended = true;
}

void resume_animation()
{
    // dprintf("resume_animation\n");

    if (!animation.is_suspended)
        return;

    animation.is_running = true;
    animation.is_suspended = false;

    last_key_pressed_timestamp = timer_read32();
}

void resume_animation_in_idle_state()
{
    // dprintf("resume_animation\n");

    if (!animation.is_suspended)
        return;

    animation.is_running = true;
    animation.is_suspended = false;

    last_key_pressed_timestamp -= ANIMATION_SUSPEND_TIMEOUT;
}

void animation_decrease_hsv_color(animation_hsv_names hsv_name, HSVColorName color_name)
{
    switch (hsv_name)
    {
    case animation_hsv_1:
        animation.hsv.hsv[color_name] = decrement(animation.hsv.hsv[color_name], HSV_COLOR_STEP, 0, 255);
        animation.rgb = hsv_to_rgb(animation.hsv);
        break;
    case animation_hsv_2:
        animation.hsv2.hsv[color_name] = decrement(animation.hsv2.hsv[color_name], HSV_COLOR_STEP, 0, 255);
        break;
    }
}

void animation_increase_hsv_color(animation_hsv_names hsv_name, HSVColorName color_name)
{
    switch (hsv_name)
    {
    case animation_hsv_1:
        animation.hsv.hsv[color_name] = increment(animation.hsv.hsv[color_name], HSV_COLOR_STEP, 0, 255);
        animation.rgb = hsv_to_rgb(animation.hsv);
        break;
    case animation_hsv_2:
        animation.hsv2.hsv[color_name] = increment(animation.hsv2.hsv[color_name], HSV_COLOR_STEP, 0, 255);
        break;
    }
}

void animate()
{
    if (!animation.is_running || animation.animationLoop == 0)
        return;

    if (timer_elapsed(animation.loop_timer) < animation.delay_in_ms)
        return;

    if (suspend_animation_on_idle && timer_elapsed32(last_key_pressed_timestamp) > ANIMATION_SUSPEND_TIMEOUT)
        return;

/*
if (animation.duration_in_ms > 0 && timer_elapsed32(animation.duration_timer) > animation.duration_in_ms)
{
    stop_animation();
    return;
}
*/

#ifdef DEBUG_ANIMATION_SPEED
    if (loop_count)
    {
        loop_count--;
        elapsed_ms = timer_read32();
    }
    else
    {
        loop_count = 10;
        duration_ms /= 10;
        dprintf("avg: %lu\n", duration_ms);
        duration_ms = 0;
        elapsed_ms = timer_read32();
    }
#endif

    animation.loop_timer = timer_read();
    animation.animationLoop();

#ifdef DEBUG_ANIMATION_SPEED
    duration_ms += timer_elapsed32(elapsed_ms);
// dprintf("el: %u\n", duration_ms);
#endif
}

void animation_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    last_key_pressed_timestamp = timer_read32();

    if (!animation.is_running)
        return;

    animation_default_typematrix_row(row_number, row);

    if (animation.animation_typematrix_row)
    {
        // animation.loop_timer = timer_read();
        animation.animation_typematrix_row(row_number, row);
    }
}

char *animation_name(animation_names animation_by_name)
{
	char *buf = (char*)malloc((strlen_P(animation_name_strings[animation_by_name])+1) * sizeof(char));
	strcpy_P(buf, animation_name_strings[animation_by_name]);
	return buf;
}

uint16_t get_options_for_animation(animation_names animation_by_name)
{
    uint16_t options = 0;
    dprintf("get_options_for_animation: %d\n", animation_by_name);

    switch (animation_by_name)
    {
    case animation_sweep:
        break;
    case animation_type_o_matic:
        break;
    case animation_type_o_raindrops:
        break;
    case animation_type_o_circles:
        break;
    case animation_raindrops:
        break;
    case animation_jellybean_raindrops:
        break;
    case animation_cycle_all:
        break;
    case animation_cycle_up_down:
        break;
    case animation_cycle_left_right:
        break;
    case animation_flying_ball:
        break;
    case animation_wave:
        break;
    case animation_gradient_up_down:
        break;
    case animation_gradient_left_right:
        break;
    case animation_gradient_full_flicker:
        break;
    case animation_conway:
        break;
    case animation_particle_sys_flame:
        break;
    case animation_floating_plasma:
    	options = get_animation_options_floating_plasma();
        break;
    case animation_breathing:
        break;
    case animation_LAST:
        break;
    }

    return options;
}

void set_animation_option(animation_names animation_by_name, animation_options option)
{
    switch (animation_by_name)
    {
    case animation_sweep:
        break;
    case animation_type_o_matic:
        break;
    case animation_type_o_raindrops:
        break;
    case animation_type_o_circles:
        break;
    case animation_raindrops:
        break;
    case animation_jellybean_raindrops:
        break;
    case animation_cycle_all:
        break;
    case animation_cycle_up_down:
        break;
    case animation_cycle_left_right:
        break;
    case animation_flying_ball:
        break;
    case animation_wave:
        break;
    case animation_gradient_up_down:
        break;
    case animation_gradient_left_right:
        break;
    case animation_gradient_full_flicker:
        break;
    case animation_conway:
        break;
    case animation_particle_sys_flame:
        break;
    case animation_floating_plasma:
    	set_animation_option_floating_plasma(option);
        break;
    case animation_breathing:
        break;
    case animation_LAST:
        break;
    }
}

animation_options get_animation_option(animation_names animation_by_name)
{
    animation_options option = animation_option_none;
    dprintf("get_options_for_animation: %d\n", animation_by_name);

    switch (animation_by_name)
    {
    case animation_sweep:
        break;
    case animation_type_o_matic:
        break;
    case animation_type_o_raindrops:
        break;
    case animation_type_o_circles:
        break;
    case animation_raindrops:
        break;
    case animation_jellybean_raindrops:
        break;
    case animation_cycle_all:
        break;
    case animation_cycle_up_down:
        break;
    case animation_cycle_left_right:
        break;
    case animation_flying_ball:
        break;
    case animation_wave:
        break;
    case animation_gradient_up_down:
        break;
    case animation_gradient_left_right:
        break;
    case animation_gradient_full_flicker:
        break;
    case animation_conway:
        break;
    case animation_particle_sys_flame:
        break;
    case animation_floating_plasma:
    	option = get_animation_option_floating_plasma();
        break;
    case animation_breathing:
        break;
    case animation_LAST:
        break;
    }

    return option;
}

void set_current_animation_option(animation_options option)
{
    set_animation_option(current_animation, option);
}

animation_options get_current_animation_option()
{
    return get_animation_option(current_animation);
}

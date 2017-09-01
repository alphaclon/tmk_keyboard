
#include "../control.h"
#include "animation_utils.h"
#include "config.h"
#include "matrix.h"

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

uint8_t animation_frame;

void breathing_animation_start(void)
{
    animation_frame = 1;

    animation_prepare(animation_frame);

    issi.setAutoFramePlayConfig(0, 1, 40);
    issi.setAutoFramePlayMode(0);
    issi.setBreathConfig(7, 7, 2);
    issi.setBreathMode(true);

    /*
    issi.enableLed(0, 1, animation_frame);
    issi.setLedBrightness(14, 255, animation_frame);
    issi.setAutoFramePlayConfig(0,1,40);
    issi.setAutoFramePlayMode(animation_frame);
    issi.setBreathConfig(7, 7, 2);
    issi.setBreathMode(1);
    */
}

void breathing_animation_stop(void)
{
    issi.setBreathMode(false);
    issi.setPictureMode();

    animation_postpare(animation_frame);
}

void set_animation_breathing()
{
    dprintf("breathing\r\n");

    animation.brightness = 255;
    animation.delay_in_ms = 1000;
    animation.duration_in_ms = 0;

    animation.animationStart = &breathing_animation_start;
    animation.animationStop = &breathing_animation_stop;
    animation.animationLoop = 0;
    animation.animation_typematrix_row = 0;
}

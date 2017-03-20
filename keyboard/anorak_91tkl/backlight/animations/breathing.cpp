
#include "../control.h"
#include "animation_utils.h"
#include "config.h"
#include "matrix.h"

uint8_t animation_frame;

void breathing_animation_start()
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

void breathing_animation_stop()
{
    issi.setBreathMode(false);
    issi.setPictureMode();

    animation_postpare(animation_frame);
}

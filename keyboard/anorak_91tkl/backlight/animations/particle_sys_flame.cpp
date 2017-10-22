
#include "../key_led_map.h"
#include "animation_utils.h"
#include "config.h"
#include "arduino-particle-sys/ParticleSys.h"
#include "arduino-particle-sys/Particle_Std.h"
#include "arduino-particle-sys/Particle_Fixed.h"
#include "arduino-particle-sys/Emitter_Fountain.h"
#include "arduino-particle-sys/PartMatrix.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

class particle_sys_flame
{
public:
    particle_sys_flame()
    {
        emitter = new Emitter_Fountain(0, 0, 5, &source);
        pSys = new ParticleSys(numParticles, particles, emitter);
    }

    ~particle_sys_flame()
    {
        delete emitter;
        delete pSys;
    }

    void start()
    {
        // randomSeed(analogRead(0));

        // source.vx = 3;
        // source.vy = 1;
        source.x = 112;
        source.y = 1;
        Emitter_Fountain::minLife = 20;
        Emitter_Fountain::maxLife = 80;
        Particle_Std::ay = 1;
        // PartMatrix::isOverflow = false;

        // init all pixels to zero
        pMatrix.reset();
    }

    void update()
    {
        pSys->update();
        drawMatrix();
        is31fl3733_91tkl_update_led_pwm(&issi);
    }

private:
    /**
     * Render the particles into a low-resolution matrix
     */
    void drawMatrix()
    {
        pMatrix.reset();
        pMatrix.render(particles, numParticles);
        // update the actual LED matrix
        for (uint8_t y = 0; y < PS_PIXELS_Y; y++)
        {
            for (uint8_t x = 0; x < PS_PIXELS_X; x++)
            {
                // RGB color_rgb = {.r = pMatrix.matrix[x][y].r, .g = pMatrix.matrix[x][y].g, .b =
                // pMatrix.matrix[x][y].b};

                RGB color_rgb;
                color_rgb.r = pMatrix.matrix[x][y].r;
                color_rgb.g = pMatrix.matrix[x][y].g;
                color_rgb.b = pMatrix.matrix[x][y].b;

                draw_keymatrix_rgb_pixel(&issi, PS_PIXELS_Y - y - 1, x, color_rgb);
            }
        }
    }

    static const byte numParticles = 50;

    Particle_Std particles[numParticles];
    Particle_Fixed source;
    Emitter_Fountain *emitter;
    ParticleSys *pSys;
    PartMatrix pMatrix;
};

particle_sys_flame *flame;

void particle_sys_flame_animation_start(void)
{
    animation_prepare(true);

    flame = new particle_sys_flame;
    flame->start();
}

void particle_sys_flame_animation_stop(void)
{
    animation_postpare();
    delete flame;
}

void particle_sys_flame_animation_loop(void)
{
    flame->update();
}

#ifdef __cplusplus
extern "C" {
#endif

void set_animation_particle_sys_flame()
{
    dprintf("particle_sys_flame\n");

    animation.delay_in_ms = FPS_TO_DELAY(10);
    animation.duration_in_ms = 0;

    animation.animationStart = &particle_sys_flame_animation_start;
    animation.animationStop = &particle_sys_flame_animation_stop;
    animation.animationLoop = &particle_sys_flame_animation_loop;
    animation.animation_typematrix_row = 0;
}

#ifdef __cplusplus
}
#endif

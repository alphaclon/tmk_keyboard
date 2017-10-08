
#include "conway.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "utils.h"
#include "config.h"
#include <stdlib.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define MCPOS(row, col) (row * MATRIX_COLS + col)

static uint8_t *cells = 0;

void conway_init_cells(void)
{
    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            bool draw = rand() % 7 ? false : true;

            cells[MCPOS(key_row, key_col)] = draw;
            // cells[key_row][key_col] = draw;

            if (draw)
            {
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, animation.hsv);
            }
        }
    }
}

void conway_animation_start(void)
{
    animation_prepare(true);
    cells = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));

    conway_init_cells();
}

void conway_animation_stop(void)
{
    animation_postpare();
    free(cells);
}

void conway_animation_loop(void)
{
    HSV hsv_black = {.h = 0, .s = 0, .v = 0};
    int rm, rp, cm, cp, nsum;
    bool is_empty = true;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            rm = (0 == key_row) ? (MATRIX_ROWS - 1) : (key_row - 1);
            rp = ((MATRIX_ROWS - 1) == key_row) ? 0 : (key_row + 1);
            cm = (0 == key_col) ? (MATRIX_COLS - 1) : (key_col - 1);
            cp = ((MATRIX_COLS - 1) == key_col) ? 0 : (key_col + 1);

            nsum = cells[MCPOS(rm, cm)] + cells[MCPOS(rm, key_col)] + cells[MCPOS(rm, cp)] + cells[MCPOS(key_row, cm)] +
                   cells[MCPOS(key_row, cp)] + cells[MCPOS(rp, cm)] + cells[MCPOS(rp, key_col)] + cells[MCPOS(rp, cp)];

            switch (nsum)
            {

            case 3:
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, animation.hsv);
                cells[MCPOS(key_row, key_col)] = true;
                is_empty = false;
                break;

            case 2:
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col,
                                         (cells[MCPOS(key_row, key_col)]) ? hsv_black : animation.hsv);

                // cells[key_row][key_col] = cells[key_row][key_col];
                break;

            default:
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv_black);
                cells[MCPOS(key_row, key_col)] = false;
            }

            if (cells[MCPOS(key_row, key_col)])
                is_empty = false;
        }
    }

    if (is_empty)
    {
        conway_init_cells();
    }

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void set_animation_conway()
{
    dprintf("conway\n");

    animation.delay_in_ms = FPS_TO_DELAY(1);
    animation.duration_in_ms = 0;

    animation.animationStart = &conway_animation_start;
    animation.animationStop = &conway_animation_stop;
    animation.animationLoop = &conway_animation_loop;
    animation.animation_typematrix_row = 0;
}

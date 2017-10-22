
#include "conway.h"
#include "../key_led_map.h"
#include "animation_utils.h"
#include "utils.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_ANIMATION
#include "debug.h"
#else
#include "nodebug.h"
#endif

#define MCPOS(row, col) (row * MATRIX_COLS + col)

static uint8_t *cells[2] = {0, 0};
static uint8_t *cell_colors = 0;
static uint8_t cycle = 0;
static uint8_t cells_changed = 0;

void conway_init_cells(void)
{
    dprintf("cycle %u", cycle);

    HSV hsv;
    hsv.h = animation.hsv.h + cycle;
    hsv.s = animation.hsv.s;
    hsv.v = animation.hsv.v;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            bool draw = rand() % 7 ? false : true;
            cells[0][MCPOS(key_row, key_col)] = draw;
            cells[1][MCPOS(key_row, key_col)] = 0;
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, draw ? hsv : hsv_black);
        }
    }

    /*
    cells[0][MCPOS(3, 5)] = 1;
    cells[0][MCPOS(3, 6)] = 1;
    cells[0][MCPOS(3, 7)] = 1;

    draw_keymatrix_hsv_pixel(&issi, 3, 5, hsv);
    draw_keymatrix_hsv_pixel(&issi, 3, 6, hsv);
    draw_keymatrix_hsv_pixel(&issi, 3, 7, hsv);
    */

    cycle += 4;
}

void conway_animation_loop(void)
{
    int rm, rp, cm, cp, nsum;
    bool unchanged = true;

    HSV hsv;
    hsv.h = animation.hsv.h + cycle;
    hsv.s = animation.hsv.s;
    hsv.v = animation.hsv.v;

    cells_changed = 0;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            rm = (0 == key_row) ? (MATRIX_ROWS - 1) : (key_row - 1);
            rp = ((MATRIX_ROWS - 1) == key_row) ? 0 : (key_row + 1);
            cm = (0 == key_col) ? (MATRIX_COLS - 1) : (key_col - 1);
            cp = ((MATRIX_COLS - 1) == key_col) ? 0 : (key_col + 1);

            nsum = cells[0][MCPOS(rm, cm)] + cells[0][MCPOS(rm, key_col)] + cells[0][MCPOS(rm, cp)] +
                   cells[0][MCPOS(key_row, cm)] + cells[0][MCPOS(key_row, cp)] + cells[0][MCPOS(rp, cm)] +
                   cells[0][MCPOS(rp, key_col)] + cells[0][MCPOS(rp, cp)];

            /*
            dprintf("row %d", key_row);
            dprintf("col %d", key_col);
            dprintf("rm %d", rm);
            dprintf("rp %d", rp);
            dprintf("cm %d", cm);
            dprintf("cp %d", cp);

            dprintf("rm cm %d", cells[0][MCPOS(rm, cm)]);
            dprintf("rm c  %d", cells[0][MCPOS(rm, key_col)]);
            dprintf("rm cp %d", cells[0][MCPOS(rm, cp)]);
            dprintf("r  cm %d", cells[0][MCPOS(key_row, cm)]);
            dprintf("r  cp %d", cells[0][MCPOS(key_row, cp)]);
            dprintf("rp cm %d", cells[0][MCPOS(rp, cm)]);
            dprintf("rp c  %d", cells[0][MCPOS(rp, key_col)]);
            dprintf("rp cp %d", cells[0][MCPOS(rp, cp)]);
            dprintf("nsum %d", nsum);
            */

            switch (nsum)
            {

            case 3:
                cells[1][MCPOS(key_row, key_col)] = 1;
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
                break;

            case 2:
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col,
                                         (cells[0][MCPOS(key_row, key_col)]) ? hsv : hsv_black);

                cells[1][MCPOS(key_row, key_col)] = cells[0][MCPOS(key_row, key_col)];
                break;

            default:
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv_black);
                cells[1][MCPOS(key_row, key_col)] = 0;
            }

            cells_changed += abs(cells[1][MCPOS(key_row, key_col)] - cells[0][MCPOS(key_row, key_col)]);

            if (cells[1][MCPOS(key_row, key_col)] != cells[0][MCPOS(key_row, key_col)])
                unchanged = false;
        }
    }

    // dprintf("cells changed %u", cells_changed);

    memcpy(cells[0], cells[1], MATRIX_ROWS * MATRIX_COLS * sizeof(uint8_t));

    if (unchanged)
        conway_init_cells();

    is31fl3733_91tkl_update_led_pwm(&issi);
}

void conway_rgb_init_cells(void)
{
    dprintf("cycle %u", cycle);

    cycle = animation.hsv.h;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            bool draw = rand() % 7 ? false : true;
            cells[0][MCPOS(key_row, key_col)] = draw;
            cells[1][MCPOS(key_row, key_col)] = 0;
            cell_colors[MCPOS(key_row, key_col)] = animation.hsv.h;
            draw_keymatrix_hsv_pixel(&issi, key_row, key_col, draw ? animation.hsv : hsv_black);
        }
    }
}

void conway_rgb_animation_loop(void)
{
    HSV hsv;
    int rm, rp, cm, cp, nsum;
    bool unchanged = true;

    cells_changed = 0;

    for (uint8_t key_row = 0; key_row < MATRIX_ROWS; ++key_row)
    {
        for (uint8_t key_col = 0; key_col < MATRIX_COLS; ++key_col)
        {
            rm = (0 == key_row) ? (MATRIX_ROWS - 1) : (key_row - 1);
            rp = ((MATRIX_ROWS - 1) == key_row) ? 0 : (key_row + 1);
            cm = (0 == key_col) ? (MATRIX_COLS - 1) : (key_col - 1);
            cp = ((MATRIX_COLS - 1) == key_col) ? 0 : (key_col + 1);

            nsum = cells[0][MCPOS(rm, cm)] + cells[0][MCPOS(rm, key_col)] + cells[0][MCPOS(rm, cp)] +
                   cells[0][MCPOS(key_row, cm)] + cells[0][MCPOS(key_row, cp)] + cells[0][MCPOS(rp, cm)] +
                   cells[0][MCPOS(rp, key_col)] + cells[0][MCPOS(rp, cp)];

            switch (nsum)
            {

            case 3:
                if (cells[0][MCPOS(key_row, key_col)] == 0)
                {
                    cell_colors[MCPOS(key_row, key_col)] = animation.hsv.h + cycle;
                }

                cells[1][MCPOS(key_row, key_col)] = 1;

                hsv.h = cell_colors[MCPOS(key_row, key_col)];
                hsv.s = animation.hsv.s;
                hsv.v = animation.hsv.v;

                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
                break;

            case 2:
                if (cells[0][MCPOS(key_row, key_col)])
                {
                    hsv.h = cell_colors[MCPOS(key_row, key_col)];
                    hsv.s = animation.hsv.s;
                    hsv.v = animation.hsv.v;

                    draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv);
                }
                else
                {
                    draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv_black);
                }

                cells[1][MCPOS(key_row, key_col)] = cells[0][MCPOS(key_row, key_col)];
                break;

            default:
                draw_keymatrix_hsv_pixel(&issi, key_row, key_col, hsv_black);
                cells[1][MCPOS(key_row, key_col)] = 0;
                cell_colors[MCPOS(key_row, key_col)] = animation.hsv.h;
            }

            cells_changed += abs(cells[1][MCPOS(key_row, key_col)] - cells[0][MCPOS(key_row, key_col)]);

            if (cells[1][MCPOS(key_row, key_col)] != cells[0][MCPOS(key_row, key_col)])
                unchanged = false;
        }
    }

    // dprintf("cells changed %u", cells_changed);

    memcpy(cells[0], cells[1], MATRIX_ROWS * MATRIX_COLS * sizeof(uint8_t));

    if (unchanged)
        conway_rgb_init_cells();

    is31fl3733_91tkl_update_led_pwm(&issi);

    cycle++;
}

void conway_typematrix_row(uint8_t row_number, matrix_row_t row)
{
    if (cells_changed < 5)
        conway_init_cells();
}

void conway_animation_start(void)
{
    animation_prepare(true);
    cells[0] = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));
    cells[1] = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));
    cell_colors = (uint8_t *)calloc(MATRIX_ROWS * MATRIX_COLS, sizeof(uint8_t));
    cycle = animation.hsv.h;
    conway_rgb_init_cells();
    is31fl3733_91tkl_update_led_pwm(&issi);
}

void conway_animation_stop(void)
{
    animation_postpare();
    free(cells[0]);
    free(cells[1]);
    free(cell_colors);
}

void set_animation_conway()
{
    dprintf("conway\n");

    animation.delay_in_ms = FPS_TO_DELAY(5);
    animation.duration_in_ms = 0;

    animation.animationStart = &conway_animation_start;
    animation.animationStop = &conway_animation_stop;
    animation.animationLoop = &conway_rgb_animation_loop;
    animation.animation_typematrix_row = &conway_typematrix_row;
}

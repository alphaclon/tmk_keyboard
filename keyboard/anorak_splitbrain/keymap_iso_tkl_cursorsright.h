#ifndef __KEYMAP_ISO_TKL_CURSORSRIGHT__
#define __KEYMAP_ISO_TKL_CURSORSRIGHT__

/*
 * Tenkeyless keyboard default layout, ISO & ANSI (ISO is between Left Shift
 * and Z, and the ANSI \ key above Return/Enter is used for the additional ISO
 * switch in the ASD row next to enter.  Use NUBS as keycode for the first and
 * NUHS as the keycode for the second.
 *
 * Layer 0: Default ISO
 *
 * Ten-key-less, cursor keys on the right side
 *
 * ,---.   ,---------------. ,---------------. ,---------------. ,-----------.
 * |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12| |PrS|ScL|Pau|
 * `---'   `---------------' `---------------' `---------------' `-----------'
 * ,-----------------------------------------------------------. ,-----------.
 * |~  |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backsp | |Ins|Hom|PgU|
 * |-----------------------------------------------------------| |-----------|
 * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|     | |Del|End|PgD|
 * |-----------------------------------------------------------| `-----------'
 * |Caps  |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |
 * |-----------------------------------------------------------| ,-----------.
 * |Shft|ISO|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift     | |FN1|Up |FN2|
 * |-----------------------------------------------------------| ,-----------.
 * |Ctl|Gui|FN0|Alt|          Space            |Alt|Gui|App|Ctl| |Lef|Dow|Rig|
 * `-----------------------------------------------------------' `-----------'
 */

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: qwerty */
    KEYMAP_ISO_TKL_CURSORSRIGHT(\
         ESC,      F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,       PSCR,SLCK,BRK,  \
         GRV, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSPC,      INS, HOME,PGUP, \
         TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,           DEL, END, PGDN, \
        CAPS, A,   S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,NUHS,ENT,                       \
        LSFT,NUBS, Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RSFT,       FN1,  UP, FN2, \
        LCTL, FN0, LGUI,LALT,          SPC,                RALT,RGUI,APP, RCTL,      LEFT,DOWN,RGHT  ),
    /* 1: media keys */
    KEYMAP_ISO_TKL_CURSORSRIGHT(\
        TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,      TRNS,TRNS,SLEP, \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,MUTE,VOLD,VOLU,           TRNS,TRNS,TRNS, \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,MSTP,MPLY,MPRV,MNXT,MSEL,      TRNS,TRNS,TRNS, \
        TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,                      \
        TRNS,TRNS,TRNS,TRNS,CALC,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     CAPS,      TRNS,TRNS,TRNS, \
        TRNS,TRNS,TRNS,TRNS,          TRNS,               TRNS,TRNS,TRNS,TRNS,      TRNS,TRNS,TRNS  )
};

#endif

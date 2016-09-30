#ifndef __KEYMAP_ISO_TKL_CURSORSLEFT__
#define __KEYMAP_ISO_TKL_CURSORSLEFT__

/*
 * Tenkeyless keyboard default layout, ISO & ANSI (ISO is between Left Shift
 * and Z, and the ANSI \ key above Return/Enter is used for the additional ISO
 * switch in the ASD row next to enter.  Use NUBS as keycode for the first and
 * NUHS as the keycode for the second.
 *
 * Layer 0: Default ISO
 *
 * Ten-key-less, cursor keys on the left side
 *
 * ,-----------. ,---.   ,---------------. ,---------------. ,---------------.
 * |PrS|ScL|Pau| |Esc|   |F1 |F2 |F3 |F4 | |F5 |F6 |F7 |F8 | |F9 |F10|F11|F12|
 * `-----------' `---'   `---------------' `---------------' `---------------'
 * ,-----------. ,-----------------------------------------------------------.
 * |Ins|Hom|PgU| |~  |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backsp |
 * |-----------| |-----------------------------------------------------------|
 * |Del|End|PgD| |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|     |
 * `-----------' |-----------------------------------------------------------|
 *               |Caps  |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |
 * ,-----------. |-----------------------------------------------------------|
 * |FN1|Up |FN2| |Shft|ISO|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift     |
 * ,-----------. |-----------------------------------------------------------|
 * |Lef|Dow|Rig| |Ctl|Gui|FN0|Alt|          Space            |Alt|Gui|App|Ctl|
 * `-----------' `-----------------------------------------------------------'
 */

static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 0: qwerty */
    KEYMAP_ISO_TKL_CURSORSLEFT(\
         PSCR,SLCK,BRK,     ESC,     F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12,      \
         INS, HOME,PGUP,    GRV, 1,  2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSPC,     \
         DEL, END, PGDN,    TAB, Q,  W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,          \
                           CAPS, A,  S,   D,   F,   G,   H,   J,   K,   L,   SCLN,QUOT,NUHS,ENT,      \
          FN1,  UP, FN2,   LSFT,NUBS,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,     RSFT,     \
         LEFT,DOWN,RGHT    LCTL, FN0,LGUI,LALT,          SPC,                RALT,RGUI,APP, RCTL,     ),
    /* 1: media keys */
    KEYMAP_ISO_TKL_CURSORSLEFT(\
         TRNS,TRNS,SLEP,   TRNS,     TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     \
         TRNS,TRNS,TRNS,   TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,MUTE,VOLD,VOLU,          \
         TRNS,TRNS,TRNS,   TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,MSTP,MPLY,MPRV,MNXT,MSEL,     \
                           TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     \
         TRNS,TRNS,TRNS,   TRNS,TRNS,TRNS,TRNS,CALC,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,     CAPS,     \
         TRNS,TRNS,TRNS    TRNS,TRNS,TRNS,TRNS,          TRNS,               TRNS,TRNS,TRNS,TRNS,     )
};

#endif

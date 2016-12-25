hid_liberation firmware
======================
DIY daughterboard for Filco Majestouch TKL developed by Geekhack and Deskthority communities.
The PCB was engineered by bpiphany.

## Wiki on Deskthority.net
- [Instructions](http://deskthority.net/wiki/HID_Liberation_Device_-_Instructions)
- [Assembly Instructions](http://deskthority.net/wiki/HID_Liberation_Device_-_DIY_Instructions)


Build
-----
Move to this directory then just run `make` like:

    $ make -f Makefile.[pjrc|lufa] [ansi|iso|custom|alaricljs]

Use `Makefile.pjrc` if you want to use PJRC stack or use `Makefile.lufa` for LUFA stack.

|Esc||F1 |F2 |F3 |F4 ||F5 |F6 |F7 |F8 ||F9 |F10|F11|F12||FN4|FN5||PrS|ScL|Pau|
|~  |  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|Backsp |    |Ins|Hom|PgU|
|Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|     |    |Del|End|PgD|
|Caps  |  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|Return  |
|Shft|ISO|  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift     |    |FN1|Up |FN2|
|Ctl|Gui|FN0|Alt|Space || Space         |Alt|FN1|Gui|App|Ctl|    |Lef|Dow|Rig|

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



 0: Default layer (QWERT)

 | Esc |   1|  2|  3|  4|  5|
 |--------------------------|
 | Tab |   Q|  W|  E|  R|  T|
 |--------------------------|
 |    M|   A|  S|  D|  F|  G|
 |--------------------------|
 |Shift|   Z|  X|  C|  V|  B|
 |--------------------------|
 |LCtrl|FN0|FN14|LAlt|FN2|FN3|SPC|


 1: layer 1, QWERT,
    normal space on middle thumb key, enter on left thumb key
    toggled by FN14-layer: FN16

 |  Esc|   1|  2|  3|  4|  5|
 |  Tab|   Q|  W|  E|  R|  T|
 |    M|   A|  S|  D|  F|  G|
 |  Shift|   Z|  X|  C|  V|  B|
 |  LCtrl|  FN0|  FN14|  LAlt|  FN2|  SPC|  ENT|



3: layer 3, the layer selection layer
   activated by FN14

4: layer 4, TF2 F-key layer, thumb key
   activated by FN2 (leftmost thumb key)
   FN22: lock macro for A-key


5: layer 5, media and backlight layer,
    activated by FN0

    Fn5: CAPS inc all
    Fn6: LSFT dec all
    Fn4: LCTL toggle all
   Fn12: THUMB MID inc region
   Fn13: THUMB LFT dec region
   Fn15: THUMB RIG toggle region
   Fn16: LALT save

    Fn7: A WASD
    Fn8: S JUMP
    Fn9: D CTRL
   Fn10: F LOGO
   Fn11: G OTHR

   Fn23: start animation


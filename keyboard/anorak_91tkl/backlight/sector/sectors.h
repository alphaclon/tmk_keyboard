#ifndef _SECTORS_
#define _SECTORS_

#include <inttypes.h>

enum keyboard_sector
{
    WASDKeys = 0,
    ControlKeys,
    FunctionKeys,
    CursorKeys,
    OtherKeys,
    User1,
    User2,
    User3,
    User4
};

#define SECTOR_BV(arg) (1 << arg)

#define SECTOR_ALL_MASK 0xff
#define SECTOR_MAX 8

#endif

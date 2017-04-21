#ifndef _SECTORS_H_
#define _SECTORS_H_

enum keyboard_sector_t
{
    WASDKeys = 0,
    ControlKeys,
    FunctionKeys,
    CursorKeys,
    OtherKeys,
	NavKeys,
	ESCKey,
    User1,
    User2,
};

typedef enum keyboard_sector_t KeyboardSector;

#define SECTOR_BV(arg) (1 << arg)

#define SECTOR_ALL_MASK 0xff
#define SECTOR_MAX 6

#endif

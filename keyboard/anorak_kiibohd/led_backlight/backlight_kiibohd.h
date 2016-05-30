
enum backlight_region
{
	BACKLIGHT_REGION_ALL = 0x01,
    BACKLIGHT_REGION_WASD = 0x02,
    BACKLIGHT_REGION_JUMP = 0x04,
    BACKLIGHT_REGION_CONTROLS = 0x08,
    BACKLIGHT_REGION_LOGO = 0x10,
    BACKLIGHT_REGION_CASE = 0x20,
};

void backlight_internal_enable(void);
void backlight_pwm_increase(uint8_t level);
void backlight_pwm_decrease(uint8_t level);


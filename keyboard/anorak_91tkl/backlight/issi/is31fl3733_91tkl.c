
#include "is31fl3733_91tkl.h"
#include "is31fl3733_sdb.h"
#include "is31fl3733_iicrst.h"
#include "is31fl3733_twi.h"

#ifdef DEBUG_ISSI
#include "debug.h"
#else
#include "nodebug.h"
#endif


IS31FL3733_91TKL issi;

static IS31FL3733_RGB device_rgb_upper;
static IS31FL3733_RGB device_rgb_lower;

static IS31FL3733 device_upper;
static IS31FL3733 device_lower;

static bool is_initialized = false;

void is31fl3733_91tkl_init(IS31FL3733_91TKL *device)
{
    device->upper = &device_rgb_upper;
    device_rgb_upper.device = &device_upper;

    device_upper.gcc = 128;
    device_upper.is_master = true;
    device_upper.address = IS31FL3733_I2C_ADDR(ADDR_GND, ADDR_GND);
    device_upper.pfn_hardware_enable = &sdb_hardware_shutdown_enable_upper;
    device_upper.pfn_iic_reset = &iic_reset_upper;

    is31fl3733_rgb_init(device->upper);

    device->lower = &device_rgb_lower;
    device_rgb_lower.device = &device_lower;

    device_lower.gcc = 128;
    device_upper.is_master = false;
    device_lower.address = IS31FL3733_I2C_ADDR(ADDR_VCC, ADDR_GND);
    device_lower.pfn_hardware_enable = &sdb_hardware_shutdown_enable_lower;
    device_lower.pfn_iic_reset = &iic_reset_lower;

    is31fl3733_rgb_init(device->lower);

    is_initialized = true;
}

void is31fl3733_91tkl_hardware_shutdown(IS31FL3733_91TKL *device, bool enabled)
{
	if (!is31fl3733_91tkl_initialized())
		return;

	is31fl3733_hardware_shutdown(device->upper->device, enabled);
	is31fl3733_hardware_shutdown(device->lower->device, enabled);
}

void is31fl3733_91tkl_fill_rgb_masked(IS31FL3733_91TKL *device, RGB color)
{
	is31fl3733_fill_rgb_masked(device->upper, color);
	is31fl3733_fill_rgb_masked(device->lower, color);
}

void is31fl3733_91tkl_fill_hsv_masked(IS31FL3733_91TKL *device, HSV color)
{
	is31fl3733_fill_hsv_masked(device->upper, color);
	is31fl3733_fill_hsv_masked(device->lower, color);
}

void is31fl3733_91tkl_power_target(IS31FL3733_91TKL *device, uint16_t milliampere)
{
	dprintf("issi: power target %u\n", milliampere);

    uint32_t gcc2 = 5 + (2560L * (milliampere * 10L)) / 6720L;
    gcc2 /= 20;

	dprintf("issi: gcc %u\n", gcc2);

    device->upper->device->gcc = gcc2;
    device->lower->device->gcc = gcc2;

    is31fl3733_update_global_current_control(device->upper->device);
    is31fl3733_update_global_current_control(device->lower->device);
}

bool is31fl3733_91tkl_initialized(void)
{
    return is_initialized;
}

void is31fl3733_91tkl_update(IS31FL3733_91TKL *device)
{
	is31fl3733_update(device->upper->device);
	is31fl3733_update(device->lower->device);
}

void is31fl3733_91tkl_update_led_enable(IS31FL3733_91TKL *device)
{
	is31fl3733_update_led_enable(device->upper->device);
	is31fl3733_update_led_enable(device->lower->device);
}

void is31fl3733_91tkl_update_led_pwm(IS31FL3733_91TKL *device)
{
	is31fl3733_update_led_pwm(device->upper->device);
	is31fl3733_update_led_pwm(device->lower->device);
}

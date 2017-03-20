
#include "is31fl3733_rgb_91tkl.h"
#include "is31fl3733_twi.h"

IS31FL3733_RGB_91TKL issi;

IS31FL3733_RGB issi_rgb_1;
IS31FL3733_RGB issi_rgb_2;

IS31FL3733 issi_dev_1;
IS31FL3733 issi_dev_2;

static bool is_initialized = false;

void IS31FL3733_RGB_91TKL_Init(IS31FL3733_RGB_91TKL *device, enum IS31FL3733_RGB_ColorOrder color_order)
{
    device->device1 = &issi_rgb_1;
    issi_rgb_1.device = &issi_dev_1;

    issi_dev_1.gcc = 128;
    issi_dev_1.address = IS31FL3733_I2C_ADDR(ADDR_GND, ADDR_GND);
    issi_dev_1.pfn_i2c_read_reg = &i2c_read_reg;
    issi_dev_1.pfn_i2c_write_reg = &i2c_write_reg;
    issi_dev_1.pfn_i2c_read_reg8 = &i2c_read_reg8;
    issi_dev_1.pfn_i2c_write_reg8 = &i2c_write_reg8;

    IS31FL3733_RGB_Init(device->device1, color_order);

    device->device2 = &issi_rgb_2;
    issi_rgb_2.device = &issi_dev_2;

    issi_dev_2.gcc = 128;
    issi_dev_2.address = IS31FL3733_I2C_ADDR(ADDR_GND, ADDR_VCC);
    issi_dev_2.pfn_i2c_read_reg = &i2c_read_reg;
    issi_dev_2.pfn_i2c_write_reg = &i2c_write_reg;
    issi_dev_2.pfn_i2c_read_reg8 = &i2c_read_reg8;
    issi_dev_2.pfn_i2c_write_reg8 = &i2c_write_reg8;

    IS31FL3733_RGB_Init(device->device2, color_order);

    // TODO: disable hardware shutdown for both devices

    is_initialized = true;
}

void IS31FL3733_RGB_91TKL_SetBrightnessForMasked(IS31FL3733_RGB_91TKL *device, enum IS31FL3733_RGB_Color color,
                                                 uint8_t brightness)
{
    IS31FL3733_RGB_SetBrightnessForMasked(device->device1, color, brightness);
    IS31FL3733_RGB_SetBrightnessForMasked(device->device2, color, brightness);
}

void IS31FL3731_RGB_91TKL_Power_Target(IS31FL3733_RGB_91TKL *device, uint16_t ma)
{
    // TODO: set a value
    device->device1->device->gcc = 128;
    device->device2->device->gcc = 128;

    IS31FL3733_Update_GlobalConfiguration(device->device1->device);
    IS31FL3733_Update_GlobalConfiguration(device->device2->device);
}

bool IS31FL3731_RGB_91TKL_Initialized(void)
{
    return is_initialized;
}

void IS31FL3733_RGB_91TKL_Update(IS31FL3733_RGB_91TKL *device)
{
    IS31FL3733_Update(device->device1->device);
    IS31FL3733_Update(device->device2->device);
}

void IS31FL3733_RGB_91TKL_Update_LedEnableStates(IS31FL3733_RGB_91TKL *device)
{
    IS31FL3733_Update_LedEnableStates(device->device1->device);
    IS31FL3733_Update_LedEnableStates(device->device2->device);
}

void IS31FL3733_RGB_91TKL_Update_LedPwmValues(IS31FL3733_RGB_91TKL *device)
{
    IS31FL3733_Update_LedPwmValues(device->device1->device);
    IS31FL3733_Update_LedPwmValues(device->device2->device);
}

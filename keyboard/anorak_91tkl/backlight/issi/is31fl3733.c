#include "is31fl3733.h"

void IS31FL3733_SelectPage(IS31FL3733 *device, uint8_t page)
{
    // Unlock Command Register.
    IS31FL3733_WriteCommonReg(device, IS31FL3733_PSWL, IS31FL3733_PSWL_ENABLE);
    // Select requested page in Command Register.
    IS31FL3733_WriteCommonReg(device, IS31FL3733_PSR, page);
}

void IS31FL3733_WriteCommonReg(IS31FL3733 *device, uint8_t reg_addr, uint8_t reg_value)
{
    // Write value to register.
    device->pfn_i2c_write_reg8(device->address, reg_addr, reg_value);
}

void IS31FL3733_WritePagedReg(IS31FL3733 *device, uint16_t reg_addr, uint8_t reg_value)
{
    // Select register page.
    IS31FL3733_SelectPage(device, IS31FL3733_GET_PAGE(reg_addr));
    // Write value to register.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(reg_addr), &reg_value, sizeof(uint8_t));
}

void IS31FL3733_Init(IS31FL3733 *device)
{
	memset(device->leds, 0, IS31FL3733_LED_ENABLE_SIZE);
	memset(device->leds_mask, 0, IS31FL3733_LED_ENABLE_SIZE);
	memset(device->pwm, 0, IS31FL3733_LED_PWM_SIZE);

    // Clear software reset in configuration register.
    IS31FL3733_WritePagedReg(device, IS31FL3733_CR, IS31FL3733_CR_SSD);
    // Set master/slave bit
    IS31FL3733_WritePagedReg(device, IS31FL3733_CR,
                             (device->master ? IS31FL3733_CR_SYNC_MASTER : IS31FL3733_CR_SYNC_SLAVE));
    // Set global current control register.
    IS31FL3733_WritePagedReg(device, IS31FL3733_GCC, device->gcc);
}

void IS31FL3733_Update_GlobalConfiguration(IS31FL3733 *device)
{
    // Set global current control register.
    IS31FL3733_WritePagedReg(device, IS31FL3733_GCC, device->gcc);
}

void IS31FL3733_Software_Shutdown(IS31FL3733 *device, bool enabled)
{
    if (enabled)
    {
        // TODO: set flag
    }
    else
    {
        // Clear software reset in configuration register.
        IS31FL3733_WritePagedReg(device, IS31FL3733_CR, IS31FL3733_CR_SSD);
    }
}

void IS31FL3733_Update_LedEnableStates(IS31FL3733 *device)
{
    // Select IS31FL3733_LEDONOFF register page.
    IS31FL3733_SelectPage(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDONOFF));
    // Write LED states.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDONOFF), device->leds,
                              sizeof(device->leds));
}

void IS31FL3733_Update_LedPwmValues(IS31FL3733 *device)
{
    // Select IS31FL3733_LEDPWM register page.
    IS31FL3733_SelectPage(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDPWM));
    // Write PWM values.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDPWM), device->pwm,
                              sizeof(device->pwm));
}

void IS31FL3733_Update(IS31FL3733 *device)
{
    IS31FL3733_Update_LedEnableStates(device);
    IS31FL3733_Update_LedPwmValues(device);
}

void IS31FL3733_SetLedEnabled(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enabled)
{
    uint8_t offset;

    // TODO: fix this

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    // Set brightness level of selected LED.
    device->leds[offset] = enabled;
}

void IS31FL3733_SetLedPwm(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness)
{
    uint8_t offset;

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    // Set brightness level of selected LED.
    device->pwm[offset] = brightness;
}

void IS31FL3733_Fill(IS31FL3733 *device, uint8_t brightness)
{
    uint8_t i;

    // Set brightness level of all LED's.
    for (i = 0; i < IS31FL3733_LED_PWM_SIZE; i++)
    {
        device->pwm[i] = brightness;
    }
}

void IS31FL3733_CopyMask(IS31FL3733 *device, uint8_t *mask)
{
    memcpy(device->leds_mask, mask, IS31FL3733_LED_ENABLE_SIZE);
}

void IS31FL3733_ClearMask(IS31FL3733 *device)
{
    memset(device->leds_mask, 0, IS31FL3733_LED_ENABLE_SIZE);
}

void IS31FL3733_ApplyMask_Or(IS31FL3733 *device, uint8_t *mask)
{
    uint8_t i;

    for (i = 0; i < IS31FL3733_LED_ENABLE_SIZE; i++)
    {
        device->leds_mask[i] |= mask[i];
    }
}

void IS31FL3733_ApplyMask_NAnd(IS31FL3733 *device, uint8_t *mask)
{
    uint8_t i;

    for (i = 0; i < IS31FL3733_LED_ENABLE_SIZE; i++)
    {
        device->leds_mask[i] &= ~(mask[i]);
    }
}

void IS31FL3733_SetBrightnessForMasked(IS31FL3733 *device, uint8_t brightness)
{
    uint8_t i;
    uint8_t mask_byte;
    uint8_t mask_bit;

    // Set brightness level of all LED's.
    for (i = 0; i < IS31FL3733_LED_PWM_SIZE; i++)
    {
        uint8_t mask_byte = i / 8;
        uint8_t mask_bit = i % 8;

        if (device->leds_mask[mask_byte] & (1 << mask_bit))
            device->pwm[i] = brightness;
    }
}

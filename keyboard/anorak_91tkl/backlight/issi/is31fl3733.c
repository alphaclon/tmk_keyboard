#include "is31fl3733.h"
#include <string.h>

void is31fl3733_select_page(IS31FL3733 *device, uint8_t page)
{
    // Unlock Command Register.
    is31fl3733_write_common_reg(device, IS31FL3733_PSWL, IS31FL3733_PSWL_ENABLE);
    // Select requested page in Command Register.
    is31fl3733_write_common_reg(device, IS31FL3733_PSR, page);
}

void is31fl3733_write_common_reg(IS31FL3733 *device, uint8_t reg_addr, uint8_t reg_value)
{
    // Write value to register.
    device->pfn_i2c_write_reg8(device->address, reg_addr, reg_value);
}

void is31fl3733_write_paged_reg(IS31FL3733 *device, uint16_t reg_addr, uint8_t reg_value)
{
    // Select register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(reg_addr));
    // Write value to register.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(reg_addr), &reg_value, sizeof(uint8_t));
}

void is31fl3733_init(IS31FL3733 *device)
{
    memset(device->leds, 0, IS31FL3733_LED_ENABLE_SIZE);
    memset(device->leds, 0, IS31FL3733_LED_ENABLE_SIZE);
    memset(device->pwm, 0, IS31FL3733_LED_PWM_SIZE);

    is31fl3733_hardware_shutdown(device, false);
    is31fl3733_software_shutdown(device, true);

    // Clear software reset in configuration register.
    is31fl3733_write_paged_reg(device, IS31FL3733_CR, IS31FL3733_CR_SSD);
    // Set master/slave bit
    is31fl3733_write_paged_reg(device, IS31FL3733_CR,
                               (device->master ? IS31FL3733_CR_SYNC_MASTER : IS31FL3733_CR_SYNC_SLAVE));
    // Set global current control register.
    is31fl3733_write_paged_reg(device, IS31FL3733_GCC, device->gcc);

    // clear all LEDs
    is31fl3733_disable_all_leds(device);
    is31fl3733_fill(device, 0);

    is31fl3733_software_shutdown(device, false);
}

void is31fl3733_update_global_gonfiguration(IS31FL3733 *device)
{
    // Set global current control register.
    is31fl3733_write_paged_reg(device, IS31FL3733_GCC, device->gcc);
}

void is31fl3733_software_shutdown(IS31FL3733 *device, bool enabled)
{
    if (enabled)
    {
        // TODO: set flag
    }
    else
    {
        // Clear software reset in configuration register.
        is31fl3733_write_paged_reg(device, IS31FL3733_CR, IS31FL3733_CR_SSD);
    }
}

void is31fl3733_hardware_shutdown(IS31FL3733 *device, bool enabled)
{
	device->pfn_hardware_enable(enabled);
}

void is31fl3733_update_led_enable(IS31FL3733 *device)
{
    // Select IS31FL3733_LEDONOFF register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDONOFF));
    // Write LED states.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDONOFF), device->leds,
                              sizeof(device->leds));
}

void is31fl3733_update_led_pwm(IS31FL3733 *device)
{
    // Select IS31FL3733_LEDPWM register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDPWM));
    // Write PWM values.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDPWM), device->pwm,
                              sizeof(device->pwm));
}

void is31fl3733_Update(IS31FL3733 *device)
{
    is31fl3733_update_led_enable(device);
    is31fl3733_update_led_pwm(device);
}

void is31fl3733_set_led(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enabled)
{
    uint8_t offset;

    // TODO: fix this

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    uint8_t mask_byte = offset / 8;
    uint8_t mask_bit = (1 << (offset % 8));

    if (enabled)
    {
        device->leds[mask_byte] |= mask_bit;
    }
    else
    {
        device->leds[mask_byte] &= ~(mask_bit);
    }
}

void is31fl3733_set_led_masked(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enabled)
{
    uint8_t offset;

    // TODO: fix this

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    uint8_t mask_byte = offset / 8;
    uint8_t mask_bit = (1 << (offset % 8));

    if (!(device->leds[mask_byte] & mask_bit))
        return;

    if (enabled)
    {
        device->leds[mask_byte] |= mask_bit;
    }
    else
    {
        device->leds[mask_byte] &= ~(mask_bit);
    }
}

void is31fl3733_led_disable_all(IS31FL3733 *device)
{
    memset(device->leds, 0, IS31FL3733_LED_ENABLE_SIZE);
}

void is31fl3733_enable_leds_by_mask(IS31FL3733 *device, uint8_t *mask)
{
    for (uint8_t i = 0; i < IS31FL3733_LED_ENABLE_SIZE; i++)
    {
        device->leds[i] |= mask[i];
    }
}

void is31fl3733_disable_leds_by_mask(IS31FL3733 *device, uint8_t *mask)
{
    for (uint8_t i = 0; i < IS31FL3733_LED_ENABLE_SIZE; i++)
    {
        device->leds[i] &= ~(mask[i]);
    }
}

void is31fl3733_set_pwm(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness)
{
    uint8_t offset;

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    // Set brightness level of selected LED.
    device->pwm[offset] = brightness;
}

void is31fl3733_set_pwm_masked(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness)
{
    uint8_t offset;

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;

    // Set brightness level of selected LED.
    uint8_t mask_byte = offset / 8;
    uint8_t mask_bit = offset % 8;

    if (device->mask[mask_byte] & (1 << mask_bit))
        device->pwm[offset] = brightness;
}

void is31fl3733_fill(IS31FL3733 *device, uint8_t brightness)
{
    uint8_t i;

    // Set brightness level of all LED's.
    for (i = 0; i < IS31FL3733_LED_PWM_SIZE; i++)
    {
        device->pwm[i] = brightness;
    }
}

void is31fl3733_fill_masked(IS31FL3733 *device, uint8_t brightness)
{
    uint8_t i;
    uint8_t mask_byte;
    uint8_t mask_bit;

    // Set brightness level of all LED's.
    for (i = 0; i < IS31FL3733_LED_PWM_SIZE; i++)
    {
        mask_byte = i / 8;
        mask_bit = i % 8;

        if (device->mask[mask_byte] & (1 << mask_bit))
            device->pwm[i] = brightness;
    }
}

uint8_t* is31fl3733_pwm_buffer(IS31FL3733 *device)
{
	return device->pwm;
}

void is31fl3733_set_mask(IS31FL3733 *device, uint8_t *mask)
{
    memcpy(device->mask, mask, IS31FL3733_LED_ENABLE_SIZE);
}

void is31fl3733_clear_mask(IS31FL3733 *device)
{
    memset(device->mask, 0, IS31FL3733_LED_ENABLE_SIZE);
}

void is31fl3733_or_mask(IS31FL3733 *device, uint8_t *mask)
{
    for (uint8_t i = 0; i < IS31FL3733_LED_ENABLE_SIZE; i++)
    {
        device->mask[i] |= mask[i];
    }
}

void is31fl3733_nand_mask(IS31FL3733 *device, uint8_t *mask)
{
    for (uint8_t i = 0; i < IS31FL3733_LED_ENABLE_SIZE; i++)
    {
        device->mask[i] &= ~(mask[i]);
    }
}

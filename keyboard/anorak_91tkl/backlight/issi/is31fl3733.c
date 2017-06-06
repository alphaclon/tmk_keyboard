#include "is31fl3733.h"
#include <util/delay.h>
#include <string.h>

#ifdef DEBUG_ISSI
#include "debug.h"
#else
#include "nodebug.h"
#endif

void is31fl3733_write_common_reg(IS31FL3733 *device, uint8_t reg_addr, uint8_t reg_value)
{
    // Write value to register.
    device->pfn_i2c_write_reg8(device->address, reg_addr, reg_value);
}

void is31fl3733_select_page(IS31FL3733 *device, uint8_t page)
{
    // Unlock Command Register.
    is31fl3733_write_common_reg(device, IS31FL3733_PSWL, IS31FL3733_PSWL_ENABLE);
    // Select requested page in Command Register.
    is31fl3733_write_common_reg(device, IS31FL3733_PSR, page);
}

void is31fl3733_write_paged_reg(IS31FL3733 *device, uint16_t reg_addr, uint8_t reg_value)
{
    // Select register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(reg_addr));
    // Write value to register.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(reg_addr), &reg_value, sizeof(uint8_t));
}

uint8_t is31fl3733_read_paged_reg(IS31FL3733 *device, uint16_t reg_addr)
{
  uint8_t reg_value;

  // Select register page.
  is31fl3733_select_page (device, IS31FL3733_GET_PAGE(reg_addr));
  // Read value from register.
  device->pfn_i2c_read_reg (device->address, IS31FL3733_GET_ADDR(reg_addr), &reg_value, sizeof(uint8_t));
  // Return register value.
  return reg_value;
}

void is31fl3733_init(IS31FL3733 *device)
{
#ifdef DEBUG_ISSI
    device_upper.pfn_i2c_read_reg = &i2c_read_reg;
    device_upper.pfn_i2c_write_reg = &i2c_write_reg;
    device_upper.pfn_i2c_read_reg8 = &i2c_read_reg8;
    device_upper.pfn_i2c_write_reg8 = &i2c_write_reg8;
#else
    device_upper.pfn_i2c_read_reg = &i2c_read_reg;
    device_upper.pfn_i2c_write_reg = &i2c_queued_write_reg;
    device_upper.pfn_i2c_read_reg8 = &i2c_read_reg8;
    device_upper.pfn_i2c_write_reg8 = &i2c_queued_write_reg8;
#endif

    // TWI_detect: 0 = device accessible, 1= failed to access device
    bool device_present = (TWI_detect(device->address) ? false : true);
    dprintf("issi: device at 0x%X: %u\n", device->address, device_present);

    if (!device_present)
    {
        device_upper.pfn_i2c_read_reg = &i2c_dummy_read_reg;
        device_upper.pfn_i2c_write_reg = &i2c_dummy_write_reg;
        device_upper.pfn_i2c_read_reg8 = &i2c_dummy_read_reg8;
        device_upper.pfn_i2c_write_reg8 = &i2c_dummy_write_reg8;
    }

    memset(device->leds, 0, IS31FL3733_LED_ENABLE_SIZE);
    memset(device->mask, 0, IS31FL3733_LED_ENABLE_SIZE);
    memset(device->pwm, 0, IS31FL3733_LED_PWM_SIZE);

    device->cr = IS31FL3733_CR_SSD | (device->is_master ? IS31FL3733_CR_SYNC_MASTER : IS31FL3733_CR_SYNC_SLAVE);
    //device->cr = IS31FL3733_CR_SSD;

    is31fl3733_hardware_shutdown(device, true);
    dprintf("hsd\n");

    // Read reset register to reset device.
    is31fl3733_read_paged_reg(device, IS31FL3733_RESET);
    dprintf("reset\n");

    is31fl3733_update(device);

    // Set global current control register.
    is31fl3733_write_paged_reg(device, IS31FL3733_GCC, device->gcc);
    dprintf("gcc\n");

    // Clear software reset in configuration register.
    // Set master/slave bit
    is31fl3733_write_paged_reg(device, IS31FL3733_CR, device->cr);
    dprintf("ssd\n");

    is31fl3733_hardware_shutdown(device, false);
    dprintf("hsd\n");
}

void is31fl3733_update_global_current_control(IS31FL3733 *device)
{
    // Set global current control register.
    is31fl3733_write_paged_reg(device, IS31FL3733_GCC, device->gcc);
}

void is31fl3733_set_resistor_values(IS31FL3733 *device, IS31FL3733_RESISTOR swpur, IS31FL3733_RESISTOR cspdr)
{
	// Write resistor value to SWPUR register.
    is31fl3733_write_paged_reg(device, IS31FL3733_SWPUR, swpur);
    // Write resistor value to CSPDR register.
    is31fl3733_write_paged_reg(device, IS31FL3733_CSPDR, cspdr);
}

void is31fl3733_auto_breath_mode(IS31FL3733 *device, bool enable)
{
    if (enable)
    {
        device->cr |= IS31FL3733_CR_BEN;
    }
    else
    {
        device->cr &= ~(IS31FL3733_CR_BEN);
    }

    is31fl3733_write_paged_reg(device, IS31FL3733_CR, device->cr);
}

void is31fl3733_software_shutdown(IS31FL3733 *device, bool enable)
{
    if (enable)
    {
        device->cr &= ~(IS31FL3733_CR_SSD);
    }
    else
    {
        // Clear software shutdown in configuration register.
        device->cr |= IS31FL3733_CR_SSD;
    }

    is31fl3733_write_paged_reg(device, IS31FL3733_CR, device->cr);
}

void is31fl3733_hardware_shutdown(IS31FL3733 *device, bool enable)
{
    device->pfn_hardware_enable(enable);
}

void is31fl3733_write_interrupt_mask_register(IS31FL3733 *device, uint8_t imr)
{
    is31fl3733_write_common_reg(device, IS31FL3733_IMR, imr);
}

uint8_t is31fl3733_read_interrupt_status_register(IS31FL3733 *device)
{
    uint8_t isr = 0;
    device->pfn_i2c_read_reg8(device->address, IS31FL3733_ISR, &isr);
    return isr;
}

void is31fl3733_update_led_enable(IS31FL3733 *device)
{
	dprintf("issi: up led %X\n", device->address);

    // Select IS31FL3733_LEDONOFF register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDONOFF));

#if 0
    // Write LED states.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDONOFF), device->leds,
                              sizeof(device->leds));
#else
    // Write LED states.
    for (uint8_t offset = 0; offset < IS31FL3733_LED_ENABLE_SIZE; offset += IS31FL3733_LED_ENABLE_SIZE / 2)
    {
        device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDONOFF) + offset,
                                  device->leds + offset, IS31FL3733_LED_ENABLE_SIZE / 2);
    }
#endif
}

void is31fl3733_update_led_pwm(IS31FL3733 *device)
{
	dprintf("issi: up pwm %X\n", device->address);

    // Select IS31FL3733_LEDPWM register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDPWM));

#if 0
    // Write PWM values.
    device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDPWM), device->pwm,
                              sizeof(device->pwm));
#else
    // Write PWM values.
    /*
    for (uint8_t sw = 0; sw < IS31FL3733_SW; ++sw)
    {
        device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDPWM) + (sw * IS31FL3733_CS),
                                  device->pwm + (sw * IS31FL3733_CS), IS31FL3733_CS);
    }
    */
    for (uint8_t offset = 0; offset < IS31FL3733_LED_PWM_SIZE; offset += IS31FL3733_CS)
    {
        device->pfn_i2c_write_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDPWM) + offset,
                                  device->pwm + offset, IS31FL3733_CS);
    }
#endif
}

void is31fl3733_update(IS31FL3733 *device)
{
    is31fl3733_update_led_enable(device);
    is31fl3733_update_led_pwm(device);
}

void is31fl3733_set_led(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enabled)
{
    uint8_t offset;

	// Set state of individual LED.
	// Calculate LED bit offset.
	offset = (sw << 1) + (cs / 8);

	dprintf("set_led: dev:%X, cs:%u, sw:%u, on:%u -> offset: %u", device->address, cs, sw, enabled, offset);

	// Update state of LED in internal buffer.
	if (enabled)
	{
		// Set bit for selected LED.
		device->leds[offset] |= 0x01 << (cs % 8);
	}
	else
	{
		// Clear bit for selected LED.
		device->leds[offset] &= ~(0x01 << (cs % 8));
	}

	dprintf(" leds: %u\n", device->leds[offset]);
}

void is31fl3733_set_led_masked(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enabled)
{
    uint8_t offset;

    // Calculate LED offset in RAM buffer.
	offset = (sw << 1) + (cs / 8);
    uint8_t mask_bit = (0x01 << (cs % 8));

    dprintf("set_led_masked: dev:%X, cs:%u, sw:%u, on:%u -> offset: %u, bit: %u, mask: %u", device->address, cs, sw, enabled, offset, mask_bit, device->mask[offset]);

    if (!(device->mask[offset] & mask_bit))
        return;

    if (enabled)
    {
        device->leds[offset] |= mask_bit;
    }
    else
    {
        device->leds[offset] &= ~(mask_bit);
    }
}

void is31fl3733_led_disable_all(IS31FL3733 *device)
{
    memset(device->leds, 0x00, IS31FL3733_LED_ENABLE_SIZE);
}

void is31fl3733_led_enable_all(IS31FL3733 *device)
{
    memset(device->leds, 0xff, IS31FL3733_LED_ENABLE_SIZE);
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

uint8_t is31fl3733_get_pwm(IS31FL3733 *device, uint8_t cs, uint8_t sw)
{
    uint8_t offset;

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    // Set brightness level of selected LED.
    return device->pwm[offset];
}

void is31fl3733_set_pwm_masked(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness)
{
    uint8_t offset;

	offset = (sw << 1) + (cs / 8);
    uint8_t mask_bit = (0x01 << (cs % 8));

    dprintf("set_led_masked: dev:%X, cs:%u, sw:%u, bri:%u -> offset: %u, bit: %u, mask: %u", device->address, cs, sw, brightness, offset, mask_bit, device->mask[offset]);

    if (!(device->mask[offset] & mask_bit))
        return;

    // Calculate LED offset in RAM buffer.
    offset = sw * IS31FL3733_CS + cs;
    device->pwm[offset] = brightness;
}

void is31fl3733_fill(IS31FL3733 *device, uint8_t brightness)
{
	// Set brightness level of all LED's.
	memset(device->pwm, brightness, IS31FL3733_LED_PWM_SIZE);
}

void is31fl3733_fill_masked(IS31FL3733 *device, uint8_t brightness)
{
    uint8_t i;
    uint8_t offset;
    uint8_t mask_bit;

    // Set brightness level of all LED's.
    for (i = 0; i < IS31FL3733_LED_PWM_SIZE; i++)
    {
    	offset = i / 8;
        mask_bit = (0x01 << (i % 8));

        if (device->mask[offset] & mask_bit)
        {
            device->pwm[i] = brightness;
        }
    }
}

uint8_t *is31fl3733_led_buffer(IS31FL3733 *device)
{
    return device->leds;
}

uint8_t *is31fl3733_pwm_buffer(IS31FL3733 *device)
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

/*
By setting the OSD bit of the Configuration Register (PG3, 00h) from "0" to “1”, the LED Open Register
and LED Short Register will start to store the open/short information and after at least 2 scanning
cycle (3.264ms) the MCU can get the open/short information by reading the 18h~2fh/30h~47h, for
those dots are turned off via LED On/Off Registers (PG0, 00h~17h), the open/short data will not get
refreshed when setting the OSD bit of the Configuration Register (PG3, 00h) from "0" to “1”.

The Global Current Control Register (PG3, 01h) need to set to 0x01 in order to get the right open/short data.

The detect action is one-off event and each time before reading out the open/short information, the
OSD bit of the Configuration Register (PG3, 00h) need to be set from "0" to “1” (clear before set operation).
*/

void is31fl3733_detect_led_open_short_states(IS31FL3733 *device)
{
	is31fl3733_write_paged_reg(device, IS31FL3733_GCC, 0x01);

	is31fl3733_led_enable_all(device);
	is31fl3733_fill(device, 0x7f);
	is31fl3733_update(device);

	is31fl3733_write_paged_reg(device, IS31FL3733_CR, device->cr & ~IS31FL3733_CR_OSD);
    is31fl3733_write_paged_reg(device, IS31FL3733_CR, device->cr | IS31FL3733_CR_OSD);

    _delay_ms(5);
}

void is31fl3733_read_led_open_states(IS31FL3733 *device)
{
    // Select IS31FL3733_LEDOPEN register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDOPEN));

#if 0
    // Read LED open states.
    device->pfn_i2c_read_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDOPEN), device->leds,
                              sizeof(device->leds));
#else
    // Read LED open states.
    for (uint8_t offset = 0; offset < IS31FL3733_LED_ENABLE_SIZE; offset += IS31FL3733_LED_ENABLE_SIZE / 2)
    {
        device->pfn_i2c_read_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDOPEN) + offset,
                                 device->leds + offset, IS31FL3733_LED_ENABLE_SIZE / 2);
    }
#endif
}

void is31fl3733_read_led_short_states(IS31FL3733 *device)
{
    // Select IS31FL3733_LEDSHORT register page.
    is31fl3733_select_page(device, IS31FL3733_GET_PAGE(IS31FL3733_LEDSHORT));

#if 0
    // Read LED short states.
    device->pfn_i2c_read_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDSHORT), device->leds,
                              sizeof(device->leds));
#else
    // Read LED short states.
    for (uint8_t offset = 0; offset < IS31FL3733_LED_ENABLE_SIZE; offset += IS31FL3733_LED_ENABLE_SIZE / 2)
    {
        device->pfn_i2c_read_reg(device->address, IS31FL3733_GET_ADDR(IS31FL3733_LEDSHORT) + offset,
                                 device->leds + offset, IS31FL3733_LED_ENABLE_SIZE / 2);
    }
#endif
}

void is31fl3733_dump_led_buffer(IS31FL3733 *device)
{
    dprintf("led buffer\n");
    for (uint8_t sw = 0; sw < IS31FL3733_SW; ++sw)
    {
        dprintf("%02u: ");
        for (uint8_t cs = 0; cs < (IS31FL3733_CS / 8); ++cs)
        {
            dprintf("%02X ", device->leds[sw * (IS31FL3733_CS / 8) + cs]);
        }
        dprintf("\n");
    }
}

void is31fl3733_dump_pwm_buffer(IS31FL3733 *device)
{
    dprintf("pwm buffer\n");
    for (uint8_t sw = 0; sw < IS31FL3733_SW; ++sw)
    {
        dprintf("%02u: ");
        for (uint8_t cs = 0; cs < IS31FL3733_CS; ++cs)
        {
            dprintf("%02X ", device->pwm[sw * IS31FL3733_CS + cs]);
        }
        dprintf("\n");
    }
}

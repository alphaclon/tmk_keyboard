/** ISSI IS31FL3733 header file.
  */
#ifndef _IS31FL3733_H_
#define _IS31FL3733_H_

#include <stdint.h>
#include <stdbool.h>

/** Number of CS lines.
  */
#define IS31FL3733_CS (16)

/** Number of SW lines.
  */
#define IS31FL3733_SW (12)

/** Number of used SW lines.
  */
#define IS31FL3733_USED_SW (9)

#define IS31FL3733_LED_PWM_SIZE (IS31FL3733_CS * IS31FL3733_SW)
#define IS31FL3733_LED_PWM_USED_SIZE (IS31FL3733_CS * IS31FL3733_USED_SW)
#define IS31FL3733_LED_ENABLE_SIZE (IS31FL3733_LED_PWM_SIZE / 8)

/** IS31FL3733 base address on I2C bus.
  */
#define IS31FL3733_I2C_BASE_ADDR (0xA0)

/** IS31FL3733 ADDR[2:1] connection.
  */
#define ADDR_GND (0x00) ///< ADDRx pin connected to GND.
#define ADDR_SCL (0x01) ///< ADDRx pin connected to SCL.
#define ADDR_SDA (0x02) ///< ADDRx pin connected to SDA.
#define ADDR_VCC (0x03) ///< ADDRx pin connected to VCC.

/** IS31FL3733 real address on I2C bus, see Table 1 on page 9 in datasheet.
    Example: IS31FL3733_I2C_ADDR(ADDR_SDA, ADDR_VCC) is 0xB6 address on I2C bus.
  */
#define IS31FL3733_I2C_ADDR(ADDR2, ADDR1) ((IS31FL3733_I2C_BASE_ADDR) | ((ADDR2) << 3) | ((ADDR1) << 1))

/** IS31FL3733 common registers.
  */
#define IS31FL3733_PSR (0xFD)  ///< Page select register. Write only.
#define IS31FL3733_PSWL (0xFE) ///< Page select register write lock. Read/Write.
#define IS31FL3733_IMR (0xF0)  ///< Interrupt mask register. Write only.
#define IS31FL3733_ISR (0xF1)  ///< Interrupt status register. Read only.

/** Registers in Page 0.
  */
#define IS31FL3733_LEDONOFF (0x0000) /// ON or OFF state control for each LED. Write only.
#define IS31FL3733_LEDOPEN (0x0018)  /// Open state for each LED. Read only.
#define IS31FL3733_LEDSHORT (0x0030) /// Short state for each LED. Read only.

/** Registers in Page 1.
  */
#define IS31FL3733_LEDPWM (0x0100) /// PWM duty for each LED. Write only.

/** Registers in Page 2.
  */
#define IS31FL3733_LEDABM (0x0200) /// Auto breath mode for each LED. Write only.

/** Registers in Page 3.
  */
#define IS31FL3733_CR (0x0300)    /// Configuration Register. Write only.
#define IS31FL3733_GCC (0x0301)   /// Global Current Control register. Write only.
#define IS31FL3733_ABM1 (0x0302)  /// Auto breath control register for ABM-1. Write only.
#define IS31FL3733_ABM2 (0x0306)  /// Auto breath control register for ABM-2. Write only.
#define IS31FL3733_ABM3 (0x030A)  /// Auto breath control register for ABM-3. Write only.
#define IS31FL3733_TUR (0x030E)   /// Time update register. Write only.
#define IS31FL3733_SWPUR (0x030F) /// SWy Pull-Up Resistor selection register. Write only.
#define IS31FL3733_CSPDR (0x0310) /// CSx Pull-Down Resistor selection register. Write only.
#define IS31FL3733_RESET (0x0311) /// Reset register. Read only.

/// Get register page.
#define IS31FL3733_GET_PAGE(reg_addr) (uint8_t)((reg_addr) >> 8)
/// Get register 8-bit address.
#define IS31FL3733_GET_ADDR(reg_addr) (uint8_t)(reg_addr)

/// PSWL register bits.
#define IS31FL3733_PSWL_DISABLE (0x00) /// Disable write to Page Select register.
#define IS31FL3733_PSWL_ENABLE (0xC5)  /// Enable write to Page select register.

/// IMR register bits.
#define IS31FL3733_IMR_IAC (0x08) /// Auto Clear Interrupt bit.
#define IS31FL3733_IMR_IAB (0x04) /// Auto Breath Interrupt bit.
#define IS31FL3733_IMR_IS (0x02)  /// Dot Short Interrupt bit.
#define IS31FL3733_IMR_IO (0x01)  /// Dot Open Interrupt bit.

/// ISR register bits.
#define IS31FL3733_ISR_ABM3 (0x10) /// Auto Breath Mode 3 Finish Bit.
#define IS31FL3733_ISR_ABM2 (0x08) /// Auto Breath Mode 2 Finish Bit.
#define IS31FL3733_ISR_ABM1 (0x04) /// Auto Breath Mode 1 Finish Bit.
#define IS31FL3733_ISR_SB (0x02)   /// Short Bit.
#define IS31FL3733_ISR_OB (0x01)   /// Open Bit.

/// ABM register bits.
#define IS31FL3733_LEDABM_PWM (0x00)  /// PWM control mode.
#define IS31FL3733_LEDABM_ABM1 (0x01) /// Auto Breath Mode 1.
#define IS31FL3733_LEDABM_ABM2 (0x02) /// Auto Breath Mode 2.
#define IS31FL3733_LEDABM_ABM3 (0x03) /// Auto Breath Mode 3.

/// CR register bits.
#define IS31FL3733_CR_SYNC_MASTER (0x40) /// Configure as clock master device.
#define IS31FL3733_CR_SYNC_SLAVE (0x80)  /// Configure as clock slave device.
#define IS31FL3733_CR_OSD (0x04)         /// Open/Short detection enable bit.
#define IS31FL3733_CR_BEN (0x02)         /// Auto breath mode enable bit.
#define IS31FL3733_CR_SSD (0x01)         /// Software shutdown bit.

/// LED state enumeration.
typedef enum {
  IS31FL3733_LED_STATE_OFF = 0x00, ///< LED is off.
  IS31FL3733_LED_STATE_ON  = 0x01  ///< LED is on.
} IS31FL3733_LED_STATE;

/// LED status enumeration.
typedef enum {
  IS31FL3733_LED_STATUS_NORMAL  = 0x00, ///< Normal LED status.
  IS31FL3733_LED_STATUS_OPEN    = 0x01, ///< LED is open.
  IS31FL3733_LED_STATUS_SHORT   = 0x02, ///< LED is short.
  IS31FL3733_LED_STATUS_UNKNOWN = 0x03  ///< Unknown LED status.
} IS31FL3733_LED_STATUS;

/// Pull-Up or Pull-Down resistor value.
typedef enum {
  IS31FL3733_RESISTOR_OFF = 0x00, ///< No resistor.
  IS31FL3733_RESISTOR_500 = 0x01, ///< 0.5 kOhm pull-up resistor.
  IS31FL3733_RESISTOR_1K  = 0x02, ///< 1.0 kOhm pull-up resistor.
  IS31FL3733_RESISTOR_2K  = 0x03, ///< 2.0 kOhm pull-up resistor.
  IS31FL3733_RESISTOR_4K  = 0x04, ///< 4.0 kOhm pull-up resistor.
  IS31FL3733_RESISTOR_8K  = 0x05, ///< 8.0 kOhm pull-up resistor.
  IS31FL3733_RESISTOR_16K = 0x06, ///< 16 kOhm pull-up resistor.
  IS31FL3733_RESISTOR_32K = 0x07  ///< 32 kOhm pull-up resistor.
} IS31FL3733_RESISTOR;


/** IS31FL3733 structure.
  */
struct IS31FL3733Device
{
    /// Address on I2C bus.
    uint8_t address;
    /// Configuration register
    uint8_t cr;
    /// Global Current Control value. Iout = (840 / Rext) * (GCC / 256). Rext = 20 kOhm, typically.
    uint8_t gcc;
    // This device is the master
    bool is_master;
    /// LED matrix enabled.
    uint8_t leds[IS31FL3733_LED_ENABLE_SIZE];
    /// LED matrix brightness.
    uint8_t pwm[IS31FL3733_LED_PWM_SIZE];
    /// LED matrix mask.
    uint8_t mask[IS31FL3733_LED_ENABLE_SIZE];
    /// Pointer to I2C write data to register function.
    uint8_t (*pfn_i2c_write_reg)(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
    /// Pointer to I2C read data from register function.
    uint8_t (*pfn_i2c_read_reg)(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *buffer, uint8_t count);
    /// Pointer to I2C write byte to register function.
    uint8_t (*pfn_i2c_write_reg8)(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);
    /// Pointer to I2C read byte from register function.
    uint8_t (*pfn_i2c_read_reg8)(uint8_t i2c_addr, uint8_t reg_addr, uint8_t *data);
    /// Hardware enable (SDB)
    void (*pfn_hardware_enable)(bool enabled);
    /// Hardware I2C reset (IICRSET)
    void (*pfn_iic_reset)(void);
};

typedef struct IS31FL3733Device IS31FL3733;

/// Init LED matrix for normal operation.
void is31fl3733_init(IS31FL3733 *device);

void is31fl3733_update_global_current_control(IS31FL3733 *device);
void is31fl3733_set_resistor_values(IS31FL3733 *device, IS31FL3733_RESISTOR swpur, IS31FL3733_RESISTOR cspdr);

void is31fl3733_write_interrupt_mask_register(IS31FL3733 *device, uint8_t imr);
uint8_t is31fl3733_read_interrupt_status_register(IS31FL3733 *device);

void is31fl3733_software_shutdown(IS31FL3733 *device, bool enable);
void is31fl3733_hardware_shutdown(IS31FL3733 *device, bool enable);

void is31fl3733_auto_breath_mode(IS31FL3733 *device, bool enable);

/// Update LED matrix with internal buffer values.
void is31fl3733_update(IS31FL3733 *device);
/// Update LED matrix LED enable/disable states with internal buffer values.
void is31fl3733_update_led_enable(IS31FL3733 *device);
/// Update LED matrix LED brightness values with internal buffer values.
void is31fl3733_update_led_pwm(IS31FL3733 *device);

/// Enable/disable LED. Brightness level is not changed.
void is31fl3733_set_led(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enable);
/// Enable/disable LED. Brightness level is not changed.
void is31fl3733_set_led_masked(IS31FL3733 *device, uint8_t cs, uint8_t sw, bool enabled);

// Detect open or short states of LEDs
// --> Sets all LEDs to enabled and all PWM values to 0x01
// --> Changes global current control register to 0x01
void is31fl3733_detect_led_open_short_states(IS31FL3733 *device);
// Read LED open states and store them into the led buffer
void is31fl3733_read_led_open_states(IS31FL3733 *device);
// Read LED short states and store them into the led buffer
void is31fl3733_read_led_short_states(IS31FL3733 *device);

void is31fl3733_led_disable_all(IS31FL3733 *device);
void is31fl3733_led_enable_all(IS31FL3733 *device);
void is31fl3733_enable_leds_by_mask(IS31FL3733 *device, uint8_t *mask);
void is31fl3733_disable_leds_by_mask(IS31FL3733 *device, uint8_t *mask);

/// Set LED brightness level.
void is31fl3733_set_pwm(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness);
/// Set LED brightness level.
void is31fl3733_direct_set_pwm(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness);
/// Get LED brightness level.
uint8_t is31fl3733_get_pwm(IS31FL3733 *device, uint8_t cs, uint8_t sw);
/// Set LED brightness level.
void is31fl3733_set_pwm_masked(IS31FL3733 *device, uint8_t cs, uint8_t sw, uint8_t brightness);
/// Set brightness level for all LEDs.
void is31fl3733_fill(IS31FL3733 *device, uint8_t brightness);
/// Set brightness level for all LEDs.
void is31fl3733_fill_masked(IS31FL3733 *device, uint8_t brightness);

uint8_t* is31fl3733_led_buffer(IS31FL3733 *device);
uint8_t* is31fl3733_pwm_buffer(IS31FL3733 *device);

void is31fl3733_set_mask(IS31FL3733 *device, uint8_t *mask);
void is31fl3733_clear_mask(IS31FL3733 *device);
void is31fl3733_or_mask(IS31FL3733 *device, uint8_t *mask);
void is31fl3733_nand_mask(IS31FL3733 *device, uint8_t *mask);

/// Select active page.
void is31fl3733_select_page(IS31FL3733 *device, uint8_t page);

/// Write to common register.
void is31fl3733_write_common_reg(IS31FL3733 *device, uint8_t reg_addr, uint8_t reg_value);
/// Write to paged register.
void is31fl3733_write_paged_reg(IS31FL3733 *device, uint16_t reg_addr, uint8_t reg_value);


#ifdef DEBUG_ISSI
void is31fl3733_update_led_pwm_fast(IS31FL3733 *device);
#endif

#endif /* _IS31FL3733_H_ */

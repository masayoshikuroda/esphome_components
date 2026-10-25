#pragma once

#include <string>

#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace m5pm1 {

class M5PM1Component : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;

  float get_setup_priority() const override {
    return esphome::setup_priority::HARDWARE + 50;
  }

  void set_ldo_enabled(bool on);
  void set_dcdc_enabled(bool on);
  void set_5v_enabled(bool on);
  void set_charging_enabled(bool on);
  void set_standby(bool on);
  void set_speaker_amplifier(bool on);
  void shutdown(bool hold = true);

 protected:
  uint8_t _read_reg(uint8_t reg);
  void _write_reg(uint8_t reg, uint8_t val);
  void _update_reg(uint8_t reg, uint8_t mask, bool set);

  // M5PM1 registers
  static const uint8_t REG_I2C_CFG = 0x09;
  static const uint8_t REG_WDT_CNT = 0x0A;

  static const uint8_t REG_PWR_SRC = 0x04;
  static const uint8_t REG_PWR_CFG = 0x06;

  static const uint8_t REG_SYS_CMD = 0x0C;

  static const uint8_t REG_GPIO_MODE = 0x10;
  static const uint8_t REG_GPIO_OUT = 0x11;
  static const uint8_t REG_GPIO_IN = 0x12;
  static const uint8_t REG_GPIO_DRV = 0x13;

  static const uint8_t REG_GPIO_FUNC0 = 0x16;
  static const uint8_t REG_GPIO_FUNC1 = 0x17;

  static const uint8_t REG_VBAT_L = 0x22;
  static const uint8_t REG_VBAT_H = 0x23;

  // PWR_CFG
  static const uint8_t PWR_CFG_CHG_EN = (1 << 0);
  static const uint8_t PWR_CFG_DCDC_EN = (1 << 1);
  static const uint8_t PWR_CFG_LDO_EN = (1 << 2);
  static const uint8_t PWR_CFG_BOOST_EN = (1 << 3);
  static const uint8_t PWR_CFG_LED_CTRL = (1 << 4);

  // GPIO
  static const uint8_t GPIO_FUNC_GPIO = 0b00;
  static const uint8_t GPIO_MODE_INPUT = 0;
  static const uint8_t GPIO_MODE_OUTPUT = 1;

  static const uint8_t GPIO_DRIVE_PUSHPULL = 0;
  static const uint8_t GPIO_DRIVE_OPENDRAIN = 1;

  static const uint8_t GPIO_STATE_LOW = 0;
  static const uint8_t GPIO_STATE_HIGH = 1;
};

}  // namespace m5pm1
}  // namespace esphome
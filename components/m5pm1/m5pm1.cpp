#include "m5pm1.h"

#include "esphome/core/log.h"

namespace esphome {
namespace m5pm1 {

static const char *const TAG = "m5pm1";

void M5PM1Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5PM1 @ 0x%02X", this->address_);
  
  // Disable M5PM1 I2C idle sleep.
  this->_write_reg(REG_I2C_CFG, 0x00);

  // Disable PMIC watchdog
  this->_write_reg(REG_WDT_CNT, 0x00);

  uint8_t pwr_cfg = this->_read_reg(REG_PWR_CFG);
  uint8_t gpio_out = this->_read_reg(REG_GPIO_OUT);
  ESP_LOGD(TAG, "Initial PWR_CFG=0x%02X GPIO_OUT=0x%02X", pwr_cfg, gpio_out);

  // L1
  this->set_ldo_enabled(true);

  // L3B
  this->set_standby(false);
  // SPK AMP
  this->set_speaker_amplifier(false);
}

void M5PM1Component::update() {
}

void M5PM1Component::dump_config() {
  ESP_LOGCONFIG(TAG, "M5PM1:");

  LOG_I2C_DEVICE(this);

  uint8_t pwr_src = this->_read_reg(REG_PWR_SRC);
  uint8_t pwr_cfg = this->_read_reg(REG_PWR_CFG);
  uint8_t gpio_mode = this->_read_reg(REG_GPIO_MODE);
  uint8_t gpio_out = this->_read_reg(REG_GPIO_OUT);
  uint8_t gpio_in = this->_read_reg(REG_GPIO_IN);
  uint8_t gpio_drv = this->_read_reg(REG_GPIO_DRV);

  ESP_LOGCONFIG(TAG, "PWR_SRC=0x%02X PWR_CFG=0x%02X", pwr_src, pwr_cfg);

  ESP_LOGCONFIG(
      TAG,
      "GPIO_MODE=0x%02X GPIO_OUT=0x%02X GPIO_IN=0x%02X GPIO_DRV=0x%02X",
      gpio_mode,
      gpio_out,
      gpio_in,
      gpio_drv
  );
}

uint8_t M5PM1Component::_read_reg(uint8_t reg) {
  uint8_t value = 0;

  if (this->read_register(reg, &value, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C read failed: register 0x%02X", reg);
  }

  return value;
}

void M5PM1Component::_write_reg(uint8_t reg, uint8_t value) {
  if (this->write_register(reg, &value, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C write failed: register 0x%02X", reg);
  }
}

void M5PM1Component::_update_reg(uint8_t reg, uint8_t mask, bool set) {
  uint8_t value = 0;

  if (this->read_register(reg, &value, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C read failed: register 0x%02X", reg);
  }

  if (set) {
    value |= mask;
  } else {
    value &= ~mask;
  }

  if (this->write_register(reg, &value, 1) != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "I2C write failed: register 0x%02X", reg);
  }
}

void M5PM1Component::set_ldo_enabled(bool on) {
  this->_update_reg(REG_PWR_CFG, PWR_CFG_LDO_EN, on);
}

void M5PM1Component::set_dcdc_enabled(bool on) {
  this->_update_reg(REG_PWR_CFG, PWR_CFG_DCDC_EN, on);
}

void M5PM1Component::set_5v_enabled(bool on) {
  this->_update_reg(REG_PWR_CFG, PWR_CFG_BOOST_EN, on);
}

void M5PM1Component::set_charging_enabled(bool on) {
  this->_update_reg(REG_PWR_CFG, PWR_CFG_CHG_EN, on);
}

// on ?  L3A : L3B
void M5PM1Component::set_standby(bool on) {
  uint8_t pin = 2;
  uint8_t field = 0b11 << (2*pin);
  uint8_t mask = 1 << pin;

  // GPIO2 -> ordinary GPIO
  this->_update_reg(REG_GPIO_FUNC0, field, false);

  // GPIO2 -> output
  this->_update_reg(REG_GPIO_MODE, mask, true);

  // GPIO2 -> push-pull
  this->_update_reg(REG_GPIO_DRV, mask, false);

  // GPIO2 LOW | HIGH
  this->_update_reg(REG_GPIO_OUT, mask, !on);

  ESP_LOGD(TAG, "Standby: %s", on ? "ON" : "OFF");
}

void M5PM1Component::set_speaker_amplifier(bool on) {
  uint8_t pin = 3;
  uint8_t field = 0b11 << (2*pin);
  uint8_t mask = 1 << pin;

  // GPIO3 function field = bits 7:6
  this->_update_reg(REG_GPIO_FUNC0, field, false);

  // GPIO3 -> output
  this->_update_reg(REG_GPIO_MODE, mask, true);

  // GPIO3 -> push-pull
  this->_update_reg(REG_GPIO_DRV, mask, false);

  // Amplifier ON | OFF
  this->_update_reg(REG_GPIO_OUT, mask, on);

  ESP_LOGD(TAG, "Speaker amplifier: %s", on ? "ON" : "OFF");
}

void M5PM1Component::shutdown(bool hold) {
  (void) hold;

  this->_write_reg(REG_SYS_CMD, (0xA << 4) | 0x01);
}

}  // namespace m5pm1
}  // namespace esphome
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/esphal.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace adc {

class ADCSensor : public sensor::PollingSensorComponent {
 public:
  /// Construct the ADCSensor with the provided pin and update interval in ms.
  explicit ADCSensor(const std::string &name, uint8_t pin, uint32_t update_interval);

#ifdef ARDUINO_ARCH_ESP32
  /// Set the attenuation for this pin. Only available on the ESP32.
  void set_attenuation(adc_attenuation_t attenuation);
#endif

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  /// Update adc values.
  void update() override;
  /// Setup ADc
  void setup() override;
  void dump_config() override;
  /// `HARDWARE_LATE` setup priority.
  float get_setup_priority() const override;

#ifdef ARDUINO_ARCH_ESP8266
  std::string unique_id() override;
#endif

 protected:
  uint8_t pin_;

#ifdef ARDUINO_ARCH_ESP32
  adc_attenuation_t attenuation_{ADC_0db};
#endif
};

}  // namespace adc
}  // namespace esphome

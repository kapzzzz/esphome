#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/log.h"
#include "esphome/core/defines.h"

#include "parser.h"
#include "fields.h"

namespace esphome {
namespace dsmr {

static constexpr uint32_t MAX_TELEGRAM_LENGTH = 1500;
static constexpr uint32_t READ_TIMEOUT_MS = 200;

using namespace ::dsmr::fields;

// DSMR_**_LIST generated by ESPHome and written in esphome/core/defines

#if !defined(DSMR_SENSOR_LIST) && !defined(DSMR_TEXT_SENSOR_LIST)
// Neither set, set it to a dummy value to not break build
#define DSMR_TEXT_SENSOR_LIST(F, SEP) F(identification)
#endif

#if defined(DSMR_SENSOR_LIST) && defined(DSMR_TEXT_SENSOR_LIST)
#define DSMR_BOTH ,
#else
#define DSMR_BOTH
#endif

#ifndef DSMR_SENSOR_LIST
#define DSMR_SENSOR_LIST(F, SEP)
#endif

#ifndef DSMR_TEXT_SENSOR_LIST
#define DSMR_TEXT_SENSOR_LIST(F, SEP)
#endif

#define DSMR_DATA_SENSOR(s) s
#define DSMR_COMMA ,

using MyData = ::dsmr::ParsedData<DSMR_TEXT_SENSOR_LIST(DSMR_DATA_SENSOR, DSMR_COMMA)
                                      DSMR_BOTH DSMR_SENSOR_LIST(DSMR_DATA_SENSOR, DSMR_COMMA)>;

class Dsmr : public Component, public uart::UARTDevice {
 public:
  Dsmr(uart::UARTComponent *uart, bool crc_check) : uart::UARTDevice(uart), crc_check_(crc_check) {}

  void loop() override;

  bool parse_telegram();

  void publish_sensors(MyData &data) {
#define DSMR_PUBLISH_SENSOR(s) \
  if (data.s##_present && this->s_##s##_ != nullptr) \
    s_##s##_->publish_state(data.s);
    DSMR_SENSOR_LIST(DSMR_PUBLISH_SENSOR, )

#define DSMR_PUBLISH_TEXT_SENSOR(s) \
  if (data.s##_present && this->s_##s##_ != nullptr) \
    s_##s##_->publish_state(data.s.c_str());
    DSMR_TEXT_SENSOR_LIST(DSMR_PUBLISH_TEXT_SENSOR, )
  };

  void dump_config() override;

  void set_decryption_key(const std::string &decryption_key);

// Sensor setters
#define DSMR_SET_SENSOR(s) \
  void set_##s(sensor::Sensor *sensor) { s_##s##_ = sensor; }
  DSMR_SENSOR_LIST(DSMR_SET_SENSOR, )

#define DSMR_SET_TEXT_SENSOR(s) \
  void set_##s(text_sensor::TextSensor *sensor) { s_##s##_ = sensor; }
  DSMR_TEXT_SENSOR_LIST(DSMR_SET_TEXT_SENSOR, )

 protected:
  void receive_telegram_();
  void receive_encrypted_();

  /// Wait for UART data to become available within the read timeout.
  ///
  /// The smart meter might provide data in chunks, causing available() to
  /// return 0. When we're already reading a telegram, then we don't return
  /// right away (to handle further data in an upcoming loop) but wait a
  /// little while using this method to see if more data are incoming.
  /// By not returning, we prevent other components from taking so much
  /// time that the UART RX buffer overflows and bytes of the telegram get
  /// lost in the process.
  bool available_within_timeout_();

  // Telegram buffer
  char telegram_[MAX_TELEGRAM_LENGTH];
  int telegram_len_{0};

  // Serial parser
  bool header_found_{false};
  bool footer_found_{false};

// Sensor member pointers
#define DSMR_DECLARE_SENSOR(s) sensor::Sensor *s_##s##_{nullptr};
  DSMR_SENSOR_LIST(DSMR_DECLARE_SENSOR, )

#define DSMR_DECLARE_TEXT_SENSOR(s) text_sensor::TextSensor *s_##s##_{nullptr};
  DSMR_TEXT_SENSOR_LIST(DSMR_DECLARE_TEXT_SENSOR, )

  std::vector<uint8_t> decryption_key_{};
  bool crc_check_;
};
}  // namespace dsmr
}  // namespace esphome
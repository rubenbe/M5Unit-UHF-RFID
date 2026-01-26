#pragma once

#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include <map>
#include <string>

namespace esphome {
namespace m5rfid {

struct TagInfo {
  std::string epc;
  int8_t rssi;
  uint32_t last_seen;  // millis timestamp
  uint32_t first_seen; // for burst detection
};

class M5RFIDTextSensor : public PollingComponent, public text_sensor::TextSensor {
 public:
  M5RFIDTextSensor() : PollingComponent(50 /*ms*/) {}

  float get_setup_priority() const override { return esphome::setup_priority::LATE; }

  void setup() override;
  void update() override;
  void dump_config() override;

  // Configuration
  void set_dedup_timeout_ms(uint32_t timeout) { dedup_timeout_ms_ = timeout; }
  void set_burst_poll_cycles(uint8_t cycles) { burst_poll_cycles_ = cycles; }

 protected:
  int scanstate_ = 0;

  // Deduplication: EPC -> TagInfo
  std::map<std::string, TagInfo> seen_tags_;

  // Configuration
  uint32_t dedup_timeout_ms_ = 1000;  // Don't report same tag within 1 second
  uint8_t burst_poll_cycles_ = 5;     // Number of rapid polls per update for burst detection

  // Helpers
  bool should_report_tag(const std::string &epc, int8_t rssi);
  void cleanup_old_tags();
};

}  // namespace m5rfid
}  // namespace esphome

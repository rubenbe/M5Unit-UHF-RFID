#pragma once

#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace m5rfid {

class M5RFIDTextSensor : public PollingComponent, public text_sensor::TextSensor {
 public:
  M5RFIDTextSensor() : PollingComponent(50 /*ms*/) {}

  float get_setup_priority() const override { return esphome::setup_priority::LATE; }
  
  void setup() override;
  void update() override;
  void dump_config() override;

 protected:
  int scanstate_ = 0;
};

}  // namespace m5rfid
}  // namespace esphome

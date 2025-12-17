#include "m5rfid_text_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Include the RFID command headers
// The YAML includes will make these available
#include "src/RFID_command.h"

namespace esphome {
namespace m5rfid {

static const char *const TAG = "m5rfid.text_sensor";

// Global instances (matching M5RFID.h structure)
UHF_RFID RFID;
CardpropertiesInfo card;
ManyInfo cards;

void M5RFIDTextSensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up M5RFID Text Sensor...");
  
  scanstate_ = 0;
  ESP_LOGD(TAG, "Starting setup");
  RFID._debug = 1;
  Serial2.begin(115200, SERIAL_8N1, 32, 26);

  // UHF_RFID set UHF_RFID设置
  ESP_LOGD(TAG, "Setup 1");
  ESP_LOGD(TAG, "Manufacturer: %s", RFID.Inquire_manufacturer().c_str());
  ESP_LOGD(TAG, "Setup 2");
  RFID.Delay(100);
  ESP_LOGD(TAG, "Setup 4");
  RFID.Readcallback();
  RFID.clean_data();

  // Prompted to connect to UHF_RFID 提示连接UHF_RFID
  ESP_LOGI(TAG, "Please connect UHF_RFID to Port C");

  // Determined whether to connect to UHF_RFID 判断是否连接UHF_RFID
  String soft_version;
  soft_version = RFID.Query_software_version();
  int retry_count = 0;
  while (soft_version.indexOf("V2.3.5") == -1 && retry_count < 10) {
    RFID.clean_data();
    RFID.Delay(150);
    RFID.Delay(150);
    soft_version = RFID.Query_software_version();
    retry_count++;
  }

  if (soft_version.indexOf("V2.3.5") != -1) {
    ESP_LOGI(TAG, "UHF_RFID connected successfully");
    // The prompt will be RFID card close 提示将RFID卡靠近
    ESP_LOGI(TAG, "Please approach the RFID card you need to use");
  } else {
    ESP_LOGW(TAG, "UHF_RFID connection verification failed");
  }
  ESP_LOGW(TAG, "Set up work area %s", RFID.Set_up_work_area(0x03 /*EU*/));
  ESP_LOGW(TAG, "Set up transmission power %s", RFID.Set_transmission_Power(/*2000*/));
}

void M5RFIDTextSensor::update() {
  // Read one
  //  A read/write operation specifies a particular card
  card = RFID.A_single_poll_of_instructions_split(&scanstate_);
  if (card._ERROR.length() != 0) {
    // Error occurred, but don't log every time to avoid spam
  } else {
    if (card._EPC.length() == 24) {
      ESP_LOGD(TAG, "RSSI: %s, EPC: %s", card._RSSI.c_str(), card._EPC.c_str());
      this->publish_state(card._EPC.c_str());
    }
  }
  RFID.clean_data();  // Empty the data after using it
}

void M5RFIDTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "M5RFID Text Sensor:");
  LOG_TEXT_SENSOR("  ", "EPC", this);
}

}  // namespace m5rfid
}  // namespace esphome

#include "my_helpers.h"
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
  // Configure for EU/ETSI region
  ESP_LOGI(TAG, "Configuring for EU/ETSI region...");

  // Set work area to EU (0x03) - uses 865.1-868 MHz band
  String result = RFID.Set_up_work_area(0x03);
  if (result.length() > 0) {
    ESP_LOGI(TAG, "Work area set to EU: %s", result.c_str());
  } else {
    ESP_LOGW(TAG, "Failed to set EU work area");
  }
  RFID.Delay(50);

  // Enable automatic frequency hopping (required by ETSI)
  result = RFID.Set_up_automatic_frequency_modulation(0xFF);
  if (result.length() > 0) {
    ESP_LOGI(TAG, "Auto frequency hopping enabled: %s", result.c_str());
  } else {
    ESP_LOGW(TAG, "Failed to enable frequency hopping");
  }
  RFID.Delay(50);

  // Set transmission power to 20 dBm (100mW) - safe for EU/ETSI
  // Max EU/ETSI is 2W ERP, but 20 dBm conducted is conservative and compliant
  result = RFID.Set_transmission_Power(2000);
  if (result.length() > 0) {
    ESP_LOGI(TAG, "TX power set to 20 dBm: %s", result.c_str());
  } else {
    ESP_LOGW(TAG, "Failed to set TX power");
  }
  RFID.Delay(50);

  // Read back and log current settings for verification
  ReadInfo info = RFID.Read_working_area();
  ESP_LOGI(TAG, "Current region: %s", info.Region.c_str());

  info = RFID.Read_transmitting_power();
  ESP_LOGI(TAG, "Current TX power: %s", info.Pow.c_str());

  ESP_LOGI(TAG, "EU/ETSI configuration complete");
}
void M5RFIDTextSensor::update() {
  // Cleanup old tags periodically
  cleanup_old_tags();

  // Burst polling: do multiple rapid polls to catch all tags in range
  for (uint8_t i = 0; i < burst_poll_cycles_; i++) {
    // Use the non-split version for burst mode - it's synchronous but fast
    card = RFID.A_single_poll_of_instructions();

    if (card._ERROR.length() == 0 && card._EPC.length() == 24) {
      std::string epc(card._EPC.c_str());
      int8_t rssi = MyHelper::convert_rssi(card._RSSI.c_str());

      if (should_report_tag(epc, rssi)) {
        ESP_LOGI(TAG, "Tag detected - RSSI: %d dBm, EPC: %s", rssi, epc.c_str());
        this->publish_state(MyHelper::format_csv(card._EPC.c_str(), rssi));
      }
    }
    RFID.clean_data();

    // Small delay between burst polls to allow reader to settle
    if (i < burst_poll_cycles_ - 1) {
      RFID.Delay(10);
    }
  }
}

bool M5RFIDTextSensor::should_report_tag(const std::string &epc, int8_t rssi) {
  uint32_t now = millis();

  auto it = seen_tags_.find(epc);
  if (it == seen_tags_.end()) {
    // New tag - add to map and report
    TagInfo info;
    info.epc = epc;
    info.rssi = rssi;
    info.first_seen = now;
    info.last_seen = now;
    seen_tags_[epc] = info;
    return true;
  }

  // Existing tag - check if enough time has passed
  TagInfo &info = it->second;
  if (now - info.last_seen >= dedup_timeout_ms_) {
    // Enough time passed, report again
    info.rssi = rssi;
    info.first_seen = now;
    info.last_seen = now;
    return true;
  }

  // Update last_seen but don't report (dedup)
  info.last_seen = now;
  info.rssi = rssi;  // Keep latest RSSI
  return false;
}

void M5RFIDTextSensor::cleanup_old_tags() {
  uint32_t now = millis();
  // Remove tags not seen for 10x the dedup timeout (10 seconds by default)
  uint32_t cleanup_threshold = dedup_timeout_ms_ * 10;

  for (auto it = seen_tags_.begin(); it != seen_tags_.end();) {
    if (now - it->second.last_seen > cleanup_threshold) {
      ESP_LOGD(TAG, "Cleanup: removing stale tag %s", it->first.c_str());
      it = seen_tags_.erase(it);
    } else {
      ++it;
    }
  }
}

void M5RFIDTextSensor::dump_config() {
  ESP_LOGCONFIG(TAG, "M5RFID Text Sensor:");
  ESP_LOGCONFIG(TAG, "  Burst poll cycles: %d", burst_poll_cycles_);
  ESP_LOGCONFIG(TAG, "  Dedup timeout: %d ms", dedup_timeout_ms_);
  LOG_TEXT_SENSOR("  ", "EPC", this);
}

}  // namespace m5rfid
}  // namespace esphome
//

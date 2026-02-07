#pragma once
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <map>
#include <string>

#define SD_MOUNT "/sdcard"
#define SD_CS_PIN GPIO_NUM_4
#define SD_TAG_FILE SD_MOUNT "/tags.csv"
#define SD_RUNNERS_FILE SD_MOUNT "/runners.csv"

static bool sd_ready = false;
static bool sd_failed = false;
static std::map<std::string, std::string> runners;
static sdmmc_card_t *sd_card = nullptr;

// SPI bus already initialized by ESPHome on SPI2_HOST — skip re-init
static esp_err_t sd_spi_noinit(void) { return ESP_OK; }

inline bool sd_init() {
    if (sd_ready) return true;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 2,
        .allocation_unit_size = 16 * 1024,
    };

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI2_HOST;
    host.init = &sd_spi_noinit;

    sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot.gpio_cs = SD_CS_PIN;
    slot.host_id = SPI2_HOST;

    esp_err_t ret = esp_vfs_fat_sdspi_mount(SD_MOUNT, &host, &slot, &mount_cfg, &sd_card);
    if (ret != ESP_OK) {
        ESP_LOGW("sd", "SD mount failed: %s", esp_err_to_name(ret));
        sd_failed = true;
        return false;
    }

    sd_ready = true;
    ESP_LOGI("sd", "SD card mounted at " SD_MOUNT);

    // Create CSV header if file doesn't exist
    FILE *f = fopen(SD_TAG_FILE, "r");
    if (f) {
        fclose(f);
    } else {
        f = fopen(SD_TAG_FILE, "w");
        if (f) {
            fprintf(f, "count,timestamp,epc,name\n");
            fclose(f);
        }
    }
    return true;
}

inline void sd_load_runners() {
    if (!sd_ready) return;
    FILE *f = fopen(SD_RUNNERS_FILE, "r");
    if (!f) {
        ESP_LOGW("sd", "No runners.csv found");
        return;
    }
    char line[128];
    bool first = true;
    while (fgets(line, sizeof(line), f)) {
        char *comma = strchr(line, ',');
        if (!comma) continue;
        *comma = '\0';
        char *name = comma + 1;
        // Trim CR/LF
        char *nl = strchr(name, '\n'); if (nl) *nl = '\0';
        char *cr = strchr(name, '\r'); if (cr) *cr = '\0';
        // Skip header if present
        if (first) {
            first = false;
            if (strcasecmp(line, "epc") == 0) continue;
        }
        runners[std::string(line)] = std::string(name);
    }
    fclose(f);
    ESP_LOGI("sd", "Loaded %d runners", (int)runners.size());
}

inline std::string sd_lookup_runner(const std::string &epc) {
    auto it = runners.find(epc);
    return (it != runners.end()) ? it->second : std::string();
}

inline bool sd_append_tag(int count, const char* timestamp, const char* epc, const char* name) {
    if (!sd_ready) return false;
    FILE *f = fopen(SD_TAG_FILE, "a");
    if (!f) {
        ESP_LOGW("sd", "SD write failed, card removed?");
        sd_ready = false;
        sd_failed = true;
        return false;
    }
    fprintf(f, "%d,%s,%s,%s\n", count, timestamp, epc, name);
    fclose(f);
    return true;
}


#pragma once
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include <cstdio>

#define SD_MOUNT "/sdcard"
#define SD_CS_PIN GPIO_NUM_4
#define SD_TAG_FILE SD_MOUNT "/tags.csv"

static bool sd_ready = false;
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
            fprintf(f, "timestamp,epc\n");
            fclose(f);
        }
    }
    return true;
}

inline bool sd_append_tag(const char* timestamp, const char* epc) {
    if (!sd_ready) return false;
    FILE *f = fopen(SD_TAG_FILE, "a");
    if (!f) {
        ESP_LOGW("sd", "Failed to open " SD_TAG_FILE);
        return false;
    }
    fprintf(f, "%s,%s\n", timestamp, epc);
    fclose(f);
    return true;
}

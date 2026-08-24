#pragma once

// Keep personal values in include/local_config.h. That file is ignored by Git.
#if __has_include("local_config.h")
#include "local_config.h"
#endif

#ifndef WIFI_SSID_VALUE
#define WIFI_SSID_VALUE ""
#endif

#ifndef WIFI_PASSWORD_VALUE
#define WIFI_PASSWORD_VALUE ""
#endif

// Use the Windows LAN IPv4 that exposes WSL, never 127.0.0.1.
#ifndef ASSISTANT_BASE_URL_VALUE
#define ASSISTANT_BASE_URL_VALUE "http://192.168.1.50:3000"
#endif

#ifndef ESP32_API_TOKEN_VALUE
#define ESP32_API_TOKEN_VALUE ""
#endif

#ifndef DEVICE_ID_VALUE
#define DEVICE_ID_VALUE "esp-hi-3c-01"
#endif

#ifndef DEFAULT_3C_COMMAND_VALUE
#define DEFAULT_3C_COMMAND_VALUE "Cambia la tarea J10 a mensual"
#endif

namespace app_config {
static constexpr char wifiSsid[] = WIFI_SSID_VALUE;
static constexpr char wifiPassword[] = WIFI_PASSWORD_VALUE;
static constexpr char assistantBaseUrl[] = ASSISTANT_BASE_URL_VALUE;
static constexpr char apiToken[] = ESP32_API_TOKEN_VALUE;
static constexpr char deviceId[] = DEVICE_ID_VALUE;
static constexpr char defaultCommand[] = DEFAULT_3C_COMMAND_VALUE;
static constexpr unsigned long wifiRetryMs = 10000UL;
static constexpr unsigned long healthCheckMs = 30000UL;
static constexpr unsigned long httpTimeoutMs = 8000UL;
}

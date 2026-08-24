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
inline constexpr char wifiSsid[] = WIFI_SSID_VALUE;
inline constexpr char wifiPassword[] = WIFI_PASSWORD_VALUE;
inline constexpr char assistantBaseUrl[] = ASSISTANT_BASE_URL_VALUE;
inline constexpr char apiToken[] = ESP32_API_TOKEN_VALUE;
inline constexpr char deviceId[] = DEVICE_ID_VALUE;
inline constexpr char defaultCommand[] = DEFAULT_3C_COMMAND_VALUE;
inline constexpr unsigned long wifiRetryMs = 10'000;
inline constexpr unsigned long healthCheckMs = 30'000;
inline constexpr unsigned long httpTimeoutMs = 8'000;
}


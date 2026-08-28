#if defined(BOARD_ESP_HI_C3)

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_ST7789.h>
#include <ESP32Servo.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <WebServer.h>
#include <WiFi.h>

#include "app_config.h"

namespace pins {
constexpr int bootButton = 9;
constexpr int moveButton = 0;
constexpr int commandButton = 1;
constexpr int rgb = 8;
constexpr int displayMosi = 4;
constexpr int displayClock = 5;
constexpr int displayDc = 10;
constexpr int displayCs = -1;
constexpr int displayReset = -1;
constexpr int frontLeft = 21;
constexpr int frontRight = 19;
constexpr int backLeft = 20;
constexpr int backRight = 18;
}  // namespace pins

namespace {
WebServer web(80);
Adafruit_ST7789 display(&SPI, pins::displayCs, pins::displayDc, pins::displayReset);
Adafruit_NeoPixel leds(4, pins::rgb, NEO_GRB + NEO_KHZ800);
Servo servos[4];
const int servoPins[4] = {pins::frontLeft, pins::frontRight, pins::backLeft, pins::backRight};
const int neutral[4] = {90, 90, 90, 90};
const int direction[4] = {1, -1, 1, -1};

unsigned long lastWifiAttempt = 0;
unsigned long lastHealthCheck = 0;
bool backendAvailable = false;
String lastBackendMessage = "Sin verificar";
int movementIndex = 0;

struct DebouncedButton {
  int pin;
  bool stable = HIGH;
  bool previousRead = HIGH;
  unsigned long changedAt = 0;

  explicit DebouncedButton(int buttonPin) : pin(buttonPin) {}

  bool pressed() {
    const bool reading = digitalRead(pin);
    if (reading != previousRead) {
      previousRead = reading;
      changedAt = millis();
    }
    if (millis() - changedAt > 35 && reading != stable) {
      stable = reading;
      return stable == LOW;
    }
    return false;
  }
};

DebouncedButton healthButton{pins::bootButton};
DebouncedButton moveButton{pins::moveButton};
DebouncedButton commandButton{pins::commandButton};

void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  for (uint16_t index = 0; index < leds.numPixels(); ++index) leds.setPixelColor(index, leds.Color(red, green, blue));
  leds.show();
}

void drawFace(const char* state) {
  uint16_t background = ST77XX_BLACK;
  uint16_t eyeColor = ST77XX_WHITE;
  if (strcmp(state, "ok") == 0) background = display.color565(0, 55, 20);
  if (strcmp(state, "pending") == 0) background = display.color565(70, 45, 0);
  if (strcmp(state, "error") == 0) background = display.color565(70, 0, 0);
  if (strcmp(state, "offline") == 0) eyeColor = display.color565(130, 130, 130);

  display.fillScreen(background);
  if (strcmp(state, "error") == 0) {
    display.drawLine(34, 24, 54, 44, eyeColor);
    display.drawLine(54, 24, 34, 44, eyeColor);
    display.drawLine(106, 24, 126, 44, eyeColor);
    display.drawLine(126, 24, 106, 44, eyeColor);
  } else {
    display.fillRoundRect(30, 20, 28, 30, 8, eyeColor);
    display.fillRoundRect(102, 20, 28, 30, 8, eyeColor);
    display.fillCircle(44, 34, 5, ST77XX_BLACK);
    display.fillCircle(116, 34, 5, ST77XX_BLACK);
  }
  display.setTextColor(eyeColor);
  display.setTextSize(1);
  display.setCursor(4, 68);
  display.print("3C ");
  display.print(state);
}

int safeAngle(int servoIndex, int offset) {
  return constrain(neutral[servoIndex] + direction[servoIndex] * offset, 45, 135);
}

void pose(int frontLeft, int frontRight, int backLeft, int backRight, int durationMs = 180) {
  const int offsets[4] = {frontLeft, frontRight, backLeft, backRight};
  for (int index = 0; index < 4; ++index) servos[index].write(safeAngle(index, offsets[index]));
  delay(durationMs);
  web.handleClient();
}

void stand() { pose(0, 0, 0, 0, 120); }

bool runMovement(const String& action) {
  if (action == "stop" || action == "stand") {
    stand();
  } else if (action == "forward") {
    pose(22, -22, -22, 22); pose(-22, 22, 22, -22); stand();
  } else if (action == "backward") {
    pose(-22, 22, 22, -22); pose(22, -22, -22, 22); stand();
  } else if (action == "left") {
    pose(-25, -25, 25, 25); pose(18, 18, -18, -18); stand();
  } else if (action == "right") {
    pose(25, 25, -25, -25); pose(-18, -18, 18, 18); stand();
  } else if (action == "sit") {
    pose(-8, -8, 32, 32, 250);
  } else if (action == "shake") {
    pose(34, 0, 12, 12, 300); stand();
  } else {
    return false;
  }
  return true;
}

String jsonEscape(const String& input) {
  String output;
  output.reserve(input.length() + 16);
  for (size_t index = 0; index < input.length(); ++index) {
    const char value = input[index];
    if (value == '\\' || value == '"') { output += '\\'; output += value; }
    else if (value == '\n') output += "\\n";
    else if (static_cast<uint8_t>(value) >= 0x20) output += value;
  }
  return output;
}

String endpoint(const char* path) {
  String base(app_config::assistantBaseUrl);
  while (base.endsWith("/")) base.remove(base.length() - 1);
  return base + path;
}

int send3CCommand(const String& rawCommand) {
  String command = rawCommand;
  command.trim();
  if (command.isEmpty()) {
    lastBackendMessage = "Comando vacio";
    return 400;
  }
  if (WiFi.status() != WL_CONNECTED) {
    lastBackendMessage = "Wi-Fi desconectado";
    drawFace("offline"); setColor(0, 0, 40);
    return 503;
  }

  HTTPClient http;
  http.setTimeout(app_config::httpTimeoutMs);
  http.begin(endpoint("/api/device/v1/commands"));
  http.addHeader("Content-Type", "application/json");
  if (strlen(app_config::apiToken)) http.addHeader("X-3C-Device-Token", app_config::apiToken);

  const String requestId = String(app_config::deviceId) + "-" + String(millis());
  const String body = "{\"device_id\":\"" + jsonEscape(app_config::deviceId) +
    "\",\"request_id\":\"" + jsonEscape(requestId) +
    "\",\"text\":\"" + jsonEscape(command) + "\"}";
  const int code = http.POST(body);
  lastBackendMessage = code > 0 ? http.getString() : http.errorToString(code);
  http.end();

  if (code == 200 || code == 202) {
    backendAvailable = true;
    drawFace("pending"); setColor(45, 25, 0);
  } else {
    backendAvailable = false;
    drawFace("error"); setColor(60, 0, 0);
  }
  Serial.printf("POST 3C -> %d %s\n", code, lastBackendMessage.c_str());
  return code;
}

bool checkBackendHealth() {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  http.setTimeout(app_config::httpTimeoutMs);
  http.begin(endpoint("/api/device/v1/health"));
  const int code = http.GET();
  lastBackendMessage = code > 0 ? http.getString() : http.errorToString(code);
  http.end();
  backendAvailable = code == 200;
  if (backendAvailable) { drawFace("ok"); setColor(0, 24, 0); }
  else { drawFace("error"); setColor(45, 0, 0); }
  Serial.printf("GET health -> %d %s\n", code, lastBackendMessage.c_str());
  return backendAvailable;
}

const char controlPage[] PROGMEM = R"HTML(
<!doctype html><html lang="es"><meta name="viewport" content="width=device-width,initial-scale=1">
<style>body{font-family:system-ui;max-width:680px;margin:auto;padding:24px;background:#f4f6f8}section{background:#fff;padding:18px;border-radius:14px;margin-bottom:16px;box-shadow:0 4px 18px #0001}button,textarea{font:inherit}button{padding:12px 16px;margin:4px;border:0;border-radius:9px;background:#1769e0;color:#fff}textarea{box-sizing:border-box;width:100%;min-height:96px;padding:10px}.warn{color:#805500}</style>
<h1>ESP-Hi 3C</h1><section><h2>Movimiento</h2><button onclick="move('forward')">Avanzar</button><button onclick="move('backward')">Retroceder</button><button onclick="move('left')">Izquierda</button><button onclick="move('right')">Derecha</button><button onclick="move('sit')">Sentarse</button><button onclick="move('shake')">Dar pata</button><button onclick="move('stop')">Parar</button></section>
<section><h2>Orden 3C</h2><p class="warn">Se enviara como vista previa. La hoja solo cambia despues de confirmar en Asistente 3C.</p><textarea id="text" placeholder="Cambia la tarea J10 a mensual"></textarea><button onclick="send3c()">Enviar al asistente</button><pre id="result"></pre></section>
<script>async function move(a){result.textContent=await(await fetch('/api/move?action='+a,{method:'POST'})).text()}async function send3c(){const b=new URLSearchParams({text:document.querySelector('#text').value});const r=await fetch('/api/3c',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:b});result.textContent=r.status+' '+await r.text()}</script></html>
)HTML";

void configureWebServer() {
  web.on("/", HTTP_GET, [] { web.send_P(200, "text/html; charset=utf-8", controlPage); });
  web.on("/health", HTTP_GET, [] {
    const String body = String("{\"ok\":true,\"wifi\":") + (WiFi.status() == WL_CONNECTED ? "true" : "false") +
      ",\"backend\":" + (backendAvailable ? "true" : "false") + ",\"ip\":\"" + WiFi.localIP().toString() + "\"}";
    web.send(200, "application/json", body);
  });
  web.on("/api/move", HTTP_POST, [] {
    const String action = web.arg("action");
    const bool valid = runMovement(action);
    web.send(valid ? 200 : 400, "application/json", valid ? "{\"ok\":true}" : "{\"error\":\"accion invalida\"}");
  });
  web.on("/api/3c", HTTP_POST, [] {
    const int code = send3CCommand(web.arg("text"));
    web.send(code == 200 || code == 202 ? 202 : 502, "application/json", lastBackendMessage);
  });
  web.onNotFound([] { web.send(404, "application/json", "{\"error\":\"not found\"}"); });
  web.begin();
}

void connectWifi() {
  if (!strlen(app_config::wifiSsid)) {
    Serial.println("Configure include/local_config.h antes de usar Wi-Fi.");
    drawFace("offline"); setColor(0, 0, 30);
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(app_config::deviceId);
  WiFi.begin(app_config::wifiSsid, app_config::wifiPassword);
  lastWifiAttempt = millis();
  Serial.printf("Conectando a %s", app_config::wifiSsid);
}

void handleButtons() {
  if (healthButton.pressed()) checkBackendHealth();
  if (commandButton.pressed()) send3CCommand(app_config::defaultCommand);
  if (moveButton.pressed()) {
    static const char* actions[] = {"forward", "left", "right", "sit", "shake", "stop"};
    runMovement(actions[movementIndex]);
    movementIndex = (movementIndex + 1) % 6;
  }
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(250);
  pinMode(pins::bootButton, INPUT_PULLUP);
  pinMode(pins::moveButton, INPUT_PULLUP);
  pinMode(pins::commandButton, INPUT_PULLUP);

  leds.begin(); leds.setBrightness(48); leds.clear(); leds.show();
  SPI.begin(pins::displayClock, -1, pins::displayMosi, pins::displayCs);
  display.init(80, 160, SPI_MODE0);
  display.setRotation(1);
  drawFace("offline");

  for (int index = 0; index < 4; ++index) {
    servos[index].setPeriodHertz(50);
    servos[index].attach(servoPins[index], 500, 2400);
  }
  stand();
  connectWifi();
  configureWebServer();
}

void loop() {
  web.handleClient();
  handleButtons();

  if (WiFi.status() == WL_CONNECTED) {
    static bool announced = false;
    if (!announced) {
      announced = true;
      Serial.printf("\nWi-Fi listo: http://%s/\n", WiFi.localIP().toString().c_str());
      if (MDNS.begin("esp-hi-3c")) MDNS.addService("http", "tcp", 80);
      checkBackendHealth();
    }
    if (millis() - lastHealthCheck >= app_config::healthCheckMs) {
      lastHealthCheck = millis();
      checkBackendHealth();
    }
  } else if (strlen(app_config::wifiSsid) && millis() - lastWifiAttempt >= app_config::wifiRetryMs) {
    lastWifiAttempt = millis();
    WiFi.disconnect();
    WiFi.begin(app_config::wifiSsid, app_config::wifiPassword);
  }
  delay(5);
}

#endif  // BOARD_ESP_HI_C3

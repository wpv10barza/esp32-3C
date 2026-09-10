# ESP32 3C V2

Firmware para usar un ESP32 como terminal del **Asistente 3C** que se ejecuta
en WSL. El repositorio conserva dos objetivos de hardware independientes:

| Entorno PlatformIO | Hardware | Interaccion |
|---|---|---|
| `esp_hi_3c` | Perro ESP-Hi con ESP32-C3 | pantalla 160x80, LED, botones y cuatro servos |
| `panel_4848s040` | Panel ESP32-4848S040 con ESP32-S3-N16R8 | pantalla tactil 480x480, ojos, audio y botones tactiles |

No cargue el binario C3 en el panel S3 ni el binario S3 en el perro. GitHub
Actions compila ambos por separado y publica los dos grupos de binarios.

## Checklist de cierre — Ajuste de Placa

El cierre de esta tarea queda documentado en
[`docs/CHECKLIST_CIERRE_AJUSTE_PLACA.md`](docs/CHECKLIST_CIERRE_AJUSTE_PLACA.md).
El documento registra la separación de objetivos, configuración de pines,
identificación de dispositivo, manejo de credenciales locales, condición de
audio/relés y criterio de aceptación para la validación en campo.

**Estado:** ✅ Cerrado para los objetivos de hardware actualmente soportados por
`main`.

## Panel ESP32-4848S040 de las fotografias

La implementacion usa el hardware documentado para esta placa:

- ESP32-S3, flash QIO de 16 MB y PSRAM OPI de 8 MB;
- LCD ST7701 de 480x480 con bus RGB de 16 bits;
- control del LCD por SPI: CS 39, SCK 48 y MOSI 47;
- retroiluminacion en GPIO 38;
- tactil GT911 en I2C `0x5D`: SDA 19 y SCL 45;
- amplificador de altavoz por I2S: BCLK 1, LRCLK 2 y DATA 40.

Los GPIO 1, 2 y 40 tambien se usan para los reles en otras variantes de esta
familia. Si su unidad tiene reles en lugar del circuito de audio, configure
`PANEL_AUDIO_ENABLED_VALUE 0` antes de cargarla.

En la pantalla se puede tocar **PROBAR WSL** o **ENVIAR 3C**. La segunda accion
envia el comando predeterminado y muestra el ciclo completo: pendiente,
aplicado o cancelado. Para escribir otra orden, abra
`http://esp32-panel-3c.local/` desde un equipo de la misma red.

El panel fotografiado tiene salida de altavoz, pero no aparece un microfono en
su esquema. Por eso esta V2 admite tactil y texto web; una conversacion de voz
completa requiere agregar un microfono I2S compatible.

## Seguridad del flujo 3C

El dispositivo llama a:

- `GET /api/device/v1/health`;
- `POST /api/device/v1/commands`;
- `GET /api/device/v1/commands/{command_id}`.

La orden se autentica con `X-3C-Device-Token`, usa un `request_id` idempotente
y queda en `pending_confirmation`. El ESP32 nunca escribe directamente en
Google Sheets. La persona debe revisar la vista previa y pulsar **Confirmar y
aplicar** en `asistente-3c`; el panel consulta el resultado y actualiza su cara.

## 1. Instalar en WSL

Mantenga los repositorios como carpetas independientes en `~/projects`. No los
coloque dentro de `.venv` ni dentro de `sap-pm-rag-parser-production`:

```bash
cd ~/projects
git clone https://github.com/wpv10barza/asistente-3c.git
git clone https://github.com/wpv10barza/esp32-3C.git

cd ~/projects/asistente-3c
cp -n .env.example .env
nano .env
npm ci
npm run build
npm start
```

En `.env`, asigne un valor largo y aleatorio a `ESP32_API_TOKEN`. El servidor
escucha en `0.0.0.0:3000`. Desde el ESP32 use la IPv4 LAN de Windows; no use
`127.0.0.1` ni `localhost`.

Compruebe la ruta desde otro equipo conectado al mismo Wi-Fi:

```bash
curl http://IP_LAN_DE_WINDOWS:3000/api/device/v1/health
```

## 2. Configurar el ESP32 sin publicar secretos

```bash
cd ~/projects/esp32-3C
cp -n include/local_config.example.h include/local_config.h
nano include/local_config.h
```

Use el mismo token del backend y complete:

```cpp
#define WIFI_SSID_VALUE "TU_WIFI_2_4_GHZ"
#define WIFI_PASSWORD_VALUE "TU_CLAVE"
#define ASSISTANT_BASE_URL_VALUE "http://IP_LAN_DE_WINDOWS:3000"
#define ESP32_API_TOKEN_VALUE "EL_MISMO_TOKEN_LARGO"
#define DEVICE_ID_VALUE "panel-4848s040-3c-01"
#define PANEL_AUDIO_ENABLED_VALUE 1
```

`include/local_config.h` esta ignorado por Git y un futuro `git pull` no
reemplaza sus secretos.

## 3. Compilar y cargar el panel

```bash
python3 -m pip install platformio==6.1.18
pio run --environment panel_4848s040
pio run --environment panel_4848s040 --target upload
pio device monitor --baud 115200
```

Para el perro ESP-Hi C3 use `--environment esp_hi_3c`. Un `pio run` sin
entorno compila ambos objetivos y detecta incompatibilidades de los dos.

El USB-C del panel pasa por un CH340 hacia UART0. La configuracion del objetivo
S3 usa flash QIO 16 MB y PSRAM OPI. No active USB CDC al inicio para este panel.

> Advertencia: algunas versiones incluyen una placa trasera con reles para
> tension de red. Desconecte completamente esa placa y la red electrica antes
> de abrir, programar o manipular el panel. El USB no aisla contactos de red.

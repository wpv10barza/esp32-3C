# ESP-Hi 3C

Firmware V2 para el perro mecánico **ESP-Hi con ESP32-C3** conectado al
Asistente 3C que se ejecuta en WSL. Usa el pinout oficial del ESP-Hi:

- pantalla ST7789 160 × 80: MOSI 4, CLK 5 y DC 10;
- cuatro WS2812: GPIO 8;
- servos: delantero izquierdo 21, delantero derecho 19, trasero izquierdo 20
  y trasero derecho 18;
- botones: BOOT 9, movimiento 0 y orden 3C 1.

## Funciones V2

- Conexión Wi-Fi de 2,4 GHz y comprobación periódica del endpoint WSL.
- Ojos/estado en la pantalla: desconectado, disponible, pendiente o error.
- LED de estado, cuatro servos y movimientos básicos: avanzar, retroceder,
  girar, sentarse, dar la pata y detenerse.
- Control web local en `http://esp-hi-3c.local/`.
- Envío de texto a `POST /api/device/v1/commands` con token e idempotencia.
- Seguridad 3C: el dispositivo solo crea una vista previa; la escritura exige
  confirmación humana en `asistente-3c`.

La entrada de micrófono/TTS completa de XiaoZhi no forma parte de esta primera
integración 3C. El botón GPIO 1 envía el comando predeterminado y la página web
local permite escribir cualquier orden. Así se mantiene el ESP32 como terminal
de campo y el backend Linux como autoridad sobre las reglas y Google Sheets.

## 1. Preparar WSL

Coloque los repositorios como carpetas independientes dentro de `~/projects`;
no los copie dentro de `.venv` ni dentro de `sap-pm-rag-parser-production`:

```bash
cd ~/projects
git clone https://github.com/wpv10barza/asistente-3c.git
git clone https://github.com/wpv10barza/esp32-3C.git
cd ~/projects/asistente-3c
cp .env.example .env
nano .env
npm ci
npm run build
npm start
```

En `.env`, `ESP32_API_TOKEN` debe ser un valor largo y aleatorio. El servidor
escucha en `0.0.0.0:3000`. Desde el ESP32 se usa la IPv4 LAN de Windows, no
`localhost` ni `127.0.0.1`. En WSL2 habilite red reflejada o el reenvío del
puerto TCP 3000 y permita ese puerto solo para la red privada de Windows.

Compruebe desde otro equipo conectado al mismo Wi-Fi:

```bash
curl http://IP_LAN_DE_WINDOWS:3000/api/device/v1/health
```

## 2. Configurar el firmware sin publicar secretos

```bash
cd ~/projects/esp32-3C
cp include/local_config.example.h include/local_config.h
nano include/local_config.h
```

Configure el mismo `ESP32_API_TOKEN_VALUE`, su Wi-Fi 2,4 GHz y la URL, por
ejemplo `http://192.168.1.50:3000`. `include/local_config.h` está ignorado por
Git, por lo que un futuro `git pull` conserva sus secretos locales.

## 3. Compilar y cargar

Con PlatformIO:

```bash
python3 -m pip install platformio==6.1.18
pio run
pio run --target upload
pio device monitor --baud 115200
```

Antes de grabar, desconecte la cabeza del cuerpo: en el ESP-Hi real, el control
de servos comparte recursos con el USB-C. Alimente los cuatro servos desde la
placa/base prevista por el fabricante; no desde un pin GPIO. La postura inicial
usa un rango conservador de 45° a 135°, pero debe probarse con el cuerpo elevado
antes de apoyar el robot.

GitHub Actions compila el firmware para `esp32-c3-devkitm-1` en cada `push` y
publica `firmware.bin`, `bootloader.bin` y `partitions.bin` como artefacto.


# Checklist de cierre — Ajuste de Placa

**Repositorio:** `wpv10barza/esp32-3C`  
**Componente:** Código base del terminal móvil de operaciones  
**Estado:** ✅ Cerrado documental y técnicamente sobre los objetivos actualmente soportados por el repositorio.

## Alcance del cierre

Se deja trazabilidad del ajuste de placa para evitar mezclar firmwares entre los dos objetivos PlatformIO existentes:

| Objetivo | MCU / hardware | Resolución / periféricos principales | Identificador por defecto | Estado |
|---|---|---|---|---|
| `esp_hi_3c` | ESP32-C3 / ESP-Hi | Pantalla 160×80, LED, botones y servos | `esp-hi-3c-01` | ✅ |
| `panel_4848s040` | ESP32-S3 / ESP32-4848S040 | Pantalla táctil 480×480, audio y botones táctiles | `panel-4848s040-3c-01` | ✅ |

## Verificaciones realizadas sobre `main`

- [x] `platformio.ini` mantiene **entornos independientes** para C3 y 4848S040.
- [x] El firmware del panel queda condicionado por `BOARD_PANEL_4848S040`.
- [x] El panel usa la configuración de memoria documentada para 16 MB QIO + PSRAM OPI.
- [x] Los GPIO del panel permanecen centralizados en `src/panel_4848s040_main.cpp`.
- [x] Se mantiene `include/local_config.h` fuera del control de versiones para SSID, contraseña y token.
- [x] `DEVICE_ID_VALUE` distingue automáticamente los dos objetivos.
- [x] `PANEL_AUDIO_ENABLED_VALUE` permite separar la variante de audio de la variante con relés que comparte GPIO 1/2/40.
- [x] El flujo de comandos conserva autenticación mediante `X-3C-Device-Token` y estado `pending_confirmation` antes de una aplicación aprobada.
- [x] GitHub Actions compila ambos objetivos y publica sus binarios como artefactos.

## Criterio de aceptación

El ajuste se considera cerrado cuando se cumple lo siguiente:

1. Se selecciona el entorno PlatformIO correspondiente a la placa real.
2. No se intercambia el binario `esp_hi_3c` con `panel_4848s040`.
3. Las credenciales locales se cargan únicamente desde `include/local_config.h`.
4. El panel responde al arranque, pantalla, conectividad y flujo de prueba 3C.
5. La variante física con relés no se programa con audio habilitado si los GPIO 1/2/40 están ocupados.
6. La compilación de CI termina correctamente para ambos objetivos.

## Procedimiento de cierre en campo

```bash
cd ~/projects/esp32-3C
pio run --environment esp_hi_3c
pio run --environment panel_4848s040
```

Para el panel táctil:

```bash
pio run --environment panel_4848s040 --target upload
pio device monitor --baud 115200
```

Para una unidad ESP-Hi C3:

```bash
pio run --environment esp_hi_3c --target upload
pio device monitor --baud 115200
```

## Nota de compatibilidad

Este cierre documenta y estabiliza **los dos objetivos que ya existen en `main`**. No declara compatibilidad automática con otras variantes físicas del ESP32-S3. Una placa distinta debe incorporarse como un objetivo PlatformIO independiente, con su propio mapeo de pines, memoria, periféricos y `DEVICE_ID`, antes de reutilizar el flujo de este repositorio.

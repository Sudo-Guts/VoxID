# Animacion pastel de voz — VoxID

## Objetivo

Recrear los **tres circulos concentricos** del concepto visual original en
la pantalla ST7735 de Ophyra (160 x 128), utilizando el fondo claro del
proyecto: marfil lavanda, anillo exterior tenue, anillo intermedio violeta
pastel y anillo interior turquesa suave.

## Modulos

- `Core/Inc/ui_voice.h`: API `ui_voice_draw(phase, background)`.
- `Core/Src/ui_voice.c`: tres anillos dibujados con `gfx_draw_circle()`.
- `Core/Src/ui.c`: actualiza una fase cada 180 ms desde `ui_task()`,
  utilizando `HAL_GetTick()` sin bloquear el firmware.

La animacion redibuja solamente el area **64 x 56 pixeles**, preservando
la cabecera, el titulo y el footer con sus botones fisicos.

## Comportamiento actual

- Abrir **IDENTIFICAR** o **REGISTRAR**, luego pulsar OK para entrar
  a la pantalla de captura ADC: los circulos laten en colores pastel.
- Pulsar OK para la prueba de hardware ADC1 + TIM2 de **720 muestras**.
- Al terminar, aparecen los valores **MIN / MAX / PROMEDIO** reales del ADC.
- Si hay error, el indicador permanece y aparece **ERROR ADC/TIM2**.
- BACK regresa a la pantalla anterior, deteniendo la prueba ADC.

**Importante:** la captura actual dura solo 45 ms. El pulso visual
en reposo es una animacion de interfaz; no significa que el microfono
este grabando continuamente ni que este reconociendo a un hablante.
No se han implementado aun los 3 segundos de identificacion, las
tres grabaciones de 5 segundos ni el procesamiento de audio real.

## Reutilizacion futura

Cuando exista ADC DMA y la maquina de estados de identificacion, reutilizar
el mismo modulo grafico en los estados **ESCUCHANDO** y **ANALIZANDO**, y
detenerlo cuando se muestre el resultado.

## Plan de almacenamiento EEPROM (no implementado)

- Recolectar tres grabaciones de hasta 5 segundos por locutor.
- Extraer en streaming 13 MFCC, descartar frames sin voz y acumular
  estadisticas por grabacion: media y varianza por coeficiente.
- Cada grabacion produce un prototipo de 26 floats (104 bytes).
- Tres prototipos ocupan 312 bytes por locutor sin metadatos.
- Definir una estructura versionada y protegida con CRC para guardar
  los perfiles en M24C32 (4096 bytes, I2C, paginas de 32 bytes).
- Guardar unicamente despues de que las tres grabaciones sean validas.
- Calibrar umbrales de identificacion con grabaciones de varias personas;
  no asumir reconocimiento fiable hasta validar las comparaciones.

Aun NO se ha configurado I2C ni escrito la EEPROM.

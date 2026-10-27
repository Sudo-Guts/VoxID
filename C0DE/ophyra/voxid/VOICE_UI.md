# Indicador de voz pastel — VoxID Ophyra

## Estados de la interfaz

El indicador conserva los tres circulos concentricos de color pastel
sobre el fondo claro de la pantalla ST7735 de 160 x 128 pixeles.

- **PREPARADO (antes de OK):** los circulos se animan suavemente.
  `ui_task()` cambia de fase cada 180 ms mediante `HAL_GetTick()`.
  Estan disponibles OK para iniciar y BACK para regresar.
- **ESCUCHANDO (3 segundos):** al pulsar OK, se fija una sola fase
  de los circulos. **No se animan ni se redibujan durante la captura**.
  Solo se actualiza la cuenta regresiva de 3 a 2 a 1 segundos.
  OK se oculta y se ignora; BACK permite cancelar la captura.
- **RESULTADO:** se limpian los circulos y aparece la tarjeta estatica
  `3 SEGUNDOS: OK` con MIN, MAX y PROMEDIO. OK permite repetir,
  y BACK regresa.
- **ERROR:** tarjeta estatica de error, sin circulos, con OK para
  reintentar y BACK para regresar.

## Consideraciones de implementacion

`Core/Src/ui_voice.c` dibuja las fases mediante `gfx_draw_circle()`.
El cambio de fases ocurre **unicamente cuando ADC esta en IDLE**
y la pantalla actual es CAPTURA. Durante el muestreo DMA se conserva
el dibujo fijo y no se realizan llamadas periodicas a `ui_voice_draw()`.

Al solicitar la captura, la UI dibuja el footer (solo BACK), una
fase fija de los circulos y el texto ESCUCHANDO antes de arrancar DMA,
para evitar redibujar la pantalla completa mientras se reciben bloques.

La captura sigue usando ADC1_IN2 (PA2), TIM2 TRGO a 16 kHz,
DMA circular con mitades de 640 muestras y duracion de 3 segundos
(48000 muestras). **No se modifican ADC, DMA, MFCC ni EEPROM**.
Aun no se procesan MFCC de audio real ni se guardan perfiles.

## Verificacion en Ophyra

1. Entrar en IDENTIFICAR o REGISTRAR -> OK. Ver los circulos animados.
2. Pulsar OK para empezar: los circulos quedan estaticos mientras
   desciende la cuenta regresiva y solo BACK esta habilitado.
3. Al acabar los tres segundos: desaparecen los circulos y aparece
   la tarjeta de resultados, sin superposiciones ni parpadeos.
4. Pulsar OK para repetir o BACK para regresar.
5. Confirmar que DIAGNOSTICO -> prueba sintetica MFCC se mantiene igual.

Pendiente compilar y comprobar esta version en la tarjeta fisica.

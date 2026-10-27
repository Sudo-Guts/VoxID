# Adquisicion de tres segundos con ADC1 + TIM2 + DMA

## Objetivo actual

Capturar exactamente **48000 conversiones ADC** a 16 kHz
(3.000 segundos), sin guardar el audio completo en RAM ni
implementar todavia reconocimiento, registro o EEPROM.

## Configuracion STM32F407 / Ophyra

- Entrada: **PA2 = ADC1_IN2**, 12 bits, muestras crudas de 0 a 4095.
- TIM2: reloj 84 MHz, PSC 0, ARR 5249, TRGO por evento UPDATE.
- ADC1: disparo externo ascendente TIM2 TRGO, conversion no continua.
- DMA: **DMA2 Stream0 Channel0**, periferico a memoria, halfword,
  memoria incremental, circular, interrupcion de prioridad 5.
- `ADC1.DMAContinuousRequests = ENABLE` habilita solicitudes sucesivas.
- `MX_DMA_Init` se invoca antes de `MX_ADC1_Init`; la HAL une
  los handles mediante `__HAL_LINKDMA`.
- El archivo `voxid.ioc` se actualizo para conservar DMA en CubeMX.

## Control del buffer

- Buffer: 1280 muestras de 16 bits (**2560 bytes**), dos mitades
  de 640 muestras.
- Cada mitad dura 40 ms. La ISR solo incrementa `produced_blocks`.
- `voxid_audio_task` se ejecuta desde el bucle principal y
  acumula MIN, MAX y PROMEDIO; no se procesan muestras dentro de IRQ.
- Despues de 75 callbacks (48000 muestras), la ISR detiene TIM2.
- El bucle principal consume la ultima mitad y detiene HAL ADC DMA.
- Si hay dos o mas mitades pendientes antes de consumir la anterior,
  se declara **ERROR** en vez de utilizar datos posiblemente sobrescritos.

## Interfaz de los circulos

Entrar en **IDENTIFICAR** o **REGISTRAR** y pulsar OK.
En la pantalla `MARIPOSA / PA2`, OK inicia los tres segundos
de escucha. Aparecen los tres circulos pastel animados y
la cuenta regresiva **ESCUCHANDO: 3s, 2s, 1s**.

Al terminar correctamente, la UI muestra `3 SEGUNDOS: OK`
y MIN, MAX, PROMEDIO. OK repite y BACK cancela/retorna.
La pantalla se dibuja antes de habilitar DMA para evitar
bloquear el primer semibuffer durante un refresco de pantalla.

## Etapa 2

Ahora los bloques DMA tambien se convierten a PCM con signo y se
procesan mediante MFCC. Consultar `STAGE2_REAL_MFCC.md` para
el pipeline, las verificaciones de 298 frames y la nueva grafica
MFCC real. La validacion en hardware sigue pendiente.

## Validacion fisica pendiente

1. Clean/Build en STM32CubeIDE; comprobar que se reconoce el modulo DMA.
2. Programar Ophyra y verificar que el menu MFCC sintetico sigue funcionando.
3. Conectar a PA2 **solo una tension segura de 0 a 3.3 V** con
   tierra comun, idealmente 1.65 V continuos como prueba inicial.
4. Capturar 3 s, comprobar promedio aproximadamente 2048 (para VDDA=3.3 V).
5. Repetir y comprobar ausencia de errores DMA y fluidez de circulos.
6. Si se muestra ERROR DMA, revisar latencia del bucle principal
   y tiempos de dibujado antes de intentar almacenar perfiles.

Ahora se convierte ADC a PCM y se calculan MFCC reales.
No se valida la palabra MARIPOSA, ni hay perfiles EEPROM o clasificador.

## Riesgo de firmware generado

La configuracion `.ioc` y el codigo C se actualizaron juntos.
Si CubeMX regenera los archivos, verificar que conserve
`DMA2_Stream0_IRQHandler`, vinculacion con ADC y
`MX_DMA_Init` **antes** de `MX_ADC1_Init`.

# Etapa ADC1 + TIM2 — Primera adquisicion finita de VoxID

## Objetivo

Validar el camino de **audio analogico -> PA2/ADC1_IN2 -> muestras de 12 bits** en la
Ophyra STM32F407VGT6, sin alterar el pipeline MFCC ya validado. Se emplea
un bloque de 720 muestras por captura y **ADC EOC interrupt** (una interrupcion
por muestra) solo para las pruebas iniciales. Para adquisicion continua se
migrara a **DMA circular/doble buffer**, sin procesar MFCC en interrupciones.

## Perifericos existentes y frecuencia

El archivo `voxid.ioc` y la inicializacion CubeMX ya configuraban:

- ADC1, canal regular 2 en **PA2**, entrada analogica.
- Resolucion ADC 12 bits; alineacion derecha; 84 ciclos de muestreo.
- Disparo externo por **TIM2 TRGO UPDATE**, flanco ascendente.
- TIM2 PSC = 0, ARR = 5249. TIM2_CLK = 84 MHz (PCLK1 = 42 MHz, APB1 = /4).
- Tasa = 84 MHz / (1 * 5250) = **16,000 muestras/segundo**.
- ADC interrumpe por conversion; `ADC_IRQHandler` ya delega al HAL.
- El codigo anterior inicializaba los perifericos, pero nunca arrancaba el timer.

Por eso **no es necesario editar el archivo generado `main.c` ni `voxid.ioc`**
para esta primera etapa. Los callbacks `HAL_ADC_ConvCpltCallback` y
`HAL_ADC_ErrorCallback` estan implementados en `voxid_audio.c`.

## Probar desde la UI

1. Entrar en **IDENTIFICAR** o **REGISTRAR** y seleccionar **OK** para entrar
   en **CAPTURA ADC1**. En REGISTRAR se conserva el numero de locutor como
   seleccion de interfaz, **sin crear ni guardar su perfil**.
2. Conectar la salida analogica ya acondicionada a **PA2** y compartir GND.
   El voltaje nunca debe caer fuera de **0 a 3.3 V**.
3. En CAPTURA ADC1, presionar **OK**. El programa arma ADC1 con
   `HAL_ADC_Start_IT`, y luego comienza TIM2 con `HAL_TIM_Base_Start`.
4. El callback guarda una muestra de 12 bits por interrupcion. A las
   **720 conversiones (45 ms)** detiene TIM2 y deja los datos en RAM.
5. La interfaz presenta **MUESTRAS: OK**, **MIN**, **MAX** y **PROMEDIO**,
   todos en unidades crudas ADC (0...4095). OK permite repetir y BACK
   retorna al recorrido anterior.
6. Una prueba sencilla es aplicar aproximadamente **1.65 V de DC**:
   un ADC con Vref = 3.3 V debe indicar un promedio cercano a **2048**
   (aproximado, depende de la referencia real, errores y ruido).

## Limites y siguientes etapas

- La prueba verifica adquisicion **electrica**, no identifica ni registra
  locutores, no convierte ADC a PCM con signo ni ejecuta MFCC real.
- No hay transferencia DMA; la interrupcion por muestra **no es la
  arquitectura final** para captura continua de voz.
- La prueba sintetica MFCC y su grafica no se modifican.
- El **test 440 Hz** sigue recibiendo muestras digitales sinteticas, no del ADC.
- En la siguiente etapa se implementaran **DMA circular y procesamiento en
  el bucle principal** con bloques/frames y control de desbordamiento.
- No aplicar una salida de amplificador alimentado a voltajes superiores
  directamente al pin: acondicionar offset, ganancia y proteccion antes
  de conectarlo a 3.3 V.

## Validacion pendiente

En STM32CubeIDE: refrescar, Clean/Build, programar, probar las 4 teclas y
comprobar la captura con voltaje DC controlado. No se ha realizado una
compilacion ARM ni una prueba fisica automatizada para esta nueva rama.

# VoxID

Sistema embebido para identificar hablantes a partir de la palabra de referencia **MARIPOSA**.

La plataforma digital es **Ophyra con STM32F407VGT6**, pantalla ST7735 de 160 × 128 píxeles y cuatro pulsadores. El firmware principal está en `C0DE/ophyra/voxid`; el núcleo DSP compartido está en `C0DE/src` y `C0DE/inc`.

## Etapas y estado

| Etapa | Alcance | Estado |
|---|---|---|
| 1 | ADC1 + TIM2 + DMA y círculos de preparación | Implementada; pendiente de validación física de la versión actual. |
| 2 | ADC a PCM y extracción MFCC de audio real | Implementada y probada en PC con señales sintéticas y HAL simulada; pendiente de compilación ARM y validación en Ophyra. |
| 3 | Registro de tres prototipos MFCC por locutor | Implementada en RAM y probada en PC; pendiente de validación física del registro. |
| 4 | EEPROM M24C32 e identificación por distancia normalizada | Implementada y probada en PC; pendientes compilación ARM, validación en Ophyra y calibración. |

La interfaz permite registrar cuatro locutores, con tres prototipos de 13 coeficientes por locutor. Los perfiles se guardan en EEPROM y se recuperan en RAM al arrancar. La identificación compara una toma nueva con los tres prototipos de cada locutor y puede rechazar por distancia o ambigüedad. Los umbrales iniciales requieren calibración; el código no verifica la palabra pronunciada.

## Adquisición y procesamiento

La cadena prevista comprende micrófono electret, preamplificación, filtros pasaaltas y pasabajas, adaptación de nivel al ADC y procesamiento digital.

| Parámetro digital | Configuración |
|---|---|
| Entrada | PA2, ADC1_IN2, 12 bits |
| Muestreo | 16 kHz, TIM2 TRGO; reloj TIM2 de 84 MHz, PSC 0 y ARR 5249 |
| Captura | 3 segundos, 48 000 muestras |
| DMA | DMA2 Stream0 Channel0, circular; dos mitades de 640 muestras |
| Tiempo entre mitades | 40 ms |
| Frame y avance | 400 muestras, 25 ms; avance de 160 muestras, 10 ms |
| Extracción | Hamming, FFT de 512, potencia, 26 filtros Mel, logaritmo y DCT-II |
| Resultado | 298 vectores de 13 coeficientes por toma |

`voxid_config.h` concentra los parámetros compartidos. La ISR notifica bloques; el bucle principal convierte ADC a PCM, elimina DC y procesa MFCC. Se usan buffers estáticos y no se guarda el audio completo.

La media MFCC utiliza únicamente frames cuya energía supera `VOXID_VOICE_MIN_RMS`. El umbral inicial es RMS 0.01 respecto de la escala completa y necesita calibración con voz y ruido medidos en la placa. Un ruido suficientemente intenso también puede superar este criterio. Si no se selecciona ningún frame, la captura puede ser válida, pero la UI muestra `CON VOZ: 0` y no dibuja una media de voz.

## Registro de locutores

REGISTRAR permite elegir un locutor y capturar tres tomas independientes. OK acepta cada toma válida y empieza la siguiente; en la tercera confirma el guardado verificado en EEPROM. BACK descarta el registro pendiente y conserva el perfil anterior si existía.

El mínimo inicial es de 20 frames seleccionados por toma, configurable en `voxid_config.h` y pendiente de calibración física. Una toma insuficiente o un error se repite sin avanzar. Consultar [el flujo de etapa 3](C0DE/ophyra/voxid/STAGE3_ENROLLMENT.md) y [la persistencia e identificación de etapa 4](C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md).

## Identificación y EEPROM

I2C1 utiliza PB6/PB7 a 100 kHz para la M24C32 integrada. Cada locutor dispone de dos copias con versión y CRC: el reemplazo conserva la copia vigente hasta verificar la nueva. El arranque lee los perfiles sin formatear ni escribir. Un fallo de guardado permite reintentar la tercera toma sin volver a capturarla.

La distancia es el promedio de las tres distancias euclidianas entre vectores C01..C12 normalizados a norma L2 unitaria. Los valores iniciales son distancia máxima 0.35 y margen mínimo 0.05 respecto al segundo candidato. La pantalla muestra LOCUTOR nn, DESCONOCIDO, VOZ AMBIGUA, SIN LOCUTORES o VOZ INSUFICIENTE. UP/DN conserva el acceso a la gráfica MFCC y a las estadísticas ADC.

Los parámetros y la revisión DSP están en `voxid_config.h`; el [formato y las comprobaciones](C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md) describen sus límites y la calibración pendiente.

## Organización

| Ruta | Responsabilidad |
|---|---|
| `C0DE/inc`, `C0DE/src` | Algoritmos DSP portables en C. |
| `C0DE/tests` | Pruebas numéricas y regresiones de voz, adquisición e interfaz. |
| `C0DE/tests/support` | HAL simulada, exclusiva para PC. |
| `C0DE/ophyra/voxid` | Firmware principal de Ophyra. |
| `C0DE/ophyra/voxid_test` | Benchmark sintético histórico; conserva la biblioteca INTESC que utiliza. |
| `C0DE/out` | Resultados locales de compilación, excluidos de Git salvo `.gitkeep`. |
| `DOC` | Seguimiento y validación. |
| `PCB`, `SIM` | Diseño y simulaciones analógicas; ver la nota de ramas. |

## Compilar y probar en PC

Se necesitan GCC, GNU Make y objdump. Desde la raíz del repositorio:

```bash
make -C C0DE all
make -C C0DE test
make -C C0DE test test_voice.c
make -C C0DE clean
```

En Windows con MinGW, usar `mingw32-make` en lugar de `make`. La ruta de ejecución y la limpieza tienen reglas para ambos sistemas; la verificación automatizada de esta revisión se realizó en Linux.

`all` comprueba la compilación y el enlace del núcleo portable y genera un ejecutable mínimo y su desensamblado. `test` ejecuta once pruebas: Frame, ventana, FFT, MFCC, voz, adquisición, interfaz, GFX, registro, persistencia e identificación. Las pruebas con HAL de PC no sustituyen las mediciones en placa.

## Compilar el firmware

1. Mantener el checkout completo del repositorio: el proyecto enlaza los ocho módulos DSP compartidos mediante rutas relativas.
2. Importar `C0DE/ophyra/voxid` en STM32CubeIDE.
3. Refrescar con F5, ejecutar Clean y Build Debug, y programar Ophyra.
4. Seguir [la validación de etapas 1 y 2](DOC/Validacion_etapas_1_2.md), [el registro de etapa 3](C0DE/ophyra/voxid/STAGE3_ENROLLMENT.md) y [la validación de etapa 4](C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md).

El Makefile de PC no genera firmware ARM. Si CubeMX regenera código, comprobar la configuración DMA del `.ioc`, la vinculación con ADC1 y el orden GPIO, DMA, ADC1, SPI1, TIM2, I2C1 y los GPIO PB6/PB7 de I2C1.

## Ramas y hardware

La adquisición y los MFCC reales se desarrollaron en `feature/adc-tim2-acquisition`. La revisión de mantenimiento parte de esa rama.

El diseño analógico más reciente está en [la rama pcb-sim](https://github.com/Sudo-Guts/VoxID/tree/pcb-sim). Esta rama de firmware todavía conserva una revisión anterior de `PCB` y `SIM`; no usarla como fuente de fabricación. La corrección de simulaciones se prepara sobre `pcb-sim` para conservar el diseño vigente.

Los resultados históricos de filtros usan valores distintos de algunas resistencias y capacitores comerciales de la PCB actual. Deben distinguirse de los nuevos experimentos con valores de placa. Las simulaciones, los Gerbers y los archivos KiCad no acreditan por sí solos mediciones físicas ni aprobación ERC/DRC.

Consultar [el seguimiento](DOC/CheckList.txt) para separar implementación, pruebas en PC y validación física.


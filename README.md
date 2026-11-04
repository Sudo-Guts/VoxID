# VoxID

Sistema embebido para identificar hablantes a partir de la palabra de referencia **MARIPOSA**.

La plataforma digital es **Ophyra con STM32F407VGT6**, pantalla ST7735 de 160 x 128 pixeles y cuatro pulsadores. El firmware principal esta en `C0DE/voxid`; los ocho modulos DSP estan dentro del mismo proyecto, en `C0DE/voxid/Core/Src` y `C0DE/voxid/Core/Inc`.

## Etapas y estado

| Etapa | Alcance | Estado |
|---|---|---|
| 1 | ADC1 + TIM2 + DMA y circulos de preparacion | Implementada; funcionamiento general reportado en Ophyra. Mediciones de temporizacion pendientes. |
| 2 | ADC a PCM y extraccion MFCC de audio real | Implementada y probada en PC; funcionamiento general reportado en Ophyra. Calibracion y mediciones pendientes. |
| 3 | Registro de tres prototipos MFCC por locutor | Implementada y probada en PC; funcionamiento general reportado en Ophyra. Ajustes con varias personas pendientes. |
| 4 | EEPROM M24C32 e identificacion por distancia normalizada | Implementada y probada en PC; funcionamiento general y navegacion de locutores confirmados por el usuario. Calibracion pendiente. |

La interfaz permite registrar cuatro locutores, con tres prototipos de 13 coeficientes por locutor. Los perfiles se guardan en EEPROM y se recuperan en RAM al arrancar. La identificacion compara una toma nueva con los tres prototipos de cada locutor y puede rechazar por distancia o ambiguedad. Los umbrales iniciales requieren calibracion; el codigo no verifica la palabra pronunciada.

## Adquisicion y procesamiento

La cadena prevista comprende microfono electret, preamplificacion, filtros pasaaltas y pasabajas, adaptacion de nivel al ADC y procesamiento digital.

| Parametro digital | Configuracion |
|---|---|
| Entrada | PA2, ADC1_IN2, 12 bits |
| Muestreo | 16 kHz, TIM2 TRGO; reloj TIM2 de 84 MHz, PSC 0 y ARR 5249 |
| Captura | 2500 ms, 40 000 muestras |
| DMA | DMA2 Stream0 Channel0, circular; dos mitades de 800 muestras |
| Tiempo entre mitades | 50 ms |
| Frame y avance | 400 muestras, 25 ms; avance de 160 muestras, 10 ms |
| Extraccion | Hamming, FFT de 512, potencia, 26 filtros Mel, logaritmo y DCT-II |
| Resultado | 248 vectores de 13 coeficientes por toma |

`voxid_config.h` concentra los parametros compartidos. La ISR notifica bloques; el bucle principal convierte ADC a PCM, elimina DC y procesa MFCC. Se usan buffers estaticos y no se guarda el audio completo.

La fase 2 de reorganizacion integra el adaptador MFCC en `voxid_voice.c` y declara sus funciones existentes en `voxid_voice.h`. Se retiran los dos archivos separados `voxid_dsp.c` y `voxid_dsp.h` del firmware principal, conservando las APIs y el procesamiento de tomas de 2.5 segundos.

La media MFCC utiliza unicamente frames cuya energia supera `VOXID_VOICE_MIN_RMS`. El umbral inicial es RMS 0.01 respecto de la escala completa y necesita calibracion con voz y ruido medidos en la placa. Un ruido suficientemente intenso tambien puede superar este criterio. Si no se selecciona ningun frame, la captura puede ser valida, pero la UI muestra `CON VOZ: 0` y no dibuja una media de voz.

## Registro de locutores

REGISTRAR permite elegir un locutor y capturar tres tomas independientes. OK acepta cada toma valida y empieza la siguiente; en la tercera confirma el guardado verificado en EEPROM. BACK descarta el registro pendiente y conserva el perfil anterior si existia.

El minimo inicial es de 20 frames seleccionados por toma, configurable en `voxid_config.h` y pendiente de calibracion fisica. Una toma insuficiente o un error se repite sin avanzar. Consultar [el flujo de etapa 3](DOC/STAGE3_ENROLLMENT.md) y [la persistencia e identificacion de etapa 4](DOC/STAGE4_STORAGE_IDENTIFY.md).

## Consulta de locutores

El menu ofrece IDENTIFICAR, REGISTRAR y LOCUTORES. En LOCUTORES, UP/DN selecciona uno de los cuatro espacios; se muestra su estado, cantidad de tomas y 13 MFCC por toma. OK abre las tomas de un perfil registrado. Dentro, UP/DN recorre TOMA 1/3, 2/3 y 3/3; BACK regresa al mismo locutor y otro BACK al menu. Los botones sin accion quedan sin texto y con el fondo inactivo de la paleta.

Cada toma tiene su propia grafica C01..C12, normalizada a norma L2 unitaria y con escala fija -1..1; C00 queda fuera de la comparacion. El identificador promedia las tres distancias, por lo que se conservan las tres referencias. La consulta usa RAM, sin capturar audio, reiniciar el DSP ni escribir EEPROM. La EEPROM acepta unicamente perfiles de 248 frames. Los perfiles anteriores de 3 segundos deben registrarse de nuevo con tomas de 2.5 segundos.

## Identificacion y EEPROM

I2C1 utiliza PB6/PB7 a 100 kHz para la M24C32 integrada. Cada locutor dispone de dos copias con version y CRC: el reemplazo conserva la copia vigente hasta verificar la nueva. El arranque lee los perfiles sin formatear ni escribir. Un fallo de guardado permite reintentar la tercera toma sin volver a capturarla.

La distancia es el promedio de las tres distancias euclidianas entre vectores C01..C12 normalizados a norma L2 unitaria. Los valores iniciales son distancia maxima 0.35 y margen minimo 0.05 respecto al segundo candidato. La pantalla muestra LOCUTOR nn, DESCONOCIDO, VOZ AMBIGUA, SIN LOCUTORES o VOZ INSUFICIENTE. UP/DN conserva el acceso a la grafica MFCC y a las estadisticas ADC.

Los parametros y la revision DSP estan en `voxid_config.h`; el [formato y las comprobaciones](DOC/STAGE4_STORAGE_IDENTIFY.md) describen sus limites y la calibracion pendiente.

## Organizacion

| Ruta | Responsabilidad |
|---|---|
| `C0DE/voxid` | Unico proyecto de codigo; incluye configuracion CubeIDE y Makefile de pruebas. |
| `C0DE/voxid/Core/Inc`, `C0DE/voxid/Core/Src` | Firmware y ocho modulos DSP integrados. |
| `C0DE/voxid/Drivers` | HAL y CMSIS del fabricante. |
| `C0DE/voxid/tests` | Once pruebas numericas y regresiones de voz, adquisicion e interfaz. |
| `C0DE/voxid/tests/support` | HAL simulada, exclusiva para PC. |
| `C0DE/voxid/tests/build` | Ejecutables de pruebas generados y excluidos de Git. |
| `DOC` | Seguimiento y validacion. |
| `PCB` | Proyecto KiCad `voxid` y archivos de fabricacion versionados. |
| `SIM` | Preparacion de grabaciones en MATLAB, antecedentes PSIM y experimentos SPICE; ver [la guia](SIM/README.md). |

## Compilar y probar en PC

Se necesitan GCC y GNU Make. Desde la raiz del repositorio:

```bash
make -C C0DE/voxid all
make -C C0DE/voxid test
make -C C0DE/voxid test test_voice.c
make -C C0DE/voxid clean
```

En Windows con MinGW, usar `mingw32-make` en lugar de `make`. La ruta de ejecucion y la limpieza tienen reglas para ambos sistemas; la verificacion automatizada de esta revision se realizo en Linux.

`all` compila las once pruebas sin ejecutarlas. `test` compila lo necesario y ejecuta Frame, ventana, FFT, MFCC, voz, adquisicion, interfaz, GFX, registro, persistencia e identificacion. Los ejecutables se generan en `C0DE/voxid/tests/build`; `clean` elimina solo ese directorio. La creacion del directorio admite reconstruccion forzada con `make -B`. Las pruebas con HAL de PC no sustituyen las mediciones en placa.

El Makefile busca primero las cabeceras de `tests/support`, para evitar mezclar `main.h` de la placa con su sustituto de PC. Las configuraciones Debug y Release de CubeIDE incluyen fuentes solo de `Core` y `Drivers`, por lo que no compilan las pruebas ni sus sustitutos HAL.

## Compilar el firmware

1. Importar directamente `C0DE/voxid` en STM32CubeIDE: los ocho modulos DSP son archivos fisicos de `Core`, sin enlaces externos.
2. Confirmar que `Core/Src` contenga frame, window, fft, power, mel, log_mel, dct y mfcc, ademas de los modulos del firmware.
3. Refrescar con F5, ejecutar Clean y Build Debug, y programar Ophyra.
4. Seguir [la validacion de etapas 1 y 2](DOC/Validacion_etapas_1_2.md), [el registro de etapa 3](DOC/STAGE3_ENROLLMENT.md) y [la validacion de etapa 4](DOC/STAGE4_STORAGE_IDENTIFY.md).

La fase 3 de reorganizacion deja unicamente `voxid` dentro de `C0DE`. Retira las copias externas del DSP, el benchmark sintetico separado y el ejecutable minimo de PC con su desensamblado. Las once pruebas utilizan las fuentes de `Core`; las seis guias del firmware se concentran en `DOC`. Los fuentes, parametros y configuracion ARM del firmware se conservan.

Los modulos propios de Core siguen `Estandar_Programacion.md` v2.0: ASCII, cabeceras consistentes, rutas correctas y argumentos de funciones en una linea. Se conservan las APIs y algoritmos; los archivos autogenerados y HAL/CMSIS mantienen su formato original.

El Makefile de PC no genera firmware ARM. Si CubeMX regenera codigo, comprobar la configuracion DMA del `.ioc`, la vinculacion con ADC1 y el orden GPIO, DMA, ADC1, SPI1, TIM2, I2C1 y los GPIO PB6/PB7 de I2C1.

## Hardware y simulaciones

`main` integra el firmware de las cuatro etapas, el diseno analogico de `PCB/voxid.kicad_sch` y `PCB/voxid.kicad_pcb`, sus archivos de fabricacion y el trabajo de simulacion. Se conserva `pcb-sim` para continuar los cambios de hardware y simulacion; las ramas de etapas incorporadas se retiran sin borrar sus commits.

Los tres experimentos SPICE actualizados usan los dieciseis componentes comerciales de las cuatro secciones de filtro de la PCB. Desde la raiz, comprobar su correspondencia con:

```bash
python3 SIM/tests/comprobar_valores.py
```

Los antecedentes PSIM y los resultados historicos de filtros se conservan por separado en `SIM`. Deben repetirse los analisis AC, Monte Carlo y ruido con los valores vigentes. Las simulaciones, los Gerbers y los archivos KiCad no acreditan por si solos mediciones fisicas ni aprobacion ERC/DRC o de fabricacion.

La preparacion de grabaciones en MATLAB lee archivos o una carpeta, selecciona un canal, retira la media y remuestrea a 16 kHz cuando hace falta. Exporta WAV, CSV y MAT sin modificar los originales. Sus grabaciones y resultados locales quedan fuera de Git. Esta preparacion todavia no calcula MFCC ni identifica locutores; la ejecucion de sus pruebas en MATLAB y la lectura de un MP4 real siguen pendientes. Consultar [SIM/README.md](SIM/README.md) para requisitos y comandos.

El usuario confirmo el funcionamiento general del firmware y la navegacion LOCUTORES. Las mediciones de temporizacion, la calibracion de umbrales y los ensayos de fallos en hardware requieren evidencia especifica. Consultar [el seguimiento](DOC/CheckList.txt) para distinguir estos pendientes de la implementacion y las pruebas en PC.

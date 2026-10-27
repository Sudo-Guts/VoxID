# Simulaciones de VoxID

## Preparacion de grabaciones en MATLAB

Esta primera etapa lee una pronunciacion de MARIPOSA por archivo y prepara su
audio. Todavia no calcula MFCC, registra perfiles, clasifica personas ni estima
exactitud. Una toma por persona permite explorar las grabaciones disponibles;
evaluar repetibilidad requiere otras tomas y comparar palabras requiere otras
palabras pronunciadas bajo un protocolo comparable.

### Archivos

| Ruta | Responsabilidad |
|---|---|
| `main.m` | Procesar un archivo o una carpeta y reunir las salidas. |
| `configuracion.m` | Parametros de audio y rutas relativas a SIM. |
| `funciones/preparar_audio.m` | Leer y preparar una toma sin modificar el original. |
| `datos/originales/` | MP4, M4A, WAV o FLAC locales; una toma por archivo. |
| `datos/preparados/` | WAV mono generados, agrupados por ejecucion. |
| `resultados/` | Resumen CSV y datos MAT de cada ejecucion. |

Las grabaciones y las salidas se excluyen de Git. PSIM y SPICE conservan sus
carpetas y su procedimiento independiente.

### Requisitos y ejecucion

Se apunta a MATLAB R2020b o posterior. `audioread` debe poder decodificar la pista
de audio del MP4 en el equipo; la extension no garantiza un codec compatible.
Convertir a 16 kHz con `resample` requiere Signal Processing Toolbox. La lectura
de audio que ya esta a 16 kHz no utiliza esa funcion. Esta etapa no requiere
Audio Toolbox.

Desde la raiz del repositorio, en MATLAB:

```matlab
addpath('SIM');
[resumen, tomas] = main();
```

Antes, colocar los archivos directamente en `SIM/datos/originales/`. No se buscan
subcarpetas. Se sugiere `locutor_01_mariposa_01.mp4`, sin asignar automaticamente
una identidad a partir del nombre. Tambien se puede indicar un archivo o carpeta:

```matlab
[resumen, tomas] = main('ruta/al/locutor_01_mariposa_01.mp4');
[resumen, tomas] = main('ruta/a/las/grabaciones');
```

Una carpeta sin archivos compatibles devuelve resultados vacios e instrucciones.
Un archivo que falla queda como `ERROR` en el resumen y no impide procesar los
demas. `tomas{indice}` esta vacia para ese archivo; las filas `OK` o `REVISAR`
incluyen la estructura de preparacion y su ruta WAV.

### Decisiones de preparacion

- Seleccionar el canal 1, configurable. No se promedian canales: dos canales de
  fase opuesta podrian cancelarse. El numero de canales queda en el resumen.
- Mantener la duracion disponible. No se fuerza una toma de tres segundos, no se
  rellena con silencio y no se altera la velocidad de pronunciacion.
- Dejar `intervalo_s = []` para conservar la toma completa. Un intervalo opcional
  `[inicio, fin]` en segundos selecciona `[inicio, fin)` en la frecuencia original;
  se registran los limites efectivos de las muestras seleccionadas. Para recortes
  distintos por persona, ejecutar cada archivo con su configuracion.
- Restar la media del segmento cuando `retirar_media` esta activo. Es una
  preparacion exploratoria; no reproduce el filtro DC ni el ADC del firmware.
- Remuestrear con filtro antialiasing cuando la frecuencia original difiere de
  16 kHz. Aumentar una frecuencia menor no recupera banda que no fue capturada.
- Mantener la amplitud salvo que el resultado exceda el margen de exportacion;
  en ese caso solo se atenua y se registra `ganancia_aplicada`. Nunca se aumenta
  automaticamente una toma debil. Una atenuacion no repara distorsion previa.
- Exportar WAV PCM de 24 bits. El MAT conserva la senal calculada antes de esa
  cuantizacion y la configuracion completa. No se afirma que la grabacion original
  ni el ADC tengan una resolucion efectiva de 24 bits.

El resumen muestra frecuencia original, canal, duraciones, intervalo efectivo,
pico de entrada, RMS preparado, fraccion de muestras cercanas a escala completa,
media retirada y ganancia. `REVISAR` utiliza indicadores provisionales de nivel
bajo o muestras cercanas al riel; no demuestra presencia de voz, ruido o recorte.
No se aplica reduccion de ruido, deteccion automatica de MARIPOSA ni eliminacion
automatica de silencios. Un segmento que no alcanza un frame se informa como error.

Ejemplo de configuracion para un solo archivo:

```matlab
config = configuracion();
config.audio.canal = 1;
config.audio.intervalo_s = [0.5, 2.0];
[resumen, tomas] = main('ruta/a/una_toma.mp4', config);
```

Para inspeccionar una toma preparada, usar el indice de una fila sin error:

```matlab
indice = 1;
toma = tomas{indice};
tiempo_s = (0:numel(toma.senal) - 1) / toma.frecuencia_hz;
plot(tiempo_s, toma.senal);
xlabel('Tiempo (s)');
ylabel('Amplitud normalizada');
grid on;
```

Cada ejecucion crea directorios nuevos, con el mismo identificador en preparados
y resultados. `resumen_preparacion.csv` permite revisar los estados;
`preparacion_audio.mat` conserva configuracion, tabla y tomas. Las rutas se
imprimen al terminar. Los archivos originales no se escriben ni reemplazan.

### Pruebas de la preparacion

Desde la raiz del repositorio, en MATLAB:

```matlab
addpath('SIM/tests');
probar_preparacion_audio();
```

Los ensayos generan WAV sinteticos y comprueban canales de fase opuesta,
media, recorte manual, silencio, atenuacion de salida, errores, exportacion,
restauracion de rutas y conservacion de los originales. Si `resample` esta
disponible, verifican conversion de 48 a 16 kHz y rechazo de una componente
de 12 kHz que se plegaria a 4 kHz. Los archivos temporales se eliminan al salir.
Los codecs MP4 se comprueban aparte con las grabaciones reales del equipo.

### Siguiente etapa

`configuracion.m` reserva 400 muestras por frame, avance de 160, FFT de 512,
26 filtros Mel y 13 MFCC. El extractor y su comparacion con el firmware se
implementaran despues, incluyendo convenciones de ventana, banco Mel, escalado,
logaritmo e indices. Esos campos todavia no ejecutan la cadena MFCC.

Referencias oficiales: [audioread](https://www.mathworks.com/help/matlab/ref/audioread.html),
[resample](https://www.mathworks.com/help/signal/ref/resample.html) y
[audiowrite](https://www.mathworks.com/help/matlab/ref/audiowrite.html).

## Simulaciones analogicas

La referencia de componentes es `PCB/voxid.kicad_sch`, commit `001fb24`. Esta corrección actualiza los experimentos SPICE y conserva el diseño físico.

## Experimentos con valores de placa

| Archivo | Análisis |
|---|---|
| `spice/Filtro_PCB_AC.cir` | AC y transitorio nominales, cortes y ancho de banda. |
| `spice/Filtro_Real_MonteCarlo.cir` | 100 iteraciones LTspice, R ±1 % y C ±5 %. |
| `spice/Filtro_Real_Ruido.cir` | Ruido de salida entre 100 Hz y 6 kHz. |

Los tres usan `spice/TL072.lib` y alimentación de ±12 V.

| Sección | Resistencias | Capacitores |
|---|---|---|
| HPF 1 | R1 = 1.2 kΩ; R2 = 8.2 kΩ | C1 = C2 = 470 nF |
| HPF 2 | R3 = 3.3 kΩ; R4 = 3.9 kΩ | C3 = C4 = 470 nF |
| LPF 1 | R5 = 2.2 kΩ; R6 = 6.8 kΩ | C6 = 22 nF; C7 = 2.2 nF |
| LPF 2 | R10 = 820 Ω; R14 = 3.9 kΩ | C8 = 22 nF; C9 = 10 nF |

Son ensayos de cuatro secciones en cascada: se supone la conexión de salida HPF a entrada LPF para el experimento. No son una exportación completa de las conexiones de la placa. No incluyen micrófono, preamplificación, offset, protección ADC, carga real ni parásitos.

Los componentes comerciales modifican la respuesta respecto del diseño ideal. Ejecutar nuevamente los análisis para conocer cortes, ganancias y ruido de esta revisión. No se presentan resultados nuevos sin ejecutar el simulador.

## Verificación de correspondencia

Desde la raíz:

```bash
python3 SIM/tests/comprobar_valores.py
```

El comprobador lee los componentes instanciados en KiCad y coteja los dieciséis valores nominales con los tres netlists, también dentro de `mc()`. Comprueba valores; no ejecuta SPICE ni reemplaza ERC/DRC.

Abrir los experimentos en LTspice y ejecutar cada análisis. Las tolerancias del Monte Carlo representan el supuesto indicado; el ancho de banda se calcula como FH - FL.

## Referencias históricas

- `spice/Filtro_Referencia_Ruido.cir` conserva el circuito anterior de comparación.
- `spice/Filtro_KiCad_ngspice.cir` corresponde a una propuesta previa con otros valores RC.
- Los proyectos de `psim/` se conservan como antecedentes. No se declara que estén sincronizados con la PCB actual.
- Los valores antes anotados, 2.6088 y 2.6253 µV RMS, pertenecen a experimentos previos. No describen los netlists actualizados ni una medición física.
- El modelo TL072 y las tolerancias supuestas limitan el alcance de los experimentos. Las mediciones reales, ERC/DRC y validación de fabricación siguen pendientes.

Las salidas automáticas RAW, LOG y resultados AC no deben almacenarse como código fuente.


# Etapa 3: tres prototipos MFCC por locutor

La etapa 3 registra cuatro locutores, con tres prototipos independientes por locutor. La etapa 3 introdujo el banco RAM y la sesión de tres tomas. La versión actual incorpora el guardado en EEPROM y la recuperación al reiniciar, descritos en [etapa 4](STAGE4_STORAGE_IDENTIFY.md).

Un prototipo es un vector de 13 coeficientes: la media de los frames seleccionados por energía de una toma de MARIPOSA. No es un frame aislado ni la media de las tres tomas juntas.

## Flujo en Ophyra

1. Entrar en REGISTRAR y elegir locutor con UP/DN.
2. Pulsar OK para preparar TOMA 1/3 y OK para comenzar.
3. Durante los tres segundos se mantienen los círculos fijos y solo aparece BACK.
4. Al terminar, UP/DN permite revisar resumen y gráfica de la toma.
5. Si la toma es válida, OK la acepta y comienza la siguiente: TOMA 2/3 y después TOMA 3/3.
6. En la tercera toma válida aparece OK: GUARDAR. Al aceptarla se verifica la escritura EEPROM y se publica el perfil completo y se vuelve al selector.
7. El selector muestra GUARDADO EN EEPROM. OK: REGRABAR inicia un reemplazo de tres tomas.

Si falta voz, la pantalla indica VOZ INSUFICIENTE y OK: REPETIR VOZ. La repetición conserva el número de toma y las aceptadas anteriormente. Un error de ADC/DMA tampoco añade un prototipo.

BACK abandona el registro y descarta sus tomas pendientes. Si había un perfil completo del mismo locutor, sigue disponible hasta aceptar las tres tomas del reemplazo y verificar su persistencia.

## Criterio de aceptación

Cada toma debe contener las 48 000 muestras y los 298 frames esperados, media MFCC finita con C01..C12 no degenerados, metadatos PCM coherentes y al menos 20 frames seleccionados por energía.

Los parámetros están en `Core/Inc/voxid_config.h`:

| Parámetro | Valor inicial |
|---|---|
| VOXID_SPEAKER_COUNT | 4 |
| VOXID_PROTOTYPES_PER_SPEAKER | 3 |
| VOXID_ENROLL_MIN_VOICED_FRAMES | 20 |
| VOXID_VOICE_MIN_RMS | 0.01, respecto de escala completa PCM |

El mínimo de 20 frames es provisional y requiere pruebas con las voces reales y la ganancia utilizada. Como los frames se traslapan, ese conteo no representa 20 intervalos independientes de 25 ms. El criterio de energía no verifica la palabra MARIPOSA ni descarta todo tipo de ruido.

## Código

`voxid_enroll.c/.h` mantiene el banco de perfiles y una sesión pendiente. Cada prototipo conserva su vector MFCC y el número de frames seleccionados que lo originaron. El perfil anterior se reemplaza únicamente al aceptar la tercera toma y confirmar el guardado.

El módulo usa memoria estática y delega la persistencia a `voxid_storage`; la comparación se implementa en `voxid_identify`. `voxid_enroll_profile()` permite consultar un perfil completo sin modificarlo; devuelve NULL si el espacio está vacío o es inválido.

La UI acepta una toma una sola vez por pulsación válida. Mientras la próxima captura está pendiente o activa, se ocultan e ignoran UP, DN y OK; BACK permite cancelar. Esto también cubre el instante en que ADC aún conserva READY del resultado anterior.

## Verificación en PC

```bash
make -C C0DE test
make -C C0DE test test_enroll.c
make -C C0DE test test_ui.c
```

Las pruebas de registro comprueban tres vectores distintos, copia de sus datos, rechazo de entradas incompletas/no finitas, cancelación, reemplazo y cuatro locutores independientes.

Un fallo de guardado conserva las dos tomas aceptadas y el resultado de la tercera, con OK: REINTENTAR sin recaptura. Las pruebas también verifican recuperación de perfiles tras cargar EEPROM.

La prueba de UI utiliza el procesamiento real con ADC/DMA simulados y una señal sintética. Comprueba las tres tomas, repetición por silencio y error, contador, aceptación explícita, ocultamiento de controles y conservación del perfil previo.

## Verificación física pendiente

1. Refrescar con F5, Clean y Build Debug en STM32CubeIDE. Comprobar que se compila `voxid_enroll.c`.
2. Registrar tres pronunciaciones distintas de MARIPOSA para el mismo locutor, observando TOMA 1/3, 2/3 y 3/3.
3. Comprobar que durante cada captura solo se ve BACK, incluso al pasar de una toma a la siguiente.
4. Probar una toma en silencio: debe poder repetirse sin avanzar el registro.
5. Cancelar una regrabación y confirmar que el selector conserva el perfil anterior.
6. Registrar otro locutor y comprobar que cada espacio mantiene su estado.
7. Reiniciar la placa y confirmar que los perfiles confirmados siguen disponibles.

Consultar [etapa 4](STAGE4_STORAGE_IDENTIFY.md) para el formato persistente, el reintento y la validación de identificación.

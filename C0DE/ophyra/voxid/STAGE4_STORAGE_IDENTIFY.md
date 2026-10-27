# Etapa 4: EEPROM e identificación

La versión actual conserva los tres prototipos MFCC de cada uno de los cuatro locutores en la M24C32 y los recupera al arrancar. IDENTIFICAR compara una toma nueva con los perfiles recuperados en RAM.

La implementación y las pruebas de PC están completas. Quedan pendientes la compilación y el enlace ARM, la prueba en Ophyra y la calibración con voces reales. Los perfiles de etapa 3 estaban solo en RAM: deben registrarse una vez con esta versión para que queden persistidos.

## Integración con Ophyra

| Parámetro | Configuración |
|---|---|
| Controlador | I2C1, modo estándar a 100 kHz |
| Reloj | PB6, AF4, drenador abierto |
| Datos | PB7, AF4, drenador abierto |
| Dirección del dispositivo | 0x50 en 7 bits; HAL recibe 0xA0 |
| Dirección interna | Dos bytes, `I2C_MEMADD_SIZE_16BIT` |
| Memoria | 4096 bytes, páginas de 32 bytes |
| Esperas | Timeout HAL de 20 ms por transferencia y sondeo ACK de 10 ms |

El bus ya está conectado a la EEPROM integrada de Ophyra. La configuración sigue el [manual de Ophyra Rev E, sección 10](https://intesc.mx/wp-content/uploads/2021/08/ManualOphyraRevE.pdf) y el [tutorial de EEPROM del fabricante](https://intesc.mx/9-lectura-de-la-memoria-m24c32-eeprom/). El formato de transferencia y los límites de página siguen la [hoja de datos M24C32 de ST](https://www.st.com/resource/en/datasheet/m24c32-r.pdf).

`main.c`, el MSP, `stm32f4xx_hal_conf.h` y `voxid.ioc` habilitan I2C1. Se incluyen los archivos I2C/I2CEx oficiales de [HAL STM32F4 v1.8.5](https://github.com/STMicroelectronics/stm32f4xx-hal-driver/tree/v1.8.5), la misma versión que usa el proyecto, conservando su licencia y contenido. El proyecto de benchmark `voxid_test` no cambia.

Todas las operaciones EEPROM se ejecutan desde el contexto principal, con SysTick activo y la captura detenida. No se escribe al arrancar, al identificar, al aceptar las dos primeras tomas ni al cancelar. La identificación consulta RAM y no genera tráfico I2C.

## Registro y reintento

1. REGISTRAR permite elegir LOCUTOR 01 a 04.
2. Se capturan tres tomas independientes, con aceptación explícita mediante OK.
3. En la tercera toma aparece `OK: GUARDAR`. Se escribe y verifica el perfil antes de actualizar el banco RAM.
4. El selector muestra `GUARDADO EN EEPROM` al terminar.
5. BACK cancela el registro pendiente y conserva el perfil anterior.

Si falla el guardado, se muestra `ERROR AL GUARDAR`, `EEPROM NO CONFIRMA` y `OK: REINTENTAR`. La tercera toma sigue disponible y el contador permanece en dos: OK reintenta guardarla sin capturar otra vez. BACK cancela. El perfil RAM anterior no se sustituye ante un error.

Un error posterior a escribir la confirmación puede impedir comprobarla aunque el nuevo registro ya sea válido en EEPROM. Al reiniciar se recuperará la copia válida más reciente. Si se reintenta la misma toma y esa copia ya existe, se verifica sin repetir las escrituras.

Durante cualquier captura, incluida la transición entre tomas, solo aparece BACK y se ignoran UP/DN/OK.

## Formato persistente

Cada locutor usa dos registros de 256 bytes. Los primeros 2048 bytes quedan asignados a perfiles; la mitad restante no se modifica.

| Locutor | Copia A | Copia B |
|---|---|---|
| 01 | 0x000 | 0x100 |
| 02 | 0x200 | 0x300 |
| 03 | 0x400 | 0x500 |
| 04 | 0x600 | 0x700 |

Cada registro se serializa explícitamente en little-endian, sin copiar el relleno de las estructuras C. Los coeficientes usan IEEE 754 de 32 bits, comprobado al compilar.

| Bytes | Contenido |
|---|---|
| 0–3 | Firma `VXID` |
| 4–5 | Versión del formato: 1 |
| 6–7 | Índice de locutor y cantidad de coeficientes |
| 8–11 | Secuencia de actualización, uint32 |
| 12–13 | Cantidad de prototipos y revisión DSP |
| 14–19 | Frames esperados y frecuencia de muestreo |
| 20–27 | Tamaño de frame, avance, FFT y filtros Mel |
| 28–31 | Longitud de datos y dos bytes reservados |
| 32–193 | Tres grupos de 13 floats y un uint16 de frames seleccionados |
| 194–219 | Relleno a cero incluido en el CRC |
| 220–223 | CRC32 de los bytes 0–219 |
| 224–227 | Confirmación 0xC04D17ED, en una página independiente |
| 228–255 | Sin uso |

El CRC32 utiliza polinomio reflejado 0xEDB88320, inicio 0xFFFFFFFF y XOR final 0xFFFFFFFF. Detecta corrupción accidental; no autentica el contenido.

El lector exige firma, versión, parámetros DSP compatibles, confirmación, CRC, coeficientes finitos, C01..C12 no degenerados y conteos válidos. `VOXID_PROFILE_DSP_REVISION` debe cambiar si cambia la definición del vector, el filtrado o la extracción sin cambiar sus dimensiones. Un formato incompatible no se migra ni se formatea automáticamente.

Al actualizar se elige la copia que no contiene la última versión válida. Primero se invalida su confirmación y se comprueba; después se escribe y relee el cuerpo; al final se escribe la confirmación y se valida el registro completo. La copia vigente queda intacta. La secuencia se compara con aritmética modular, incluido el paso de UINT32_MAX a cero.

El arranque recupera la copia válida más reciente de cada locutor. Si una está dañada, utiliza la otra. Sin copias válidas, el espacio queda vacío. Un fallo de lectura no se interpreta como memoria vacía; los otros espacios que sí pudieron leerse se conservan. DIAGNOSTICO distingue memoria lista, vacía, datos dañados y error I2C. El estado también refleja el resultado del último guardado.

## Distancia normalizada

Se utilizan C01..C12; C00 se conserva en EEPROM y en la gráfica, pero se excluye de la comparación por su relación con la energía global. Cada vector se divide entre su norma L2. La normalización se calcula con escalado previo para evitar elevar al cuadrado magnitudes arbitrarias.

Para una toma q y los tres prototipos p de un locutor s:

\[
\hat q = q / \lVert q \rVert_2, \qquad
D_s = \frac{1}{3}\sum_{j=1}^{3}\lVert\hat q-\hat p_{s,j}\rVert_2
\]

La distancia está entre 0 y 2; menor significa mayor similitud bajo este criterio. Se promedian las tres distancias, de modo que un prototipo cercano no decide por sí solo. No se promedian los prototipos para sustituirlos por un único vector.

| Resultado | Condición |
|---|---|
| VOZ INSUFICIENTE | Captura inválida, menos de 20 frames seleccionados o vector degenerado |
| SIN LOCUTORES | Toma válida, sin perfiles completos utilizables |
| DESCONOCIDO | La menor distancia supera `VOXID_IDENTIFY_MAX_DISTANCE`, inicialmente 0.35 |
| VOZ AMBIGUA | La distancia es aceptable, pero el segundo candidato queda a menos de `VOXID_IDENTIFY_MIN_MARGIN`, inicialmente 0.05, del primero |
| LOCUTOR nn | Cumple distancia y margen |

Con un solo perfil no existe segundo candidato: se comprueba la distancia y se muestra `UN SOLO PERFIL`. Los empates entre perfiles se rechazan como ambiguos. La distancia no es un porcentaje de confianza ni verifica que se haya pronunciado MARIPOSA.

UP/DN recorre resultado, gráfica MFCC y estadísticas ADC. OK inicia otra toma; BACK vuelve. En REGISTRAR se mantienen las dos páginas de la toma: resumen y gráfica.

Los valores 0.35 y 0.05 son iniciales, no umbrales validados. Deben ajustarse con nuevas tomas de los locutores registrados y personas no registradas, manteniendo separados registro y evaluación. También siguen pendientes la calibración RMS, el mínimo de frames y la comprobación de ruido de la etapa analógica.

## Código y pruebas

| Módulo | Responsabilidad |
|---|---|
| `m24c32.c/.h` | Límites de memoria, direcciones de dos bytes, división por páginas y espera ACK |
| `voxid_storage.c/.h` | Formato, validación, dos copias y carga inicial |
| `voxid_enroll.c/.h` | Sesión de tres tomas; publicación RAM solo tras guardado verificado |
| `voxid_identify.c/.h` | Normalización, distancias y rechazo |
| `ui.c` | Registro, reintento de guardado y presentación del resultado |

No se reserva memoria dinámica ni se conserva la grabación completa.

```bash
make -C C0DE all test CFLAGS='-std=c11 -Wall -Wextra -Wpedantic -Werror'
make -C C0DE test test_storage.c
make -C C0DE test test_identify.c
make -C C0DE test test_ui.c
```

Pasan las once pruebas de PC. La prueba EEPROM cubre páginas, límites, ACK, formato incompatible con CRC correcto, corrupción de datos, cuatro espacios, reinicio, copia anterior y desbordamiento de secuencia. Inyecta 233 puntos de corte entre bytes de una actualización y 32 fallos de lectura. Ese modelo comprueba el protocolo del código, no todos los efectos eléctricos de una caída de tensión real.

La identificación se contrasta con distancias analíticas, cambio de C00, escalado, promedio de los tres prototipos, umbrales, empates y cuatro locutores. La UI usa ADC→PCM→MFCC reales con HAL de prueba: comprueba reintento sin recaptura, identificación tras recargar EEPROM, ambigüedad y controles ocultos durante captura.

Registro, persistencia, identificación y UI se comprobaron además con AddressSanitizer y UndefinedBehaviorSanitizer, sin comprobación de fugas. Se revisó sintaxis de 19 archivos con cabeceras reales STM32 y HAL I2C. Estas comprobaciones no generan un binario ARM ni validan el enlace, la memoria o los tiempos en placa.

## Validación pendiente en Ophyra

1. Actualizar a `feature/eeprom-identificacion`, refrescar F5, Clean y Build Debug. Verificar que se compilan los nuevos módulos y HAL I2C/I2CEx; programar la placa.
2. En DIAGNOSTICO comprobar el estado de EEPROM y ejecutar la autoprueba MFCC.
3. Registrar tres tomas de un locutor. Comprobar que solo aparece BACK durante las capturas y que termina con `GUARDADO EN EEPROM`.
4. Reiniciar sin regrabar: el mismo espacio debe seguir ocupado. IDENTIFICAR con una pronunciación nueva debe mostrar un resultado y su distancia.
5. Cancelar una regrabación después de una o dos tomas. Reiniciar y comprobar que sigue disponible el perfil anterior.
6. Registrar los otros locutores y comprobar nuevas tomas de cada uno y de personas no registradas. Anotar distancias, márgenes y rechazos para calibrar.
7. Comprobar recuperación y errores I2C en un banco de prueba controlado. Verificar tiempos y que no se pierden bloques de adquisición; la resistencia a cortes eléctricos sigue pendiente de ensayo físico.

No se marca la etapa como validada físicamente hasta completar estas comprobaciones con la versión compilada.

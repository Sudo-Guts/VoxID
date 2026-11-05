# Etapa 4: EEPROM e identificacion

La version actual conserva los tres prototipos MFCC de cada uno de los cuatro locutores en la M24C32 y los recupera al arrancar. IDENTIFICAR compara una toma nueva con los perfiles recuperados en RAM.

El usuario reporto reconocimiento funcional con tomas de 3 segundos y fallos al usar 2.5 segundos. Los conteos actuales son coherentes, pero falta determinar en Ophyra la causa de esos fallos y calibrar con voces reales. Las nuevas capturas duran 2500 ms y producen 248 frames. Los bytes 14-15 deben indicar los 248 frames actuales. Los perfiles anteriores de 3 segundos se rechazan y deben registrarse de nuevo con tomas de 2.5 segundos. La duracion no altera el algoritmo ni la revision DSP del vector. El arranque no migra ni borra la EEPROM. Un registro incompatible queda sin perfil utilizable; LOCUTORES muestra PERFIL NO UTILIZABLE y REGISTRA TRES TOMAS hasta reemplazarlo. No se anade una ruta de migracion ni se cambia la revision DSP, porque la definicion del vector se conserva.

## Integracion con Ophyra

| Parametro | Configuracion |
|---|---|
| Controlador | I2C1, modo estandar a 100 kHz |
| Reloj | PB6, AF4, drenador abierto |
| Datos | PB7, AF4, drenador abierto |
| Direccion del dispositivo | 0x50 en 7 bits; HAL recibe 0xA0 |
| Direccion interna | Dos bytes, `I2C_MEMADD_SIZE_16BIT` |
| Memoria | 4096 bytes, paginas de 32 bytes |
| Esperas | Timeout HAL de 20 ms por transferencia y sondeo ACK de 10 ms |

El bus ya esta conectado a la EEPROM integrada de Ophyra. La configuracion sigue el [manual de Ophyra Rev E, seccion 10](https://intesc.mx/wp-content/uploads/2021/08/ManualOphyraRevE.pdf) y el [tutorial de EEPROM del fabricante](https://intesc.mx/9-lectura-de-la-memoria-m24c32-eeprom/). El formato de transferencia y los limites de pagina siguen la [hoja de datos M24C32 de ST](https://www.st.com/resource/en/datasheet/m24c32-r.pdf).

`main.c`, el MSP, `stm32f4xx_hal_conf.h` y `voxid.ioc` habilitan I2C1. Se incluyen los archivos I2C/I2CEx oficiales de [HAL STM32F4 v1.8.5](https://github.com/STMicroelectronics/stm32f4xx-hal-driver/tree/v1.8.5), la misma version que usa el proyecto, conservando su licencia y contenido.

Todas las operaciones EEPROM se ejecutan desde el contexto principal, con SysTick activo y la captura detenida. No se escribe al arrancar, al identificar, al aceptar las dos primeras tomas ni al cancelar. La identificacion consulta RAM y no genera trafico I2C.

## Registro y reintento

1. REGISTRAR permite elegir LOCUTOR 01 a 04.
2. Se capturan tres tomas independientes, con aceptacion explicita mediante OK.
3. En la tercera toma aparece `OK: GUARDAR`. Se escribe y verifica el perfil antes de actualizar el banco RAM.
4. El selector muestra `GUARDADO EN EEPROM` al terminar.
5. BACK cancela el registro pendiente y conserva el perfil anterior.

Si falla el guardado, se muestra `ERROR AL GUARDAR`, `EEPROM NO CONFIRMA` y `OK: REINTENTAR`. La tercera toma sigue disponible y el contador permanece en dos: OK reintenta guardarla sin capturar otra vez. BACK cancela. El perfil RAM anterior no se sustituye ante un error.

Un error posterior a escribir la confirmacion puede impedir comprobarla aunque el nuevo registro ya sea valido en EEPROM. Al reiniciar se recuperara la copia valida mas reciente. Si se reintenta la misma toma y esa copia ya existe, se verifica sin repetir las escrituras.

Durante cualquier captura, incluida la transicion entre tomas, solo aparece BACK y se ignoran UP/DN/OK.

## Formato persistente

Cada locutor usa dos registros de 256 bytes. Los primeros 2048 bytes quedan asignados a perfiles; la mitad restante no se modifica.

| Locutor | Copia A | Copia B |
|---|---|---|
| 01 | 0x000 | 0x100 |
| 02 | 0x200 | 0x300 |
| 03 | 0x400 | 0x500 |
| 04 | 0x600 | 0x700 |

Cada registro se serializa explicitamente en little-endian, sin copiar el relleno de las estructuras C. Los coeficientes usan IEEE 754 de 32 bits, comprobado al compilar.

| Bytes | Contenido |
|---|---|
| 0-3 | Firma `VXID` |
| 4-5 | Version del formato: 1 |
| 6-7 | Indice de locutor y cantidad de coeficientes |
| 8-11 | Secuencia de actualizacion, uint32 |
| 12-13 | Cantidad de prototipos y revision DSP |
| 14-19 | Frames esperados por toma (248) y frecuencia de muestreo |
| 20-27 | Tamano de frame, avance, FFT y filtros Mel |
| 28-31 | Longitud de datos y dos bytes reservados |
| 32-193 | Tres grupos de 13 floats y un uint16 de frames seleccionados |
| 194-219 | Relleno a cero incluido en el CRC |
| 220-223 | CRC32 de los bytes 0-219 |
| 224-227 | Confirmacion 0xC04D17ED, en una pagina independiente |
| 228-255 | Sin uso |

El CRC32 utiliza polinomio reflejado 0xEDB88320, inicio 0xFFFFFFFF y XOR final 0xFFFFFFFF. Detecta corrupcion accidental; no autentica el contenido.

El lector exige firma, version, parametros DSP compatibles, confirmacion, CRC, coeficientes finitos, C01..C12 no degenerados y conteos validos. `VOXID_PROFILE_DSP_REVISION` debe cambiar si cambia la definicion del vector, el filtrado o la extraccion sin cambiar sus dimensiones. Un formato incompatible no se migra ni se formatea automaticamente.

Al actualizar se elige la copia que no contiene la ultima version valida. Primero se invalida su confirmacion y se comprueba; despues se escribe y relee el cuerpo; al final se escribe la confirmacion y se valida el registro completo. La copia vigente queda intacta. La secuencia se compara con aritmetica modular, incluido el paso de UINT32_MAX a cero.

El arranque recupera la copia valida mas reciente de cada locutor. Si una esta danada, utiliza la otra. Sin copias validas, el espacio queda vacio. Un fallo de lectura no se interpreta como memoria vacia; los otros espacios que si pudieron leerse se conservan. LOCUTORES consulta los espacios cargados y sus referencias. Cada espacio conserva su propio estado de carga o guardado. Si no tiene un perfil utilizable, LOCUTORES distingue espacio vacio, perfil no utilizable, error I2C y memoria no inicializada. Un fallo no modifica el estado de los otros locutores; la vista no vuelve a consultar el bus.

## Distancia normalizada

Se utilizan C01..C12; C00 se conserva en EEPROM y en la grafica, pero se excluye de la comparacion por su relacion con la energia global. Cada vector se divide entre su norma L2. La normalizacion se calcula con escalado previo para evitar elevar al cuadrado magnitudes arbitrarias.

Para una toma q y los tres prototipos p de un locutor s:

\[
\hat q = q / \lVert q \rVert_2, \qquad
D_s = \frac{1}{3}\sum_{j=1}^{3}\lVert\hat q-\hat p_{s,j}\rVert_2
\]

La distancia esta entre 0 y 2; menor significa mayor similitud bajo este criterio. Se promedian las tres distancias, de modo que un prototipo cercano no decide por si solo. No se promedian los prototipos para sustituirlos por un unico vector.

| Resultado | Condicion |
|---|---|
| VOZ INSUFICIENTE | Captura invalida, menos de 20 frames seleccionados o vector degenerado |
| SIN LOCUTORES | Toma valida, sin perfiles completos utilizables |
| DESCONOCIDO | La menor distancia supera `VOXID_IDENTIFY_MAX_DISTANCE`, inicialmente 0.35 |
| VOZ AMBIGUA | La distancia es aceptable, pero el segundo candidato queda a menos de `VOXID_IDENTIFY_MIN_MARGIN`, inicialmente 0.05, del primero |
| LOCUTOR nn | Cumple distancia y margen |

Con un solo perfil no existe segundo candidato: se comprueba la distancia y se muestra `UN SOLO PERFIL`. Los empates entre perfiles se rechazan como ambiguos. La distancia no es un porcentaje de confianza ni verifica que se haya pronunciado MARIPOSA.

UP/DN recorre resultado, grafica MFCC y estadisticas ADC. OK inicia otra toma; BACK vuelve. En REGISTRAR se mantienen las dos paginas de la toma: resumen y grafica.

Los valores 0.35 y 0.05 son iniciales, no umbrales validados. Deben ajustarse con nuevas tomas de los locutores registrados y personas no registradas, manteniendo separados registro y evaluacion. Tambien siguen pendientes la calibracion RMS, el minimo de frames y la comprobacion de ruido de la etapa analogica.

## Consulta de referencias

El menu LOCUTORES sustituye a DIAGNOSTICO. En la seleccion, UP/DN cambia de locutor y se muestran estado, cantidad de tomas y coeficientes por toma. OK abre TOMA 1/3 unicamente si hay un perfil registrado. En la consulta, UP/DN recorre las tres tomas del mismo locutor y BACK vuelve a su seleccion; otro BACK regresa al menu. Un espacio sin perfil informa SIN REGISTRAR o el error de persistencia disponible.

Cada pantalla de toma muestra sus frames seleccionados y una grafica C01..C12 con escala fija -1..1. OK no tiene accion y conserva su recuadro sin texto con el fondo inactivo, igual que en un locutor sin perfil. `voxid_identify_read_prototype()` obtiene cada referencia mediante el mismo normalizador utilizado por la comparacion. Cada prototipo ya representa la media seleccionada de una toma; se conservan tres porque el criterio promedia sus distancias. La consulta no captura audio, no reinicia DSP ni escribe EEPROM. La grafica de captura conserva sus 13 MFCC crudos y su escala relativa.

## Modulos

| Modulo | Responsabilidad |
|---|---|
| `m24c32.c/.h` | Limites de memoria, direcciones de dos bytes, division por paginas y espera ACK |
| `voxid_storage.c/.h` | Formato, validacion, dos copias y carga inicial |
| `voxid_enroll.c/.h` | Sesion de tres tomas; publicacion RAM solo tras guardado verificado |
| `voxid_identify.c/.h` | Normalizacion, distancias y rechazo |
| `ui.c` | Registro, reintento de guardado, resultado y consulta de locutores |

No se reserva memoria dinamica ni se conserva la grabacion completa.

## Validacion pendiente en Ophyra

1. Actualizar el repositorio a esta revision, refrescar F5, Clean y Build Debug. Verificar que se compilan los nuevos modulos y HAL I2C/I2CEx; programar la placa.
2. Entrar a LOCUTORES y recorrer los cuatro espacios. Abrir un perfil con OK, recorrer sus tres tomas con UP/DN y volver con BACK al mismo locutor. Verificar que OK queda sin texto y con fondo inactivo en espacios vacios y dentro de las tomas.
3. Registrar tres tomas de un locutor. Comprobar que solo aparece BACK durante las capturas y que termina con `GUARDADO EN EEPROM`.
4. Reiniciar sin regrabar: el mismo espacio debe seguir ocupado. IDENTIFICAR con una pronunciacion nueva debe mostrar un resultado y su distancia.
5. Cancelar una regrabacion despues de una o dos tomas. Reiniciar y comprobar que sigue disponible el perfil anterior.
6. Registrar los otros locutores y comprobar nuevas tomas de cada uno y de personas no registradas. Anotar distancias, margenes y rechazos para calibrar.
7. Comprobar recuperacion y errores I2C en un banco de prueba controlado. Verificar tiempos y que no se pierden bloques de adquisicion; la resistencia a cortes electricos sigue pendiente de ensayo fisico.

No se marca la etapa como validada fisicamente hasta completar estas comprobaciones con la version compilada.

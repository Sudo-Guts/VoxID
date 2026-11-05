# Indicador de captura

| Estado | Circulos | Acciones |
|---|---|---|
| PREPARADO | Animados cada 180 ms, sin esperas bloqueantes. | OK inicia; BACK regresa. |
| ESCUCHANDO | Se dibujan una vez y permanecen fijos. | Cuenta de 2.5 a 0.5 segundos; solo BACK es visible y permite cancelar. |
| RESULTADO | Se borran. | UP/DN revisa la toma. OK repite en identificacion; en registro acepta una toma valida o repite una insuficiente. BACK cancela. |
| ERROR DE CAPTURA | Se borran. | OK recaptura; BACK regresa. |
| ERROR AL GUARDAR | Se borran. | OK reintenta EEPROM con la tercera toma disponible; BACK cancela. |

La solicitud de una toma tiene prioridad sobre el resultado anterior al dibujar. La cuenta se reinicia en 2.5 segundos y cambia en pasos de 0.5 segundos, se pinta la pantalla de escucha y despues se habilita DMA.

Desde que la toma queda solicitada, se ocultan UP, DN y OK y se ignoran sus eventos. Esto incluye el intervalo anterior al arranque DMA en que ADC todavia conserva READY de la toma previa.

La grafica representa la media de frames seleccionados por energia. Si no se seleccionan frames, aparece SIN VOZ DETECTADA. Su escala es relativa al mayor coeficiente absoluto y no permite comparar amplitudes absolutas entre tomas.

Los botones de la pantalla representan los pulsadores fisicos. El menu principal ofrece IDENTIFICAR, REGISTRAR y LOCUTORES.

La comprobacion visual y temporal de esta revision en Ophyra sigue pendiente.


En registro se muestra LOCxx / TOMA n/3. OK acepta cada toma valida; tras las dos primeras inicia la siguiente captura y tras la tercera verifica el guardado en EEPROM y actualiza el perfil RAM. Una repeticion no avanza el contador. BACK descarta la sesion pendiente, conservando cualquier perfil anterior. Ver [etapa 3](STAGE3_ENROLLMENT.md).

En identificacion, UP/DN recorre resultado, grafica MFCC y estadisticas ADC. El resultado puede aceptar un locutor, rechazarlo por distancia o ambiguedad, indicar ausencia de perfiles o voz insuficiente. Ver [etapa 4](STAGE4_STORAGE_IDENTIFY.md).

## Consulta de locutores

LOCUTORES sustituye a DIAGNOSTICO y tiene dos niveles:

| Pantalla | UP/DN | OK | BACK |
|---|---|---|---|
| Seleccion de locutor | Cambia entre los cuatro espacios, con retorno circular. | Abre TOMA 1/3 si el perfil esta registrado; inactivo si no hay perfil. | Regresa al menu principal. |
| Consulta de tomas | Cambia entre TOMA 1/3, 2/3 y 3/3, con retorno circular. | Inactivo. | Regresa al mismo locutor en la seleccion. |

La seleccion muestra estado, cantidad de tomas y 13 MFCC por toma. Un espacio sin perfil muestra SIN REGISTRAR y TOMAS: 0/3. El estado de persistencia pertenece solo al locutor seleccionado. Si sus registros son incompatibles o invalidos, muestra PERFIL NO UTILIZABLE y REGISTRA TRES TOMAS; un fallo I2C se informa por separado. La consulta no verifica de nuevo el bus I2C.

Al abrir un perfil se muestra una sola referencia por pantalla, identificada por locutor y numero de toma. Se informa la cantidad de frames seleccionados por energia que la originaron. La grafica presenta C01..C12 normalizados, con escala fija -1..1, barras positivas moradas y negativas coral. Los datos se obtienen mediante `voxid_identify_read_prototype()`, que usa el mismo normalizador de la comparacion.

Los botones sin accion conservan su recuadro, sin texto y con el fondo inactivo `UI_PANEL` de la paleta existente. En particular, OK queda inactivo en espacios vacios y al consultar las tomas. Durante la captura se conserva la regla previa: solo aparece BACK.

En la placa se deben comprobar ambos niveles, navegacion circular, BACK sin perder el locutor, apertura desde TOMA 1/3 y bloqueo de OK inactivo.

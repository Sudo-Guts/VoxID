# Indicador de captura

| Estado | Círculos | Acciones |
|---|---|---|
| PREPARADO | Animados cada 180 ms, sin esperas bloqueantes. | OK inicia; BACK regresa. |
| ESCUCHANDO | Se dibujan una vez y permanecen fijos. | Cuenta de 3 a 1; solo BACK es visible y permite cancelar. |
| RESULTADO | Se borran. | UP/DN revisa la toma. OK repite en identificación; en registro acepta una toma válida o repite una insuficiente. BACK cancela. |
| ERROR DE CAPTURA | Se borran. | OK recaptura; BACK regresa. |
| ERROR AL GUARDAR | Se borran. | OK reintenta EEPROM con la tercera toma disponible; BACK cancela. |

La solicitud de una toma tiene prioridad sobre el resultado anterior al dibujar. La cuenta se reinicia en tres segundos, se pinta la pantalla de escucha y después se habilita DMA.

Desde que la toma queda solicitada, se ocultan UP, DN y OK y se ignoran sus eventos. Esto incluye el intervalo anterior al arranque DMA en que ADC todavia conserva READY de la toma previa.

La gráfica representa la media de frames seleccionados por energía. Si no se seleccionan frames, aparece SIN VOZ DETECTADA. Su escala es relativa al mayor coeficiente absoluto y no permite comparar amplitudes absolutas entre tomas.

Los botones de la pantalla representan los pulsadores físicos. El menú principal ofrece IDENTIFICAR, REGISTRAR y LOCUTORES.

Las pruebas en PC comprueban las transiciones y que los círculos no se redibujen durante la captura. La comprobación visual y temporal en Ophyra sigue pendiente.


En registro se muestra LOCxx / TOMA n/3. OK acepta cada toma válida; tras las dos primeras inicia la siguiente captura y tras la tercera verifica el guardado en EEPROM y actualiza el perfil RAM. Una repetición no avanza el contador. BACK descarta la sesión pendiente, conservando cualquier perfil anterior. Ver [etapa 3](STAGE3_ENROLLMENT.md).

En identificación, UP/DN recorre resultado, gráfica MFCC y estadísticas ADC. El resultado puede aceptar un locutor, rechazarlo por distancia o ambigüedad, indicar ausencia de perfiles o voz insuficiente. Ver [etapa 4](STAGE4_STORAGE_IDENTIFY.md).

## Consulta de locutores

LOCUTORES sustituye a DIAGNOSTICO y tiene dos niveles:

| Pantalla | UP/DN | OK | BACK |
|---|---|---|---|
| Selección de locutor | Cambia entre los cuatro espacios, con retorno circular. | Abre TOMA 1/3 si el perfil está registrado; inactivo si no hay perfil. | Regresa al menú principal. |
| Consulta de tomas | Cambia entre TOMA 1/3, 2/3 y 3/3, con retorno circular. | Inactivo. | Regresa al mismo locutor en la selección. |

La selección muestra estado, cantidad de tomas y 13 MFCC por toma. Un espacio sin perfil muestra SIN REGISTRAR y TOMAS: 0/3. Si el estado de persistencia informa un fallo de lectura, datos dañados o falta de inicialización, se muestra ese estado. La consulta no verifica de nuevo el bus I2C.

Al abrir un perfil se muestra una sola referencia por pantalla, identificada por locutor y número de toma. Se informa la cantidad de frames selecciónados por energía que la originaron. La gráfica presenta C01..C12 normalizados, con escala fija -1..1, barras positivas moradas y negativas coral. Los datos se obtienen mediante `voxid_identify_read_prototype()`, que usa el mismo normalizador de la comparación.

Los botones sin acción conservan su recuadro, sin texto y con el fondo inactivo `UI_PANEL` de la paleta existente. En particular, OK queda inactivo en espacios vacíos y al consultar las tomas. Durante la captura se conserva la regla previa: solo aparece BACK.

Las pruebas comprueban ambos niveles, navegación circular, BACK sin perder el locutor, apertura desde TOMA 1/3, bloqueo de OK inactivo, colores de botones, barras con vectores conocidos y ausencia de I2C al consultar. El dibujo se revisó con UI y GFX reales sobre memoria de PC; falta revisar esta disposición en la pantalla física.

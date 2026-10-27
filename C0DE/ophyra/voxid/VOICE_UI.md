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

Los botones de la pantalla representan los pulsadores físicos. El firmware conserva el menú sintético de diagnóstico.

Las pruebas en PC comprueban las transiciones y que los círculos no se redibujen durante la captura. La comprobación visual y temporal en Ophyra sigue pendiente.


En registro se muestra LOCxx / TOMA n/3. OK acepta cada toma válida; tras las dos primeras inicia la siguiente captura y tras la tercera verifica el guardado en EEPROM y actualiza el perfil RAM. Una repetición no avanza el contador. BACK descarta la sesión pendiente, conservando cualquier perfil anterior. Ver [etapa 3](STAGE3_ENROLLMENT.md).

En identificación, UP/DN recorre resultado, gráfica MFCC y estadísticas ADC. El resultado puede aceptar un locutor, rechazarlo por distancia o ambigüedad, indicar ausencia de perfiles o voz insuficiente. Ver [etapa 4](STAGE4_STORAGE_IDENTIFY.md).

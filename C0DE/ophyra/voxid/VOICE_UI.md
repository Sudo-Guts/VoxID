# Indicador de captura

| Estado | Círculos | Acciones |
|---|---|---|
| PREPARADO | Animados cada 180 ms, sin esperas bloqueantes. | OK inicia; BACK regresa. |
| ESCUCHANDO | Se dibujan una vez y permanecen fijos. | Cuenta de 3 a 1; solo BACK es visible y permite cancelar. |
| RESULTADO | Se borran. | OK repite; UP/DN cambia entre resumen y media MFCC; BACK regresa. |
| ERROR | Se borran. | OK reintenta; BACK regresa. |

La solicitud de una toma tiene prioridad sobre el resultado anterior al dibujar. La cuenta se reinicia en tres segundos, se pinta la pantalla de escucha y después se habilita DMA.

Desde que la toma queda solicitada, se ocultan UP, DN y OK y se ignoran sus eventos. Esto incluye el intervalo anterior al arranque DMA en que ADC todavia conserva READY de la toma previa.

La gráfica representa la media de frames seleccionados por energía. Si no se seleccionan frames, aparece SIN VOZ DETECTADA. Su escala es relativa al mayor coeficiente absoluto y no permite comparar amplitudes absolutas entre tomas.

Los botones de la pantalla representan los pulsadores físicos. El firmware conserva el menú sintético de diagnóstico.

Las pruebas en PC comprueban las transiciones y que los círculos no se redibujen durante la captura. La comprobación visual y temporal en Ophyra sigue pendiente.


# Prueba MFCC y pantallas de voz (Ophyra)

Se reutilizan los algoritmos DSP compartidos y `voxid_dsp.c`; no se habilitan ADC1, TIM2 ni DMA.

## Prueba funcional

Desde **DIAGNOSTICO**, pulsar OK para ejecutar la prueba de 440 Hz / 16 kHz.
Se generan **720 muestras** PCM de senoide y se exige `MFCC_READY` exactamente en
las muestras **400, 560 y 720**. Se verifican **3 frames** y **13 coeficientes finitos
por frame**. UP y DOWN recorren cinco paginas: resumen, forma de onda PCM de 720 muestras, grafica de los 13 MFCC, coeficientes C00-C06 y C07-C12;
OK repite la prueba y BACK vuelve a Diagnostico. Los coeficientes corresponden
al tercer frame y se muestran en decimal fijo, sin `_printf_float`.

## Interfaz de captura y registro

- **IDENTIFICAR → OK:** pantalla preparatoria de adquisicion.
- **REGISTRAR → UP/DOWN:** seleccion de uno de **4 espacios de locutor**; OK abre
  su pantalla preparatoria.
- Ambas pantallas indican **ADC NO VINCULADO** y **SIN CAPTURA REAL**.
- BACK regresa a la pantalla anterior.

Los cuatro espacios **no contienen perfiles almacenados**, y no se afirma que un
locutor haya sido reconocido o registrado. No se implementan nombres, persistencia,
clasificador ni toma de muestras en esta etapa.

## Verificacion

En STM32CubeIDE: F5, Clean, Build Debug, programar placa. Validar navegacion
y `3 FRAMES: OK` en la prueba. El resultado de inicializacion ya fue observado
en hardware, pero la compilacion y prueba fisica de esta version estan pendientes.

## Footer con mini botones

Cuatro etiquetas graficas redondeadas de 32 x 12 px se distribuyen en toda
la anchura de la pantalla: `UP` (izquierda), `DN`, `OK` y `BACK` (derecha).
Se dibujan atenuadas cuando la accion no esta disponible en la pantalla.
No son botones tactiles: corresponden a los pulsadores fisicos de Ophyra.

## Graficas

- **ONDA SINTETICA**: envolvente minimo-maximo por columna de las 720 muestras
  sinteticas, reducidas a 129 columnas para conservar los picos. No usa ADC.
- **GRAFICA MFCC**: 13 barras sobre un eje cero, color morado para
  coeficientes positivos y coral para negativos. La escala vertical es
  relativa al valor absoluto maximo del ultimo vector: NO compara
  magnitudes absolutas entre distintas grabaciones.
- Los coeficientes numericos y el resumen de 3 frames permanecen disponibles.

No se reservan 720 muestras adicionales; la UI solo consulta el buffer estatico
que la prueba ya utiliza. Las graficas se redibujan al cambiar de pagina.

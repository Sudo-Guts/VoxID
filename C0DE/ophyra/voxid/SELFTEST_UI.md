# Prueba MFCC y pantallas de voz (Ophyra)

Se reutilizan los algoritmos DSP compartidos y `voxid_dsp.c`; no se habilitan ADC1, TIM2 ni DMA.

## Prueba funcional

Desde **DIAGNOSTICO**, pulsar OK para ejecutar la prueba de 440 Hz / 16 kHz.
Se generan **720 muestras** PCM de senoide y se exige `MFCC_READY` exactamente en
las muestras **400, 560 y 720**. Se verifican **3 frames** y **13 coeficientes finitos
por frame**. UP y DOWN recorren resumen, coeficientes C00-C06 y C07-C12;
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

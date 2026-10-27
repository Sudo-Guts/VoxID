# Etapa 2 — PCM y MFCC de voz real en STM32F407

## Alcance

La adquisicion ADC1 + TIM2 TRGO + DMA circular de 3 segundos
(48 000 muestras a 16 kHz) se conecta al pipeline MFCC existente.

**No hay reconocimiento, registro de perfiles ni EEPROM todavia.**
La palabra de referencia acordada es MARIPOSA, pero esta etapa
solo calcula caracteristicas acusticas; no verifica que se pronuncie.

## Flujo

1. ADC1_IN2 (PA2) produce muestras sin signo de 12 bits, 0...4095.
2. DMA circular llena 1280 muestras divididas en dos bloques de 640
   (40 ms por bloque), sin interrumpir por cada muestra.
3. `voxid_audio_task()` procesa fuera de IRQ el semibuffer completo.
4. `voxid_voice_process_block()` convierte ADC a PCM de 16 bits con
   signo y aplica un filtro DC recursivo de primer orden:
   `x = (adc - 2048)*16`,
   `y[n] = x[n] - x[n-1] + (32604/32768)*y[n-1]`.
   El resultado se satura a [-32768, 32767].
5. Cada muestra PCM pasa a `voxid_dsp_push()`. El extractor
   conserva internamente un frame de 400 muestras y un hop de 160.
6. Por cada `MFCC_READY`, se revisan los 13 coeficientes con
   `isfinite()`, se conserva el ultimo vector y se acumula su
   suma para obtener la media de la toma.
7. Al finalizar se verifica que hay 48 000 muestras y **298 frames**
   (`1 + (48000 - 400)/160 = 298`). No se acumula PCM en RAM.
8. Si el procesamiento falla, los coeficientes no son finitos o el
   DMA amenaza sobrescribir un bloque, se presenta error, no exito.

## Pantalla Ophyra

- PREPARADO: circulos pastel animados y OK para iniciar.
- ESCUCHANDO: 3 segundos, circulos fijos, cuenta regresiva,
  solo BACK para cancelar. El calculo MFCC ocurre en el bucle principal.
- RESULTADO pagina 1: MFCC OK, contador 298/298, ADC MIN/MAX
  y PROMEDIO; OK repite, BACK regresa.
- RESULTADO pagina 2: grafica de 13 coeficientes de la **media MFCC
  de toda la toma**, navegable con UP/DN solo al completar la captura.
- ERROR: no presenta valores falsos ni grafica inventada.

## Validacion sugerida

1. Clean/Build desde STM32CubeIDE tras refrescar el proyecto.
2. Confirmar que los modulos nuevos `voxid_voice.c/.h` compilan.
3. Con tension DC controlada cerca de 1.65 V en PA2 y GND comun,
   realizar una toma. Deben obtenerse 298 frames finitos incluso
   con silencio, pero eso **no prueba presencia de voz**.
4. Repetir con la etapa analogica acondicionada (0...3.3 V)
   y pronunciar MARIPOSA, comprobando que los valores MFCC cambian.
5. Comprobar que las 75 mitades DMA se consumen sin desbordamientos.
   Si aparece ERROR DE CAPTURA, medir el tiempo consumido por
   MFCC frente a los 40 ms por semibuffer y optimizar antes de
   implementar perfiles.
6. Repetir el autodiagnostico sintetico MFCC de 440 Hz para
   verificar que sigue independiente de la voz real.

## Limites y siguiente etapa

Se han acumulado estadisticas sobre **todos** los frames de los 3 s,
incluyendo segmentos silenciosos. La media obtenida es diagnostica
y **todavia no es un prototipo de locutor apto para EEPROM**.
En la siguiente etapa seleccionaremos frames con voz mediante
energia, haremos tres tomas de MARIPOSA y calcularemos prototipos.

La captura fisica y el rendimiento continuo del DSP deben validarse
en la placa. El codigo publicado no equivale a una prueba de hardware.

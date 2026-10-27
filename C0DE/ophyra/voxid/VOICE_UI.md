# Indicador de voz pastel — VoxID Ophyra

## Comportamiento confirmado y cambios en el firmware

La pantalla ST7735 (160 x 128) presenta tres circulos concentricos
sobre un fondo claro marfil-lavanda. Los anillos se dibujan desde
`Core/Src/ui_voice.c` con `gfx_draw_circle()`, y cada fase se
actualiza a intervalos de 180 ms desde `ui_task()`.

### Estados de interfaz

- **PREPARADO:** circulos pastel estaticos, texto `OK: HABLAR 3 SEG`.
  Footer: **OK y BACK**.
- **ESCUCHANDO:** los circulos laten durante los **3 segundos** de
  adquisicion ADC1/TIM2/DMA de MARIPOSA. Cuenta regresiva 3, 2, 1.
  Footer: **BACK habilitado** y **OK oculto/deshabilitado**;
  su fondo sigue visible, como en el resto de VoxID.
- **RESULTADO:** se eliminan por completo anillos y cuenta regresiva
  antes de dibujar el resumen `3 SEGUNDOS: OK`, MIN, MAX y PROMEDIO.
  La pantalla se mantiene fija hasta la siguiente accion del usuario.
  Footer: **OK para repetir** o **BACK para regresar**.
- **ERROR:** tarjeta estatica con mensaje de error, sin circulos;
  footer con OK para repetir y BACK para regresar.

La UI deja de redibujar circulos al salir del estado ESCUCHANDO.
Se incluye el estado `voice_capture_pending` en la logica del footer:
evita mostrar OK activo entre pulsar el boton e iniciar realmente el
DMA, mientras se prepara la TFT.

### Nota tecnica

La animacion y la lectura del ADC no se ejecutan en la misma IRQ.
El DMA recoge muestras en segundo plano; el bucle principal procesa
los bloques y actualiza la TFT. BACK cancela una captura activa.

**No es aun reconocimiento de hablantes:** la adquisicion real dura
3 segundos (48 000 muestras), pero todavia no se calcula MFCC con
ellas ni se almacenan perfiles en EEPROM. La prueba MFCC sintetica
conserva sus pantallas.

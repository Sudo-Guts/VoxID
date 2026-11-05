# Adquisicion ADC1, TIM2 y DMA

La etapa 1 implementa una toma de 40 000 conversiones a 16 kHz. La etapa 2 procesa cada bloque mientras llega.

- PA2: ADC1_IN2, 12 bits, disparo ascendente de TIM2 TRGO.
- TIM2: reloj de 84 MHz, PSC 0 y ARR 5249.
- DMA2 Stream0 Channel0: circular, incrementa memoria y transfiere halfwords.
- Buffer estatico de 1600 muestras, dividido en dos mitades de 800.
- Cada mitad representa 50 ms; 50 callbacks completan la toma.
- La ISR solo notifica bloques o un error y detiene TIM2 al terminar.
- El bucle principal calcula estadisticas, convierte PCM y extrae MFCC.
- Una sobreescritura o fallo descarta la toma y detiene los perifericos. El error del callback se atiende fuera de IRQ.
- BACK cancela y descarta los resultados.

La interfaz se dibuja antes de habilitar DMA. Los circulos se animan en PREPARADO, quedan fijos durante la escucha y desaparecen en RESULTADO/ERROR. OK solo inicia o repite cuando no hay captura en curso.

La configuracion C y `voxid.ioc` deben conservar DMA y el orden de inicializacion GPIO, DMA, ADC1, SPI1, TIM2, I2C1. La EEPROM solo se consulta al iniciar y al guardar un perfil, con adquisicion detenida.

La validacion fisica de esta version sigue pendiente. Consultar [la lista de comprobaciones](Validacion_etapas_1_2.md).


La duracion se expresa en milisegundos en voxid_config.h. Se usan 50 bloques de 800 muestras para detener TIM2 exactamente al alcanzar 40000 conversiones; no se redondea a una fraccion de bloque. El buffer DMA pasa de 2560 a 3200 bytes.

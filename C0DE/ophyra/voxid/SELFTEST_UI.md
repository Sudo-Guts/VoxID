# Autoprueba sintética MFCC

DIAGNOSTICO permite ejecutar una señal sintética de 440 Hz, muestreada a 16 kHz. Se procesan 720 muestras y se exige MFCC_READY en 400, 560 y 720, con tres frames y trece coeficientes finitos por frame.

UP/DN recorre resumen, gráfica y dos páginas de coeficientes. OK repite y BACK vuelve a diagnóstico. La gráfica usa el último vector y una escala relativa, sin adquisición ADC.

El extractor compartido se reinicia al iniciar la autoprueba. La navegación mantiene la captura y esta prueba en pantallas distintas para impedir su ejecución simultánea.

IDENTIFICAR y REGISTRAR ya tienen captura ADC con DMA y MFCC real; los cuatro espacios de registro admiten tres prototipos por locutor, persistidos en EEPROM y recuperados en RAM. Ver [etapa 3](STAGE3_ENROLLMENT.md) y [etapa 4](STAGE4_STORAGE_IDENTIFY.md). Ver STAGE2_REAL_MFCC.md y VOICE_UI.md.

Las regresiones de PC comprueban el retorno a esta pantalla tras usar adquisición. La prueba funcional física de la versión actual sigue pendiente.


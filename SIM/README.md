# Simulaciones de VoxID

La referencia de componentes es `PCB/voxid.kicad_sch`, commit `001fb24`. Esta corrección actualiza los experimentos SPICE y conserva el diseño físico.

## Experimentos con valores de placa

| Archivo | Análisis |
|---|---|
| `spice/Filtro_PCB_AC.cir` | AC y transitorio nominales, cortes y ancho de banda. |
| `spice/Filtro_Real_MonteCarlo.cir` | 100 iteraciones LTspice, R ±1 % y C ±5 %. |
| `spice/Filtro_Real_Ruido.cir` | Ruido de salida entre 100 Hz y 6 kHz. |

Los tres usan `spice/TL072.lib` y alimentación de ±12 V.

| Sección | Resistencias | Capacitores |
|---|---|---|
| HPF 1 | R1 = 1.2 kΩ; R2 = 8.2 kΩ | C1 = C2 = 470 nF |
| HPF 2 | R3 = 3.3 kΩ; R4 = 3.9 kΩ | C3 = C4 = 470 nF |
| LPF 1 | R5 = 2.2 kΩ; R6 = 6.8 kΩ | C6 = 22 nF; C7 = 2.2 nF |
| LPF 2 | R10 = 820 Ω; R14 = 3.9 kΩ | C8 = 22 nF; C9 = 10 nF |

Son ensayos de cuatro secciones en cascada: se supone la conexión de salida HPF a entrada LPF para el experimento. No son una exportación completa de las conexiones de la placa. No incluyen micrófono, preamplificación, offset, protección ADC, carga real ni parásitos.

Los componentes comerciales modifican la respuesta respecto del diseño ideal. Ejecutar nuevamente los análisis para conocer cortes, ganancias y ruido de esta revisión. No se presentan resultados nuevos sin ejecutar el simulador.

## Verificación de correspondencia

Desde la raíz:

```bash
python3 SIM/tests/comprobar_valores.py
```

El comprobador lee los componentes instanciados en KiCad y coteja los dieciséis valores nominales con los tres netlists, también dentro de `mc()`. Comprueba valores; no ejecuta SPICE ni reemplaza ERC/DRC.

Abrir los experimentos en LTspice y ejecutar cada análisis. Las tolerancias del Monte Carlo representan el supuesto indicado; el ancho de banda se calcula como FH - FL.

## Referencias históricas

- `spice/Filtro_Referencia_Ruido.cir` conserva el circuito anterior de comparación.
- `spice/Filtro_KiCad_ngspice.cir` corresponde a una propuesta previa con otros valores RC.
- Los proyectos de `psim/` se conservan como antecedentes. No se declara que estén sincronizados con la PCB actual.
- Los valores antes anotados, 2.6088 y 2.6253 µV RMS, pertenecen a experimentos previos. No describen los netlists actualizados ni una medición física.
- El modelo TL072 y las tolerancias supuestas limitan el alcance de los experimentos. Las mediciones reales, ERC/DRC y validación de fabricación siguen pendientes.

Las salidas automáticas RAW, LOG y resultados AC no deben almacenarse como código fuente.

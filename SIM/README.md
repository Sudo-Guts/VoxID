# Simulaciones de VoxID

Los esquemáticos editables de PSIM se guardan en `psim/`. Los circuitos y análisis independientes de SPICE se guardan en `spice/`.

```text
SIM/
├── README.md
├── psim/
│   ├── Filtro_Imaginario.psimsch
│   ├── Filtro_Real.psimsch
│   └── Filtro_Real_MonteCarlo.psimsch
└── spice/
    ├── Filtro_KiCad_ngspice.cir
    ├── Filtro_Real_MonteCarlo.cir
    ├── Filtro_Real_Ruido.cir
    ├── Filtro_Referencia_Ruido.cir
    └── TL072.lib
```

## Uso

- **PSIM:** `Filtro_Imaginario.psimsch` contiene los filtros ideales; `Filtro_Real.psimsch` utiliza los macromodelos SPICE TL072. `Filtro_Real_MonteCarlo.psimsch` se conserva como copia de trabajo; el motor Monte Carlo nativo de PSIM no admite los bloques `SPICE_SUB_NETLIST_BLOCK` de esta versión.
- **LTspice:** abrir directamente `Filtro_Real_MonteCarlo.cir`, `Filtro_Real_Ruido.cir` o `Filtro_Referencia_Ruido.cir` y ejecutar la simulación. Estos netlists referencian `TL072.lib` en su misma carpeta. La alimentación de los operacionales es de ±12 V.
- **ngspice (referencia anterior):** `Filtro_KiCad_ngspice.cir` contiene un diseño previo con valores RC distintos y comandos `.control`. No confundirlo con el diseño *low noise* vigente.
- El netlist `psim/Filtro_Real.cir` se genera desde PSIM y está ignorado por Git. Para mantener las simulaciones independientes, editar únicamente los archivos fuente del subdirectorio `spice/`.
- Los resultados automáticos `.raw`, `.log`, `.fra`, `.db` y similares se excluyen de Git mediante el `.gitignore`.

## Experimentos conservados

| Experimento | Configuración |
|---|---|
| Monte Carlo | 100 iteraciones, resistencias ±1 %, capacitores ±5 %, análisis AC |
| Ruido, filtro *low noise* | 2.6088 µV RMS, integrado de 100 Hz a 6 kHz |
| Ruido, filtro de referencia | 2.6253 µV RMS, integrado de 100 Hz a 6 kHz |

Los resultados de ruido corresponden a simulaciones LTspice con el macromodelo TL072 disponible; no son mediciones de la placa ni validan el ruido físico del integrado. Las tolerancias Monte Carlo representan una distribución uniforme supuesta. El archivo de Monte Carlo incluye mediciones de cortes `FL` y `FH`; para calcular el ancho de banda en hercios, usar `FH - FL`.

Los esquemáticos de PSIM pueden guardar referencias de modelos y bibliotecas dependientes de la instalación local. No se ha verificado su portabilidad a otros equipos.

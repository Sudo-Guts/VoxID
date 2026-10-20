# VoxID

Sistema embebido de identificación de hablante basado en adquisición de voz, acondicionamiento analógico y extracción de características MFCC.

**Estado:** recuperación y estructuración de la primera versión de trabajo a partir del respaldo `VoxID_B1` (11 de octubre de 2026). Este repositorio **no** representa una implementación terminada ni hardware validado.

## Arquitectura prevista

```text
Micrófono electret
       |
       v
Preamplificador
       |
       v
HPF Butterworth de 4.º orden (100 Hz)
       |
       v
LPF Butterworth de 4.º orden (6 kHz)
       |
       v
Offset y adaptación al ADC
       |
       v
Adquisición y procesamiento digital
       |
       v
Extracción de MFCC
       |
       v
Identificación del hablante
```

**Plataforma digital por confirmar.** El README del respaldo indica `STM32F407` como ADC y plataforma de procesamiento; en conversaciones previas también se consideró una integración con `Ophyra`. Esta decisión no está cerrada en el presente borrador.

## Organización

```text
VoxID/
├── .gitignore
├── README.md
├── C0DE/
│   ├── Makefile
│   ├── inc/
│   ├── src/
│   ├── tests/
│   └── out/
├── DOC/
│   └── CheckList.txt
├── PCB/
│   ├── voxid.kicad_pro
│   ├── voxid.kicad_sch
│   └── voxid.kicad_pcb
└── SIM/
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

- `C0DE/`: código, cabeceras, pruebas y construcción GNU Make.
- `DOC/`: seguimiento de tareas y documentación del proyecto.
- `PCB/`: proyecto KiCad editable; la preparación de archivos de fabricación sigue pendiente de validación.
- `SIM/psim/`: esquemáticos PSIM de filtros ideales y con TL072.
- `SIM/spice/`: circuitos de análisis LTspice/ngspice y biblioteca TL072. Consultar [SIM/README.md](SIM/README.md).

## Estado de los archivos

- **Disponibles:** esquemático y PCB de KiCad; proyectos PSIM; netlists LTspice para Monte Carlo y comparación de ruido.
- **Simulaciones realizadas:** respuesta AC de los filtros, Monte Carlo de 100 iteraciones y ruido de salida integrado de 100 Hz a 6 kHz mediante el macromodelo TL072. Son resultados simulados, no validación física.
- **No verificado:** ERC/DRC de KiCad, lista de materiales definitiva, fabricación de la PCB, mediciones físicas, ni portabilidad de los proyectos PSIM a otras instalaciones.
- **Pendiente:** interfaz definitiva con el ADC, integración digital, procesamiento MFCC e identificación del hablante.

## Convenciones del equipo

1. `Makefile` siempre dentro de `C0DE/`.
2. `out/` conserva solamente `.gitkeep` en Git; se excluyen los binarios de compilación.
3. Los archivos de KiCad se mantienen bajo `PCB/`; los archivos de fabricación se incorporarán después de su validación.
4. `DOC/CheckList.txt` registra actividades y comprobaciones con `[ ]` pendiente, `[-]` en progreso y `[x]` verificado.
5. El respaldo original `VoxID_B1` permanece intacto en Google Drive, incluyendo historial y respaldos automáticos de KiCad.

## Cómo continuar

Consultar `DOC/CheckList.txt`. No se proporciona todavía un procedimiento de compilación ni de programación, porque la plataforma final y la toolchain no están confirmadas.

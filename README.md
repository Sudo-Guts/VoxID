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
│   └── out/
│       └── .gitkeep
├── DOC/
│   ├── CheckList.txt
│   └── Microfono.csv
├── PCB/
│   └── Microfono/
│       ├── Microfono.kicad_pro
│       ├── Microfono.kicad_sch
│       ├── Microfono.kicad_pcb
│       └── fabrication/
│           ├── Microfono-PTH.drl
│           └── Microfono-NPTH.drl
└── SIM/
    ├── Filtro.psimsch
    ├── Filtro.py
    ├── Filtro(AC-Sweep).fra
    ├── Filtro_TL072.psimsch
    ├── Filtro_TL072.py
    ├── Filtro_TL072.cir
    ├── Filtro_TL072(AC-Sweep).fra
    ├── TL071
    └── TL072
```

- `C0DE/`: código embebido (pendiente), cabeceras y sistema de construcción GNU Make.
- `DOC/`: seguimiento del desarrollo y lista de materiales inicial exportada de KiCad.
- `PCB/Microfono/`: diseño editable de la placa; una carpeta independiente por PCB.
- `PCB/Microfono/fabrication/`: por ahora únicamente archivos de perforación. **No constituye un juego completo de Gerbers**.
- `SIM/`: proyectos y scripts PSIM, modelos y netlist SPICE, resultados AC Sweep. Se mantienen juntos para reducir el riesgo de romper rutas existentes.

## Estado de los archivos

- **Disponibles:** esquemático y placa KiCad, BOM preliminar, simulaciones y resultados AC Sweep preexistentes.
- **No verificado en esta migración:** apertura de proyectos KiCad, ERC/DRC, coherencia BOM-esquemático-PCB y ejecución reproducible de PSIM/SPICE.
- **Pendiente:** toolchain y firmware, interfaz definitiva con el ADC, procesamiento MFCC e identificación del hablante.
- **Atención:** el netlist SPICE contiene una referencia de ruta absoluta de Windows; se deberá convertir a una ruta portable y probarse en su simulador antes de reorganizar `SIM/`.

## Convenciones del equipo

1. `Makefile` siempre dentro de `C0DE/`.
2. `out/` conserva solamente `.gitkeep` en Git; se excluyen los binarios de compilación.
3. Cada PCB se almacena en una subcarpeta propia y sus salidas de fabricación en `fabrication/`.
4. `DOC/CheckList.txt` registra actividades y comprobaciones con `[ ]` pendiente, `[-]` en progreso y `[x]` verificado.
5. El respaldo original `VoxID_B1` permanece intacto en Google Drive, incluyendo historial y respaldos automáticos de KiCad.

## Cómo continuar

Consultar `DOC/CheckList.txt`. No se proporciona todavía un procedimiento de compilación ni de programación, porque la plataforma final y la toolchain no están confirmadas.

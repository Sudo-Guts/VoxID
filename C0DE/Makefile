# =============================================================================
#  Proyecto : VoxID
#  Archivo  : C0DE/Makefile
#  Autor    : Gustavo Ruiz Luis
# -----------------------------------------------------------------------------
#  Compilacion portable y pruebas en PC. El firmware usa STM32CubeIDE.
# =============================================================================

CC = gcc
OBJDUMP = objdump
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS += -Iinc
LDLIBS += -lm

SRC := src
OUT := out
CORE := ophyra/voxid/Core

FUENTES := $(wildcard $(SRC)/*.c)
FUENTES_DSP := $(filter-out $(SRC)/voxid.c,$(FUENTES))
OBJETOS := $(patsubst $(SRC)/%.c,$(OUT)/%.o,$(FUENTES))
CABECERAS := $(wildcard inc/*.h) $(wildcard $(CORE)/Inc/*.h) $(wildcard tests/support/*.h)
FUENTES_VOZ := $(CORE)/Src/voxid_dsp.c $(CORE)/Src/voxid_voice.c
FUENTES_AUDIO := $(FUENTES_VOZ) $(CORE)/Src/voxid_audio.c tests/support/mock_hal.c
FUENTES_REGISTRO := $(CORE)/Src/voxid_enroll.c
FUENTES_PERFILES := $(FUENTES_REGISTRO) $(CORE)/Src/voxid_storage.c $(CORE)/Src/m24c32.c tests/support/mock_eeprom.c
FUENTES_IDENTIFICAR := $(CORE)/Src/voxid_identify.c
FUENTES_UI := $(FUENTES_AUDIO) $(FUENTES_PERFILES) $(FUENTES_IDENTIFICAR) $(CORE)/Src/ui.c $(CORE)/Src/ui_voice.c $(CORE)/Src/buttons.c $(CORE)/Src/voxid_selftest.c
INCLUDES_PLACA := -Itests/support -I$(CORE)/Inc

PROGRAMA := $(OUT)/voxid.exe
ASM := $(OUT)/voxid.asm
MAP := $(OUT)/voxid.map
PRUEBAS := $(basename $(notdir $(wildcard tests/test_*.c)))

# Se conserva: make test test_frame.c. Sin argumento se ejecutan todas.
ifneq ($(filter test,$(MAKECMDGOALS)),)
PRUEBA_SOLICITADA := $(filter-out test all run clean,$(MAKECMDGOALS))
ifneq ($(words $(PRUEBA_SOLICITADA)),0)
ifneq ($(words $(PRUEBA_SOLICITADA)),1)
$(error Uso: make test [test_nombre.c])
endif
PRUEBA_ELEGIDA := $(basename $(notdir $(PRUEBA_SOLICITADA)))
ifeq ($(filter $(PRUEBA_ELEGIDA),$(PRUEBAS)),)
$(error No existe la prueba $(PRUEBA_SOLICITADA))
endif
.PHONY: $(PRUEBA_SOLICITADA)
$(PRUEBA_SOLICITADA):
endif
endif

PRUEBAS_ELEGIDAS := $(if $(PRUEBA_ELEGIDA),$(PRUEBA_ELEGIDA),$(PRUEBAS))
EJECUTAR_PRUEBAS := $(addprefix ejecutar-,$(PRUEBAS_ELEGIDAS))

.PHONY: all run clean test $(EJECUTAR_PRUEBAS)
.DEFAULT_GOAL := all

all: $(PROGRAMA) $(ASM)

$(OUT):
	mkdir $(OUT)

$(OUT)/%.o: $(SRC)/%.c $(wildcard inc/*.h) | $(OUT)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(PROGRAMA): $(OBJETOS)
	$(CC) $(CFLAGS) $^ -Wl,-Map,$(MAP) -o $@ $(LDLIBS)

$(ASM): $(PROGRAMA)
	$(OBJDUMP) -d $< > $@

run: all
ifeq ($(OS),Windows_NT)
	.\out\voxid.exe
else
	./$(PROGRAMA)
endif

$(OUT)/test_%.exe: tests/test_%.c $(FUENTES_DSP) $(CABECERAS) | $(OUT)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_gfx.exe: tests/test_gfx.c $(CORE)/Src/gfx.c $(CORE)/Src/font5x7.c $(CABECERAS) tests/support/stm32f4xx_hal.h | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_voice.exe: tests/test_voice.c $(FUENTES_DSP) $(FUENTES_VOZ) $(CABECERAS) | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_enroll.exe: tests/test_enroll.c $(FUENTES_DSP) $(FUENTES_AUDIO) $(FUENTES_PERFILES) $(CABECERAS) | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_storage.exe: tests/test_storage.c $(FUENTES_DSP) $(FUENTES_AUDIO) $(FUENTES_PERFILES) $(CABECERAS) | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_identify.exe: tests/test_identify.c $(FUENTES_DSP) $(FUENTES_AUDIO) $(FUENTES_PERFILES) $(FUENTES_IDENTIFICAR) $(CABECERAS) | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_audio.exe: tests/test_audio.c $(FUENTES_DSP) $(FUENTES_AUDIO) $(CABECERAS) tests/support/main.h | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(OUT)/test_ui.exe: tests/test_ui.c $(FUENTES_DSP) $(FUENTES_UI) $(CABECERAS) tests/support/main.h | $(OUT)
	$(CC) $(CPPFLAGS) $(INCLUDES_PLACA) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

test: $(EJECUTAR_PRUEBAS)

$(EJECUTAR_PRUEBAS): ejecutar-%: $(OUT)/%.exe
ifeq ($(OS),Windows_NT)
	.\$(subst /,\,$<)
else
	./$<
endif

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "if (Test-Path -LiteralPath '$(OUT)') { Remove-Item -LiteralPath '$(OUT)' -Recurse -Force -ErrorAction Stop }; New-Item -ItemType Directory -Path '$(OUT)' -Force -ErrorAction Stop | Out-Null; New-Item -ItemType File -Path '$(OUT)/.gitkeep' -Force -ErrorAction Stop | Out-Null"
else
	rm -rf "$(OUT)"
	mkdir -p "$(OUT)"
	touch "$(OUT)/.gitkeep"
endif

-include $(OBJETOS:.o=.d)


# =============================================================================
#	Proyecto : VoxID
#   Archivo  : C0DE/Makefile
#   Autor    : Gustavo Ruiz Luis
# =============================================================================
#	
# =============================================================================

# ============================================================
# VoxID - Sistema de Compilacion
# ============================================================

# Herramientas
CC 		:= gcc
OBJDUMP := objdump

# Opciones de compilacion
CFLAGS 	:= -std=c11 -Wall -Wextra -Wpedantic

# Directorios
SRC 	:= src
OUT 	:= out

# Archivos generados
OBJ 	:= $(OUT)/voxid.o
TARGET 	:= $(OUT)/voxid.exe
ASM 	:= $(OUT)/voxid.asm
MAP 	:= $(OUT)/voxid.map

# ============================================================
# Objetivos
# ============================================================

.PHONY: all run clean test
.DEFAULT_GOAL := all

all: $(TARGET) $(ASM)

# Compilacion
$(OBJ): $(SRC)/voxid.c | $(OUT)
	$(CC) $(CFLAGS) -c $< -o $@

# Enlazado
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -Wl,-Map,$(MAP) -o $@

# Desensamblado
$(ASM): $(TARGET)
	$(OBJDUMP) -d $< > $@

# Directorio de salida
$(OUT):
	mkdir $(OUT)

# ============================================================
# Ejecucion y Limpieza
# ============================================================

ifeq ($(OS),Windows_NT)
    RUN := .\out\voxid.exe
    CLEAN := powershell -NoProfile -Command "if (Test-Path -LiteralPath '$(OUT)') { Remove-Item -LiteralPath '$(OUT)' -Recurse -Force -ErrorAction Stop }; New-Item -ItemType Directory -Path '$(OUT)' -Force -ErrorAction Stop | Out-Null; New-Item -ItemType File -Path '$(OUT)/.gitkeep' -Force -ErrorAction Stop | Out-Null"
else
    RUN := ./$(TARGET)
    CLEAN := rm -rf "$(OUT)" && mkdir -p "$(OUT)" && touch "$(OUT)/.gitkeep"
endif


# Ejecutar el programa
run: all
	$(RUN)

# Eliminar archivos generados
clean:
	$(CLEAN)


# ============================================================
# Pruebas Unitarias
# ============================================================

# Activar el sistema de pruebas solo cuando se solicita "test".
ifneq ($(filter test,$(MAKECMDGOALS)),)

# Recuperar el nombre del archivo de prueba.
TEST_ARGS := $(filter-out test,$(MAKECMDGOALS))

# Exigir exactamente un archivo como argumento.
ifneq ($(words $(TEST_ARGS)),1)
    $(error Uso: mingw32-make test test_algo.c)
endif

TEST_NAME := $(firstword $(TEST_ARGS))

# Buscar primero la ruta indicada y despues el directorio tests/.
TEST_FILE := $(if $(wildcard $(TEST_NAME)),$(TEST_NAME),tests/$(TEST_NAME))

# Comprobar que exista el archivo.
ifeq ($(wildcard $(TEST_FILE)),)
    $(error No existe el archivo de prueba: $(TEST_FILE))
endif

# Identificar el modulo: test_frame.c -> frame.c.
TEST_MODULE := $(patsubst test_%,%,$(basename $(notdir $(TEST_FILE))))

# Seleccionar el archivo fuente correspondiente, si existe.
TEST_SRCS ?= $(wildcard $(SRC)/$(TEST_MODULE).c)

# Generar el nombre del ejecutable.
TEST_TARGET := $(OUT)/$(basename $(notdir $(TEST_FILE))).exe

# Aceptar el nombre del archivo como segundo objetivo.
ifneq ($(TEST_NAME),$(TEST_FILE))
.PHONY: $(TEST_NAME)
$(TEST_NAME):
endif

# Compilar la prueba y su modulo.
$(TEST_TARGET): $(TEST_FILE) $(TEST_SRCS) $(wildcard inc/*.h) | $(OUT)
	$(CC) $(CFLAGS) -Iinc $(TEST_FILE) $(TEST_SRCS) -o $@ $(LDLIBS)

# Ejecutar la prueba.
test: $(TEST_TARGET)
ifeq ($(OS),Windows_NT)
	.\$(subst /,\,$(TEST_TARGET))
else
	./$(TEST_TARGET)
endif

endif

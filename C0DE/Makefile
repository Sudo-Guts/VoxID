# =============================================================================
# Proyecto : VoxID
# Archivo  : C0DE/Makefile
# Autor    : Gustavo Ruiz Luis
# =============================================================================
#
#
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

.PHONY: all run clean
.DEFAULT_GOAL := all

all: $(TARGET) $(ASM)

# Compilacion
$(OBJ): $(SRC)/main.c | $(OUT)
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
    CLEAN := powershell -NoProfile -Command "Remove-Item -Force -ErrorAction SilentlyContinue 'out/voxid.o','out/voxid.exe','out/voxid.asm','out/voxid.map'"
else
    RUN := ./$(TARGET)
    CLEAN := rm -f $(OBJ) $(TARGET) $(ASM) $(MAP)
endif

# Ejecutar el programa
run: all
	$(RUN)

# Eliminar archivos generados
clean:
	$(CLEAN)

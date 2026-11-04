# =============================================================================
#  Proyecto : VoxID
#  Archivo  : C0DE/voxid/Makefile
#  Autor    : Gustavo Ruiz Luis
# -----------------------------------------------------------------------------
#  Compila y ejecuta las pruebas de PC sobre las fuentes del firmware.
#  STM32CubeIDE compila el firmware ARM desde Core y Drivers.
# =============================================================================

CC = gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic
CPPFLAGS += -I$(TEST_DIR)/support -I$(CORE)/Inc
LDLIBS += -lm

CORE := Core
TEST_DIR := tests
BUILD_DIR := $(TEST_DIR)/build

DSP_SOURCES := $(addprefix $(CORE)/Src/,frame.c window.c fft.c power.c mel.c log_mel.c dct.c mfcc.c)
VOICE_SOURCES := $(CORE)/Src/voxid_voice.c
AUDIO_SOURCES := $(VOICE_SOURCES) $(CORE)/Src/voxid_audio.c $(TEST_DIR)/support/mock_hal.c
PROFILE_SOURCES := $(addprefix $(CORE)/Src/,voxid_enroll.c voxid_storage.c m24c32.c) $(TEST_DIR)/support/mock_eeprom.c
IDENTIFY_SOURCES := $(CORE)/Src/voxid_identify.c
UI_SOURCES := $(AUDIO_SOURCES) $(PROFILE_SOURCES) $(IDENTIFY_SOURCES) $(addprefix $(CORE)/Src/,ui.c ui_voice.c buttons.c)
HEADERS := $(wildcard $(CORE)/Inc/*.h) $(wildcard $(TEST_DIR)/support/*.h)
TEST_NAMES := $(basename $(notdir $(wildcard $(TEST_DIR)/test_*.c)))
TEST_BINARIES := $(addprefix $(BUILD_DIR)/,$(addsuffix .exe,$(TEST_NAMES)))

# Se conserva: make test test_frame.c. Sin argumento se ejecutan todas.
ifneq ($(filter test,$(MAKECMDGOALS)),)
TEST_ARGUMENT := $(filter-out test all clean,$(MAKECMDGOALS))
ifneq ($(words $(TEST_ARGUMENT)),0)
ifneq ($(words $(TEST_ARGUMENT)),1)
$(error Uso: make test [test_nombre.c])
endif
SELECTED_TEST := $(basename $(notdir $(TEST_ARGUMENT)))
ifeq ($(filter $(SELECTED_TEST),$(TEST_NAMES)),)
$(error No existe la prueba $(TEST_ARGUMENT))
endif
.PHONY: $(TEST_ARGUMENT)
$(TEST_ARGUMENT):
endif
endif

SELECTED_TESTS := $(if $(SELECTED_TEST),$(SELECTED_TEST),$(TEST_NAMES))
RUN_TEST_TARGETS := $(addprefix run-,$(SELECTED_TESTS))

.PHONY: all clean test $(RUN_TEST_TARGETS)
.DEFAULT_GOAL := all

all: $(TEST_BINARIES)

$(BUILD_DIR):
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "New-Item -ItemType Directory -Path '$(BUILD_DIR)' -Force -ErrorAction Stop | Out-Null"
else
	mkdir -p "$(BUILD_DIR)"
endif

$(TEST_BINARIES): $(BUILD_DIR)/%.exe: $(TEST_DIR)/%.c $(HEADERS) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(filter %.c,$^) -o $@ $(LDLIBS)

$(filter-out $(BUILD_DIR)/test_gfx.exe,$(TEST_BINARIES)): $(DSP_SOURCES)
$(BUILD_DIR)/test_gfx.exe: $(CORE)/Src/gfx.c $(CORE)/Src/font5x7.c
$(BUILD_DIR)/test_voice.exe: $(VOICE_SOURCES)
$(BUILD_DIR)/test_audio.exe: $(AUDIO_SOURCES)
$(BUILD_DIR)/test_enroll.exe $(BUILD_DIR)/test_storage.exe: $(AUDIO_SOURCES) $(PROFILE_SOURCES)
$(BUILD_DIR)/test_identify.exe: $(AUDIO_SOURCES) $(PROFILE_SOURCES) $(IDENTIFY_SOURCES)
$(BUILD_DIR)/test_ui.exe: $(UI_SOURCES)

test: $(RUN_TEST_TARGETS)

$(RUN_TEST_TARGETS): run-%: $(BUILD_DIR)/%.exe
ifeq ($(OS),Windows_NT)
	.\$(subst /,\,$<)
else
	./$<
endif

clean:
ifeq ($(OS),Windows_NT)
	powershell -NoProfile -Command "if (Test-Path -LiteralPath '$(BUILD_DIR)') { Remove-Item -LiteralPath '$(BUILD_DIR)' -Recurse -Force -ErrorAction Stop }"
else
	rm -rf "$(BUILD_DIR)"
endif

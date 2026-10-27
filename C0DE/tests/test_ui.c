/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_ui.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Regresiones de navegacion, repeticion y circulos durante captura.
 *  Se ejecuta la UI real con dibujo y perifericos simulados.
 *
 * =============================================================================
 */

#include "main.h"
#include "buttons.h"
#include "gfx.h"
#include "ui.h"
#include "ui_voice.h"
#include "voxid_audio.h"
#include "voxid_dsp.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static char texto_pantalla[4096];
static uint32_t circulos_dibujados;
static bool botones_visibles[4];

static bool registrar_texto(const char *texto) {
    size_t disponibles = sizeof(texto_pantalla) - strlen(texto_pantalla) - 1U;
    strncat(texto_pantalla, texto, disponibles);
    return true;
}

bool gfx_fill_screen(uint16_t color) {
    (void)color;
    texto_pantalla[0] = '\0';

    for (uint8_t i = 0U; i < 4U; ++i) {
        botones_visibles[i] = false;
    }

    return true;
}

bool gfx_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    (void)color;
    assert(x + width <= 160U && y + height <= 128U);
    return true;
}

bool gfx_fill_round_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t color) {
    (void)radius;
    (void)color;
    assert(x >= 0 && y >= 0 && x + width <= 160 && y + height <= 128);

    if (y == 115 && x >= 4 && (x - 4) % 40 == 0) {
        botones_visibles[(x - 4) / 40] = true;
    }

    return true;
}

bool gfx_draw_circle(int16_t x, int16_t y, uint16_t radius, uint16_t color) {
    (void)color;
    assert(x - radius >= 0 && y - radius >= 0 && x + radius < 160 && y + radius < 128);
    ++circulos_dibujados;
    return true;
}

bool gfx_draw_text(int16_t x, int16_t y, const char *text, uint16_t color) {
    (void)x;
    (void)y;
    (void)color;
    return registrar_texto(text);
}

bool gfx_draw_text_scaled(int16_t x, int16_t y, const char *text, uint16_t color, uint8_t scale) {
    (void)scale;
    return gfx_draw_text(x, y, text, color);
}

bool gfx_draw_text_aligned(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale, gfx_align_t align) {
    (void)bg;
    (void)scale;
    (void)align;
    return gfx_draw_text(x, y, text, color);
}

static void encolar(uint16_t pin) {
    mock_tick += 50U;
    buttons_on_exti(pin, mock_tick);
}

static void pulsar(uint16_t pin) {
    encolar(pin);
    ui_task();
}

static void comprobar_escucha(void) {
    assert(voxid_audio_status() == VOXID_AUDIO_CAPTURING);
    assert(strstr(texto_pantalla, "ESCUCHANDO: 3s") != NULL);
    assert(strstr(texto_pantalla, "FRAMES:") == NULL);
    assert(strstr(texto_pantalla, "ERROR DE CAPTURA") == NULL);
    assert(!botones_visibles[0] && !botones_visibles[1] && !botones_visibles[2] && botones_visibles[3]);
}

int main(void) {
    mock_hal_reset();
    buttons_init();
    assert(voxid_dsp_init() && ui_init());
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_CAPTURE);
    assert(strstr(texto_pantalla, "OK: HABLAR 3 SEG") != NULL);

    uint32_t antes = circulos_dibujados;
    mock_tick += UI_VOICE_TICK_MS;
    ui_task();
    assert(circulos_dibujados == antes + 3U);

    pulsar(BTN_OK_Pin);
    comprobar_escucha();
    uint32_t circulos_fijos = circulos_dibujados;
    pulsar(BTN_OK_Pin);
    assert(circulos_dibujados == circulos_fijos);
    pulsar(BTN_UP_Pin);
    pulsar(BTN_DOWN_Pin);
    assert(circulos_dibujados == circulos_fijos);

    for (uint32_t b = 0U; b < VOXID_AUDIO_TOTAL_BLOCKS; ++b) {
        mock_dma_block(2048U);
        voxid_audio_task();
        ui_task();
    }

    assert(voxid_audio_status() == VOXID_AUDIO_READY);
    assert(circulos_dibujados == circulos_fijos);
    assert(strstr(texto_pantalla, "FRAMES: 298/298") != NULL);
    assert(strstr(texto_pantalla, "CON VOZ: 0") != NULL);
    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "SIN VOZ DETECTADA") != NULL);

    encolar(BTN_OK_Pin);
    encolar(BTN_DOWN_Pin);
    encolar(BTN_UP_Pin);
    ui_task();
    comprobar_escucha();
    assert(circulos_dibujados == circulos_fijos + 3U);
    pulsar(BTN_BACK_Pin);
    assert(ui_state() == UI_STATE_IDENTIFY);
    assert(!mock_dma_running && !mock_timer_running);

    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    HAL_ADC_ErrorCallback(&hadc1);
    voxid_audio_task();
    ui_task();
    assert(strstr(texto_pantalla, "ERROR DE CAPTURA") != NULL);
    pulsar(BTN_OK_Pin);
    comprobar_escucha();
    pulsar(BTN_BACK_Pin);
    pulsar(BTN_BACK_Pin);
    pulsar(BTN_DOWN_Pin);
    pulsar(BTN_DOWN_Pin);
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_SELFTEST);

    puts("OK: repetir, solo BACK visible durante captura, eventos inactivos y circulos fijos.");
    return 0;
}

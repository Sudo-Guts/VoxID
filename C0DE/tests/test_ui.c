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
#include "voxid_enroll.h"
#include "voxid_storage.h"
#include "mock_eeprom.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static char texto_pantalla[4096];
static uint32_t circulos_dibujados;
static bool botones_visibles[4];

typedef struct {
    uint16_t x;
    uint16_t y;
    uint16_t alto;
    uint16_t color;
} BarraPerfil;

static BarraPerfil barras_perfil[36];
static uint8_t cantidad_barras;

static bool registrar_texto(const char *texto) {
    size_t disponibles = sizeof(texto_pantalla) - strlen(texto_pantalla) - 1U;
    strncat(texto_pantalla, texto, disponibles);
    return true;
}

bool gfx_fill_screen(uint16_t color) {
    (void)color;
    texto_pantalla[0] = '\0';
    cantidad_barras = 0U;

    for (uint8_t i = 0U; i < 4U; ++i) {
        botones_visibles[i] = false;
    }

    return true;
}

bool gfx_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    assert(x + width <= 160U && y + height <= 128U);

    if (width == 2U && x >= 17U && x <= 135U && y >= 67U && y <= 93U) {
        assert(cantidad_barras < sizeof(barras_perfil) / sizeof(barras_perfil[0]));
        barras_perfil[cantidad_barras++] = (BarraPerfil){x, y, height, color};
    }

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

static void terminar_toma(bool con_senal) {
    comprobar_escucha();

    for (uint32_t b = 0U; b < VOXID_AUDIO_TOTAL_BLOCKS; ++b) {
        if (con_senal) {
            mock_dma_tone_block();
        } else {
            mock_dma_block(2048U);
        }

        voxid_audio_task();
        ui_task();
    }

    assert(voxid_audio_status() == VOXID_AUDIO_READY);
}

static void comprobar_identificacion(const char *esperado) {
    assert(ui_init());
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    uint32_t lecturas = mock_eeprom_read_calls;
    uint32_t escrituras = mock_eeprom_write_calls;
    terminar_toma(true);
    assert(strstr(texto_pantalla, esperado) != NULL);
    assert(mock_eeprom_read_calls == lecturas && mock_eeprom_write_calls == escrituras);
    pulsar(BTN_BACK_Pin);
    pulsar(BTN_BACK_Pin);
    pulsar(BTN_DOWN_Pin);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_ENROLL);
}

static void comprobar_perfiles(void) {
    voxid_audio_stop();
    voxid_enroll_reset();
    VoxIDSpeakerProfile perfil = {.completo = true};

    for (uint8_t toma = 0U; toma < VOXID_PROTOTYPES_PER_SPEAKER; ++toma) {
        perfil.frames_con_voz[toma] = VOXID_ENROLL_MIN_VOICED_FRAMES;
        perfil.mfcc[toma][0] = 1000.0f + (float)toma;
    }

    perfil.mfcc[0][1] = 3.0f;
    perfil.mfcc[0][2] = 4.0f;
    perfil.mfcc[1][1] = -5.0f;
    perfil.mfcc[2][12] = 2.0f;
    assert(voxid_enroll_restore(0U, &perfil));
    assert(voxid_enroll_restore(2U, &perfil));
    assert(ui_init());
    assert(strstr(texto_pantalla, "LOCUTORES") != NULL);
    assert(strstr(texto_pantalla, "DIAGNOSTICO") == NULL);
    uint32_t lecturas = mock_eeprom_read_calls;
    uint32_t escrituras = mock_eeprom_write_calls;
    pulsar(BTN_UP_Pin);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_PROFILES);
    assert(strstr(texto_pantalla, "LOCUTOR 01 / 04") != NULL);
    assert(strstr(texto_pantalla, "REGISTRADO / L2") != NULL);
    assert(strstr(texto_pantalla, "T1T2T3") != NULL);
    assert(strstr(texto_pantalla, "C01C12") != NULL);
    assert(botones_visibles[0] && botones_visibles[1] && !botones_visibles[2] && botones_visibles[3]);

    // Estos valores analiticos distinguen normalizacion L2, signo y toma.
    // Un promedio de prototipos o barras de MFCC crudos no produce estas barras.
    assert(cantidad_barras == 4U);
    assert(barras_perfil[0].x == 17U && barras_perfil[0].y == 72U && barras_perfil[0].alto == 8U);
    assert(barras_perfil[1].x == 27U && barras_perfil[1].y == 70U && barras_perfil[1].alto == 10U);
    assert(barras_perfil[2].x == 20U && barras_perfil[2].y == 81U && barras_perfil[2].alto == 13U);
    assert(barras_perfil[3].x == 133U && barras_perfil[3].y == 67U && barras_perfil[3].alto == 13U);
    assert(barras_perfil[0].color == barras_perfil[1].color);
    assert(barras_perfil[0].color != barras_perfil[2].color && barras_perfil[0].color != barras_perfil[3].color && barras_perfil[2].color != barras_perfil[3].color);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_PROFILES && cantidad_barras == 4U);
    pulsar(BTN_UP_Pin);
    assert(strstr(texto_pantalla, "LOCUTOR 04 / 04") != NULL);
    assert(strstr(texto_pantalla, "SIN REGISTRAR") != NULL && cantidad_barras == 0U);
    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "LOCUTOR 01 / 04") != NULL && cantidad_barras == 4U);
    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "LOCUTOR 02 / 04") != NULL && cantidad_barras == 0U);
    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "LOCUTOR 03 / 04") != NULL && cantidad_barras == 4U);
    pulsar(BTN_BACK_Pin);
    assert(ui_state() == UI_STATE_MENU);
    assert(mock_eeprom_read_calls == lecturas && mock_eeprom_write_calls == escrituras);
    assert(!mock_timer_running && !mock_dma_running);
    assert(memcmp(voxid_enroll_profile(0U)->mfcc, perfil.mfcc, sizeof(perfil.mfcc)) == 0);

    // Al recargar la EEPROM se consultan los perfiles reales del registro previo.
    assert(voxid_storage_init(&hi2c1));
    assert(ui_init());
    pulsar(BTN_UP_Pin);
    pulsar(BTN_OK_Pin);
    assert(strstr(texto_pantalla, "REGISTRADO / L2") != NULL && cantidad_barras > 0U);
}

static void comprobar_registro(void) {
    mock_hal_reset();
    mock_eeprom_reset();
    assert(voxid_storage_init(&hi2c1));
    buttons_init();
    assert(ui_init());
    pulsar(BTN_DOWN_Pin);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_ENROLL);
    assert(strstr(texto_pantalla, "ESPACIO VACIO") != NULL);
    pulsar(BTN_OK_Pin);
    assert(strstr(texto_pantalla, "TOMA 1/3") != NULL);
    pulsar(BTN_OK_Pin);
    terminar_toma(true);
    assert(voxid_enroll_count() == 0U && voxid_enroll_profile(0U) == NULL);
    assert(strstr(texto_pantalla, "OK: SIGUIENTE") != NULL);

    encolar(BTN_OK_Pin);
    encolar(BTN_DOWN_Pin);
    ui_task();
    comprobar_escucha();
    assert(voxid_enroll_count() == 1U);
    assert(strstr(texto_pantalla, "TOMA 2/3") != NULL);
    terminar_toma(false);
    assert(strstr(texto_pantalla, "VOZ INSUFICIENTE") != NULL);
    pulsar(BTN_OK_Pin);
    assert(voxid_enroll_count() == 1U);
    assert(strstr(texto_pantalla, "TOMA 2/3") != NULL);
    terminar_toma(true);
    pulsar(BTN_OK_Pin);
    comprobar_escucha();
    assert(voxid_enroll_count() == 2U);
    assert(strstr(texto_pantalla, "TOMA 3/3") != NULL);

    HAL_ADC_ErrorCallback(&hadc1);
    voxid_audio_task();
    ui_task();
    assert(strstr(texto_pantalla, "ERROR DE CAPTURA") != NULL);
    assert(voxid_enroll_count() == 2U);
    pulsar(BTN_OK_Pin);
    terminar_toma(true);
    assert(voxid_enroll_profile(0U) == NULL);
    assert(strstr(texto_pantalla, "OK: GUARDAR") != NULL);
    mock_eeprom_protected = true;
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_CAPTURE);
    assert(voxid_enroll_profile(0U) == NULL && voxid_enroll_count() == 2U);
    assert(voxid_audio_status() == VOXID_AUDIO_READY);
    assert(strstr(texto_pantalla, "ERROR AL GUARDAR") != NULL);
    assert(strstr(texto_pantalla, "OK: REINTENTAR") != NULL);
    assert(!mock_timer_running && !mock_dma_running);
    mock_eeprom_protected = false;
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_ENROLL);
    assert(strstr(texto_pantalla, "GUARDADO EN EEPROM") != NULL);
    assert(!mock_timer_running && !mock_dma_running);

    const VoxIDSpeakerProfile *perfil = voxid_enroll_profile(0U);
    assert(perfil != NULL && perfil->completo);
    VoxIDSpeakerProfile anterior = *perfil;
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    terminar_toma(true);
    pulsar(BTN_OK_Pin);
    comprobar_escucha();
    assert(voxid_enroll_count() == 1U);
    pulsar(BTN_BACK_Pin);
    assert(ui_state() == UI_STATE_ENROLL && voxid_enroll_count() == 0U);
    assert(memcmp(perfil->mfcc, anterior.mfcc, sizeof(anterior.mfcc)) == 0);

    assert(voxid_storage_init(&hi2c1));
    comprobar_identificacion("LOCUTOR 01");

    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "LOCUTOR 02") != NULL);
    assert(strstr(texto_pantalla, "ESPACIO VACIO") != NULL);
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);

    for (uint32_t i = 0U; i < VOXID_PROTOTYPES_PER_SPEAKER; ++i) {
        terminar_toma(true);
        pulsar(BTN_OK_Pin);
    }

    assert(ui_state() == UI_STATE_ENROLL);
    assert(voxid_enroll_profile(1U) != NULL);
    assert(voxid_enroll_profile(2U) == NULL);
    assert(memcmp(perfil->mfcc, anterior.mfcc, sizeof(anterior.mfcc)) == 0);
    comprobar_identificacion("VOZ AMBIGUA");
}

int main(void) {
    mock_hal_reset();
    mock_eeprom_reset();
    assert(voxid_storage_init(&hi2c1));
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
    assert(strstr(texto_pantalla, "VOZ INSUFICIENTE") != NULL);
    assert(strstr(texto_pantalla, "CON VOZ: 0") != NULL);
    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "SIN VOZ DETECTADA") != NULL);
    pulsar(BTN_DOWN_Pin);
    assert(strstr(texto_pantalla, "FRAMES: 298/298") != NULL);
    assert(strstr(texto_pantalla, "MIN:2048 MAX:2048") != NULL);

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
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    terminar_toma(true);
    assert(strstr(texto_pantalla, "SIN LOCUTORES") != NULL);
    pulsar(BTN_BACK_Pin);
    pulsar(BTN_BACK_Pin);
    pulsar(BTN_DOWN_Pin);
    pulsar(BTN_DOWN_Pin);
    pulsar(BTN_OK_Pin);
    pulsar(BTN_OK_Pin);
    assert(ui_state() == UI_STATE_PROFILES);
    assert(strstr(texto_pantalla, "SIN REGISTRAR") != NULL);

    comprobar_registro();
    comprobar_perfiles();

    puts("OK: captura, registro, identificacion y consulta de referencias L2 por locutor sin I2C.");
    return 0;
}

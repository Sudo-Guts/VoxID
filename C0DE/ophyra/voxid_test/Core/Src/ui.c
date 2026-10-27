/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid_test/Core/Src/ui.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  TFT horizontal: resumen de tiempos y dos paginas de coeficientes MFCC.
 *  UP / DOWN / OK: cambiar pagina. BACK: regresar al resumen.
 *  No dibujar durante las mediciones del DWT.
 * =============================================================================
 */

#include "ui.h"
#include "buttons.h"
#include "TFT_Ophyra.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define UI_BGR(r, g, b) (((uint32_t)(b) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(r))
#define UI_BG       UI_BGR(251, 248, 245)
#define UI_TEXT     UI_BGR(70, 62, 88)
#define UI_MUTED    UI_BGR(108, 101, 124)
#define UI_ACCENT   UI_BGR(55, 139, 125)
#define UI_BORDER   UI_BGR(207, 196, 219)
#define UI_FAIL     UI_BGR(177, 75, 75)

#define UI_PAGES 3U

static const MFCCBenchmarkResult *shown_result;
static uint8_t current_page;

static void ui_text(uint8_t x, uint8_t y, const char *value, uint32_t color) {
    (void)TFT_PrintStr(x, y, (CHAR *)value, color, UI_BG);
}

static void ui_header(const char *title) {
    (void)TFT_ClearDisplay(UI_BG);
    ui_text(7U, 7U, "VOXID TEST", UI_TEXT);
    ui_text(100U, 7U, "MFCC", UI_ACCENT);
    (void)TFT_PrintLine(6U, 25U, 153U, 25U, UI_BORDER);
    ui_text(7U, 31U, title, UI_ACCENT);
    (void)TFT_PrintLine(6U, 112U, 153U, 112U, UI_BORDER);
    ui_text(7U, 116U, "UP/DN/OK  BACK", UI_MUTED);
}

static const char *ui_status_label(MFCCBenchmarkStatus status) {
    switch (status) {
        case MFCC_BENCH_OK:             return "OK";
        case MFCC_BENCH_DWT_ERROR:      return "ERROR: DWT";
        case MFCC_BENCH_INIT_ERROR:     return "ERROR: INIT";
        case MFCC_BENCH_PROCESS_ERROR:  return "ERROR: DSP";
        case MFCC_BENCH_SEQUENCE_ERROR: return "ERROR: FRAMES";
        case MFCC_BENCH_VALUE_ERROR:    return "ERROR: NAN/INF";
        default:                        return "NO EJECUTADO";
    }
}

static void ui_duration(uint8_t y, const char *prefix, uint32_t cycles) {
    char line[26];

    /* Duracion en microsegundos, sin usar printf de punto flotante. */
    const uint64_t scaled = (uint64_t)cycles * 1000000ULL;
    const uint32_t us = (uint32_t)((scaled + shown_result->cpu_hz / 2U) / shown_result->cpu_hz);

    (void)snprintf(line, sizeof(line), "%s%lu.%03lums", prefix, (unsigned long)(us / 1000U), (unsigned long)(us % 1000U));
    ui_text(7U, y, line, UI_TEXT);
}

static void ui_summary(void) {
    char line[30];
    ui_header("440Hz / 16kHz");

    if (shown_result == NULL) {
        ui_text(7U, 59U, "PREPARANDO...", UI_MUTED);
        return;
    }
    if (shown_result->status != MFCC_BENCH_OK) {
        ui_text(7U, 55U, ui_status_label(shown_result->status), UI_FAIL);
        ui_text(7U, 77U, "Ver Debug", UI_MUTED);
        return;
    }

    (void)snprintf(line, sizeof(line), "Frames:%lu  Hops:%lu", (unsigned long)shown_result->frames, (unsigned long)shown_result->hops);
    ui_text(7U, 45U, line, UI_MUTED);
    ui_duration(57U, "F avg:", shown_result->frame_avg_cycles);
    ui_duration(69U, "F max:", shown_result->frame_max_cycles);
    ui_duration(81U, "H avg:", shown_result->hop_avg_cycles);
    ui_duration(93U, "H max:", shown_result->hop_max_cycles);

    const bool within_budget = shown_result->hop_max_cycles <=
    shown_result->cpu_hz / 100U;
    ui_text(7U, 104U, within_budget ? "HOP <10ms: SI" : "HOP <10ms: NO", within_budget ? UI_ACCENT : UI_FAIL);
}

static void ui_coefficient(uint8_t y, uint8_t index) {
    char line[25];
    float value = shown_result->mfcc[index];
    const bool negative = value < 0.0f;
    if (negative) {
        value = -value;
    }

    /* Formato fijo x100 para evitar _printf_float en newlib-nano. */
    if (value > 1000000.0f) {
        (void)snprintf(line, sizeof(line), "C%02u: fuera rango", (unsigned)index);
    } else {
        const uint32_t scaled = (uint32_t)(value * 100.0f + 0.5f);
        (void)snprintf(line, sizeof(line), "C%02u:%s%lu.%02lu", (unsigned)index, negative ? "-" : "+", (unsigned long)(scaled / 100U), (unsigned long)(scaled % 100U));
    }
    ui_text(7U, y, line, UI_TEXT);
}

static void ui_coefficients(void) {
    const uint8_t start = (current_page == 1U) ? 0U : 7U;
    const uint8_t end = (current_page == 1U) ? 7U : VOXID_MFCC_COUNT;

    ui_header((current_page == 1U) ? "MFCC C00-C06" : "MFCC C07-C12");

    if (shown_result == NULL || shown_result->status != MFCC_BENCH_OK) {
        ui_text(7U, 60U, "SIN RESULTADOS", UI_FAIL);
        return;
    }

    for (uint8_t k = start; k < end; ++k) {
        ui_coefficient((uint8_t)(43U + (k - start) * 10U), k);
    }
}

static void ui_draw(void) {
    if (current_page == 0U) {
        ui_summary();
    } else {
        ui_coefficients();
    }
}

void ui_init(void) {
    shown_result = NULL;
    current_page = 0U;
    (void)TFT_Init(0U); /* Horizontal 160 x 128. */
    (void)pfTFT_BACKLIGHT_GPIO(1U);
    ui_draw();
}

void ui_show_result(const MFCCBenchmarkResult *result) {
    shown_result = result;
    current_page = 0U;
    ui_draw();
}

void ui_task(void) {
    ButtonEvent event;
    uint8_t old_page = current_page;

    while (buttons_next_event(&event)) {
        switch (event) {
            case BUTTON_UP:
                current_page = (uint8_t)((current_page + UI_PAGES - 1U) % UI_PAGES);
                break;
            case BUTTON_DOWN:
            case BUTTON_OK:
                current_page = (uint8_t)((current_page + 1U) % UI_PAGES);
                break;
            case BUTTON_BACK:
                current_page = 0U;
                break;
            default:
                break;
        }
    }

    if (current_page != old_page) {
        ui_draw();
    }
}

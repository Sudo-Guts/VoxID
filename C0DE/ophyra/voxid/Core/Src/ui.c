/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : ui.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz pastel para la TFT de Ophyra (160 x 128).
 *  Utiliza la API grafica gfx_* y los eventos de buttons.c.
 *  Las pantallas de identificacion/registro permanecen informativas
 *  hasta integrar el procesamiento MFCC en tiempo real.
 *
 * =============================================================================
 */

#include "ui.h"

#include "buttons.h"
#include "gfx.h"
#include "voxid_dsp.h"
#include "voxid_selftest.h"

#include <math.h>
#include <stdint.h>
#include <stdio.h>

// -----------------------------------------------------------------------------
//  Paleta RGB565 - VoxID
// -----------------------------------------------------------------------------

#define UI_BG           0xFFBFU  // Marfil lavanda
#define UI_HEADER       0xEF3FU  // Lavanda clara
#define UI_PANEL        0xEF1EU  // Tarjetas pastel
#define UI_SELECTED     0x8337U  // Morado
#define UI_BORDER       0xCE1CU  // Borde lavanda
#define UI_TEXT         0x3128U  // Ciruela oscuro
#define UI_MUTED        0x6B0FU  // Gris violeta
#define UI_ACCENT       0x7295U  // Acento morado
#define UI_ACCENT_LIGHT 0xDE1DU  // Lavanda suave
#define UI_WARNING      0xA26DU  // Coral oscuro
#define UI_WHITE        0xFFFFU

#define UI_MENU_ITEMS   3U
#define UI_TEST_PAGES   5U
#define UI_GRAPH_WIDTH  129U

// -----------------------------------------------------------------------------
//  Estado privado
// -----------------------------------------------------------------------------

static UIState current_state = UI_STATE_MENU;
static UIState capture_origin = UI_STATE_IDENTIFY;

static uint8_t menu_selection = 0U;
static uint8_t rendered_menu_selection = 0U;
static uint8_t speaker_slot = 0U;
static uint8_t test_page = 0U;
static VoxIDSelfTestResult selftest_result;

static bool ui_ready = false;
static bool needs_redraw = true;

static const char *const menu_labels[UI_MENU_ITEMS] = {
    "IDENTIFICAR",
    "REGISTRAR",
    "DIAGNOSTICO"
};

// -----------------------------------------------------------------------------
//  Componentes comunes
// -----------------------------------------------------------------------------

// =============================================================================
//  Indicadores de botones fisicos
// =============================================================================

static bool ui_draw_footer_button(uint16_t x, const char *label, bool enabled) {
    uint16_t background = enabled ? UI_ACCENT_LIGHT : UI_PANEL;
    uint16_t foreground = enabled ? UI_TEXT : UI_MUTED;

    if (!gfx_fill_round_rect((int16_t)x, 115, 32U, 12U, 4U, background)) {
        return false;
    }

    return gfx_draw_text_aligned((int16_t)(x + 16U), 117, label, foreground, background, 1U, GFX_ALIGN_CENTER);
}

static bool ui_draw_footer(void) {
    bool can_move = current_state == UI_STATE_MENU || current_state == UI_STATE_ENROLL || current_state == UI_STATE_SELFTEST;
    bool can_ok = current_state != UI_STATE_CAPTURE;
    bool can_back = current_state != UI_STATE_MENU;

    if (!gfx_fill_rect(8U, 112U, 144U, 1U, UI_BORDER)) {
        return false;
    }

    if (!ui_draw_footer_button(4U, "UP", can_move)) {
        return false;
    }

    if (!ui_draw_footer_button(44U, "DN", can_move)) {
        return false;
    }

    if (!ui_draw_footer_button(84U, "OK", can_ok)) {
        return false;
    }

    return ui_draw_footer_button(124U, "BACK", can_back);
}

// =============================================================================
//  Componentes comunes
// =============================================================================

static bool ui_draw_header(void) {
    if (!gfx_fill_screen(UI_BG)) {
        return false;
    }

    if (!gfx_fill_round_rect(8, 4, 144U, 25U, 7U, UI_HEADER)) {
        return false;
    }

    if (!gfx_fill_rect(11U, 9U, 3U, 15U, UI_ACCENT)) {
        return false;
    }

    if (!gfx_draw_text_scaled(20, 9, "VOXID", UI_TEXT, 2U)) {
        return false;
    }

    if (!gfx_draw_text_aligned(145, 13, "OPHYRA", UI_ACCENT, UI_HEADER, 1U, GFX_ALIGN_RIGHT)) {
        return false;
    }

    return ui_draw_footer();
}

static bool ui_draw_title(const char *title) {
    return gfx_draw_text(10, 36, title, UI_ACCENT);
}

static bool ui_draw_card(void) {
    return gfx_fill_round_rect(8, 54, 144U, 51U, 8U, UI_PANEL);
}

// -----------------------------------------------------------------------------
//  Botones del menu
// -----------------------------------------------------------------------------

static bool ui_draw_menu_item(uint8_t index, bool selected) {
    if (index >= UI_MENU_ITEMS) {
        return false;
    }

    int16_t top = (int16_t)(49U + index * 21U);
    uint16_t background = selected ? UI_SELECTED : UI_PANEL;
    uint16_t foreground = selected ? UI_WHITE : UI_TEXT;

    if (!gfx_fill_round_rect(8, top, 144U, 18U, 7U, background)) {
        return false;
    }

    if (!gfx_fill_rect(11U, (uint16_t)(top + 4), 3U, 10U, selected ? UI_WHITE : UI_ACCENT_LIGHT)) {
        return false;
    }

    return gfx_draw_text_aligned(80, (int16_t)(top + 5), menu_labels[index], foreground, background, 1U, GFX_ALIGN_CENTER);
}

static bool ui_draw_menu(void) {
    if (!ui_draw_title("MENU PRINCIPAL")) {
        return false;
    }

    for (uint8_t i = 0U; i < UI_MENU_ITEMS; ++i) {
        if (!ui_draw_menu_item(i, i == menu_selection)) {
            return false;
        }
    }

    rendered_menu_selection = menu_selection;
    return true;
}

static bool ui_update_menu_selection(void) {
    if (rendered_menu_selection == menu_selection) {
        return true;
    }

    if (!ui_draw_menu_item(rendered_menu_selection, false)) {
        return false;
    }

    if (!ui_draw_menu_item(menu_selection, true)) {
        return false;
    }

    rendered_menu_selection = menu_selection;
    return true;
}

// -----------------------------------------------------------------------------
//  Pantallas informativas
// -----------------------------------------------------------------------------

// =============================================================================
//  Preparacion de adquisicion e inscripcion de locutor
// =============================================================================

static bool ui_draw_identify(void) {
    if (!ui_draw_title("IDENTIFICAR") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 62, "ADQUIRIR VOZ", UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 80, "PCM 16kHz / 16bit", UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, "OK: PREPARAR", UI_ACCENT);
}

static bool ui_draw_enroll(void) {
    char slot[26];

    if (!ui_draw_title("REGISTRAR") || !ui_draw_card()) {
        return false;
    }

    (void)snprintf(slot, sizeof(slot), "LOCUTOR %02u / 04", (unsigned)(speaker_slot + 1U));

    if (!gfx_draw_text(16, 62, slot, UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 80, "UP/DN: ELEGIR", UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, "OK: PREPARAR", UI_ACCENT);
}

static bool ui_draw_capture(void) {
    char slot[26];

    if (!ui_draw_title("CAPTURA DE VOZ") || !ui_draw_card()) {
        return false;
    }

    if (capture_origin == UI_STATE_ENROLL) {
        (void)snprintf(slot, sizeof(slot), "LOCUTOR %02u: SIN VOZ", (unsigned)(speaker_slot + 1U));

        if (!gfx_draw_text(16, 62, slot, UI_TEXT)) {
            return false;
        }
    } else if (!gfx_draw_text(16, 62, "IDENTIFICACION", UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 80, "ADC NO VINCULADO", UI_WARNING)) {
        return false;
    }

    return gfx_draw_text(16, 94, "SIN CAPTURA REAL", UI_MUTED);
}

// =============================================================================
//  Diagnostico del sistema
// =============================================================================

static bool ui_draw_system(void) {
    char line[26];

    if (!ui_draw_title("DIAGNOSTICO") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 60, "STM32F407 / ADC OFF", UI_TEXT)) {
        return false;
    }

    bool ok = voxid_dsp_is_ready();

    if (!gfx_draw_text(16, 74, ok ? "MFCC: LISTO" : "MFCC: ERROR", ok ? UI_ACCENT : UI_WARNING)) {
        return false;
    }

    (void)snprintf(line, sizeof(line), "Perdidos: %lu", (unsigned long)buttons_dropped());

    if (!gfx_draw_text(16, 87, line, UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 98, "OK: TEST 440HZ", UI_ACCENT);
}

// =============================================================================
//  Prueba sintetica MFCC y navegacion de coeficientes
// =============================================================================

static const char *ui_test_status(VoxIDSelfTestStatus status) {
    switch (status) {
        case VOXID_SELFTEST_OK:
            return "3 FRAMES: OK";
        case VOXID_SELFTEST_INIT_ERROR:
            return "ERROR: INICIO";
        case VOXID_SELFTEST_PROCESS_ERROR:
            return "ERROR: MFCC";
        case VOXID_SELFTEST_SEQUENCE_ERROR:
            return "ERROR: SECUENCIA";
        case VOXID_SELFTEST_VALUE_ERROR:
            return "ERROR: NAN/INF";
        default:
            return "SIN PRUEBA";
    }
}

static bool ui_draw_test_summary(void) {
    char line[26];

    if (!ui_draw_title("PRUEBA MFCC") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 61, "440Hz / 16kHz", UI_TEXT)) {
        return false;
    }

    (void)snprintf(line, sizeof(line), "MUESTRAS: %lu", (unsigned long)selftest_result.samples);

    if (!gfx_draw_text(16, 75, line, UI_MUTED)) {
        return false;
    }

    bool passed = selftest_result.status == VOXID_SELFTEST_OK;

    if (!gfx_draw_text(16, 89, ui_test_status(selftest_result.status), passed ? UI_ACCENT : UI_WARNING)) {
        return false;
    }

    return gfx_draw_text(16, 99, "OK: REPETIR", UI_MUTED);
}

static bool ui_draw_coefficient(uint8_t index, int16_t y) {
    char line[26];
    float value = selftest_result.mfcc[index];
    bool negative = value < 0.0f;

    if (negative) {
        value = -value;
    }

    if (!isfinite(value) || value > 1000000.0f) {
        (void)snprintf(line, sizeof(line), "C%02u: FUERA RANGO", (unsigned)index);
    } else {
        uint32_t scaled = (uint32_t)(value * 100.0f + 0.5f);
        (void)snprintf(line, sizeof(line), "C%02u:%c%lu.%02lu", (unsigned)index, negative ? '-' : '+', (unsigned long)(scaled / 100U), (unsigned long)(scaled % 100U));
    }

    return gfx_draw_text(13, y, line, UI_TEXT);
}

// =============================================================================
//  Grafica de muestras PCM sinteticas
// =============================================================================

static bool ui_draw_waveform(void) {
    if (!ui_draw_title("ONDA SINTETICA") || !ui_draw_card()) {
        return false;
    }

    if (selftest_result.status != VOXID_SELFTEST_OK) {
        return gfx_draw_text(16, 78, "SIN RESULTADOS", UI_WARNING);
    }

    if (!gfx_draw_text(16, 56, "440Hz / 16kHz", UI_MUTED)) {
        return false;
    }

    for (uint16_t x = 0U; x < UI_GRAPH_WIDTH; x += 8U) {
        if (!gfx_fill_rect((uint16_t)(16U + x), 80U, 4U, 1U, UI_BORDER)) {
            return false;
        }
    }

    const int16_t *samples = voxid_selftest_samples();
    uint32_t count = voxid_selftest_sample_count();

    if (samples == NULL || count != VOXID_SELFTEST_SAMPLES) {
        return false;
    }

    // Un minimo y un maximo por columna evitan perder los picos al comprimir.
    for (uint32_t x = 0U; x < UI_GRAPH_WIDTH; ++x) {
        uint32_t first = x * count / UI_GRAPH_WIDTH;
        uint32_t last = (x + 1U) * count / UI_GRAPH_WIDTH;
        int16_t minimum = samples[first];
        int16_t maximum = samples[first];

        for (uint32_t n = first + 1U; n < last; ++n) {
            if (samples[n] < minimum) {
                minimum = samples[n];
            }

            if (samples[n] > maximum) {
                maximum = samples[n];
            }
        }

        int16_t top = (int16_t)(80 - (int32_t)maximum * 15 / 12000);
        int16_t bottom = (int16_t)(80 - (int32_t)minimum * 15 / 12000);

        if (!gfx_fill_rect((uint16_t)(16U + x), (uint16_t)top, 1U, (uint16_t)(bottom - top + 1), UI_ACCENT)) {
            return false;
        }
    }

    return gfx_draw_text(16, 98, "720 MUESTRAS PCM", UI_MUTED);
}

// =============================================================================
//  Grafica de los 13 coeficientes MFCC
// =============================================================================

static bool ui_draw_mfcc_graph(void) {
    if (!ui_draw_title("GRAFICA MFCC") || !ui_draw_card()) {
        return false;
    }

    if (selftest_result.status != VOXID_SELFTEST_OK) {
        return gfx_draw_text(16, 78, "SIN RESULTADOS", UI_WARNING);
    }

    if (!gfx_draw_text(16, 56, "MORADO:+ CORAL:-", UI_MUTED)) {
        return false;
    }

    float maximum = 0.0f;

    for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        float value = fabsf(selftest_result.mfcc[k]);

        if (value > maximum) {
            maximum = value;
        }
    }

    if (maximum < 1e-10f) {
        maximum = 1.0f;
    }

    if (!gfx_fill_rect(16U, 80U, UI_GRAPH_WIDTH, 1U, UI_BORDER)) {
        return false;
    }

    // Escala simetrica alrededor de cero; se normaliza al mayor |MFCC|.
    for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        float value = selftest_result.mfcc[k];
        uint16_t magnitude = (uint16_t)(fabsf(value) * 15.0f / maximum + 0.5f);

        if (magnitude == 0U) {
            continue;
        }

        uint16_t x = (uint16_t)(17U + k * 10U);
        uint16_t y = value >= 0.0f ? (uint16_t)(80U - magnitude) : 81U;
        uint16_t color = value >= 0.0f ? UI_ACCENT : UI_WARNING;

        if (!gfx_fill_rect(x, y, 6U, magnitude, color)) {
            return false;
        }
    }

    if (!gfx_draw_text(17, 98, "C00", UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(122, 98, "C12", UI_MUTED);
}

static bool ui_draw_selftest(void) {
    if (test_page == 0U) {
        return ui_draw_test_summary();
    }

    if (test_page == 1U) {
        return ui_draw_waveform();
    }

    if (test_page == 2U) {
        return ui_draw_mfcc_graph();
    }

    uint8_t first = test_page == 3U ? 0U : 7U;
    uint8_t last = test_page == 3U ? 7U : VOXID_MFCC_COUNT;

    if (!ui_draw_title(test_page == 3U ? "MFCC C00-C06" : "MFCC C07-C12")) {
        return false;
    }

    if (selftest_result.status != VOXID_SELFTEST_OK) {
        return gfx_draw_text(15, 70, "SIN RESULTADOS", UI_WARNING);
    }

    for (uint8_t i = first; i < last; ++i) {
        if (!ui_draw_coefficient(i, (int16_t)(44U + (i - first) * 10U))) {
            return false;
        }
    }

    return true;
}

static bool ui_render(void) {
    if (!ui_draw_header()) {
        return false;
    }

    switch (current_state) {
        case UI_STATE_MENU:
            return ui_draw_menu();

        case UI_STATE_IDENTIFY:
            return ui_draw_identify();

        case UI_STATE_ENROLL:
            return ui_draw_enroll();

        case UI_STATE_SYSTEM:
            return ui_draw_system();

        case UI_STATE_CAPTURE:
            return ui_draw_capture();

        case UI_STATE_SELFTEST:
            return ui_draw_selftest();

        default:
            current_state = UI_STATE_MENU;
            return ui_draw_menu();
    }
}

// -----------------------------------------------------------------------------
//  Navegacion
// -----------------------------------------------------------------------------

// =============================================================================
//  Navegacion por eventos
// =============================================================================

static void ui_dispatch(ButtonEvent event) {
    if (event == BUTTON_NONE) {
        return;
    }

    if (current_state == UI_STATE_MENU) {
        switch (event) {
            case BUTTON_UP:
                menu_selection = (uint8_t)((menu_selection + UI_MENU_ITEMS - 1U) % UI_MENU_ITEMS);
                break;
            case BUTTON_DOWN:
                menu_selection = (uint8_t)((menu_selection + 1U) % UI_MENU_ITEMS);
                break;
            case BUTTON_OK:
                current_state = menu_selection == 0U ? UI_STATE_IDENTIFY : menu_selection == 1U ? UI_STATE_ENROLL : UI_STATE_SYSTEM;
                needs_redraw = true;
                break;
            default:
                break;
        }

        return;
    }

    if (event == BUTTON_BACK) {
        if (current_state == UI_STATE_CAPTURE) {
            current_state = capture_origin;
        } else if (current_state == UI_STATE_SELFTEST) {
            current_state = UI_STATE_SYSTEM;
        } else {
            current_state = UI_STATE_MENU;
        }

        needs_redraw = true;
        return;
    }

    if (current_state == UI_STATE_ENROLL) {
        if (event == BUTTON_UP) {
            speaker_slot = (uint8_t)((speaker_slot + 3U) % 4U);
            needs_redraw = true;
        } else if (event == BUTTON_DOWN) {
            speaker_slot = (uint8_t)((speaker_slot + 1U) % 4U);
            needs_redraw = true;
        }
    }

    if (current_state == UI_STATE_SELFTEST) {
        if (event == BUTTON_UP) {
            test_page = (uint8_t)((test_page + UI_TEST_PAGES - 1U) % UI_TEST_PAGES);
            needs_redraw = true;
        } else if (event == BUTTON_DOWN) {
            test_page = (uint8_t)((test_page + 1U) % UI_TEST_PAGES);
            needs_redraw = true;
        }
    }

    if (event != BUTTON_OK) {
        return;
    }

    switch (current_state) {
        case UI_STATE_IDENTIFY:
        case UI_STATE_ENROLL:
            capture_origin = current_state;
            current_state = UI_STATE_CAPTURE;
            needs_redraw = true;
            break;
        case UI_STATE_SYSTEM:
        case UI_STATE_SELFTEST:
            current_state = UI_STATE_SELFTEST;
            test_page = 0U;
            voxid_selftest_run(&selftest_result);
            needs_redraw = true;
            break;
        default:
            break;
    }
}

bool ui_init(void) {
    current_state = UI_STATE_MENU;
    capture_origin = UI_STATE_IDENTIFY;
    menu_selection = 0U;
    rendered_menu_selection = 0U;
    speaker_slot = 0U;
    test_page = 0U;
    selftest_result.status = VOXID_SELFTEST_NOT_RUN;
    selftest_result.samples = 0U;
    selftest_result.frames = 0U;
    needs_redraw = true;

    ui_ready = ui_render();
    needs_redraw = !ui_ready;

    return ui_ready;
}

void ui_task(void) {
    if (!ui_ready) {
        return;
    }

    ButtonEvent event;

    while (buttons_next_event(&event)) {
        ui_dispatch(event);
    }

    if (needs_redraw) {
        ui_ready = ui_render();
        needs_redraw = !ui_ready;
    } else if (current_state == UI_STATE_MENU && menu_selection != rendered_menu_selection) {
        ui_ready = ui_update_menu_selection();
    }
}

UIState ui_state(void) {
    return current_state;
}

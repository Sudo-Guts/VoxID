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
#include "voxid_audio.h"
#include "voxid_voice.h"
#include "ui_voice.h"
#include "main.h"

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
#define UI_TEST_PAGES   4U
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
static VoxIDAudioState rendered_audio_state = VOXID_AUDIO_IDLE;
static uint8_t voice_phase = 0U;
static uint32_t voice_last_tick = 0U;
static bool voice_capture_pending = false;
static uint8_t voice_seconds_rendered = 3U;
static uint8_t capture_page = 0U;
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

    if (!gfx_fill_round_rect((int16_t)x, 115, 32U, 12U, 4U, background)) {
        return false;
    }

    if (!enabled) {
        return true;
    }

    return gfx_draw_text_aligned((int16_t)(x + 16U), 117, label, UI_TEXT, background, 1U, GFX_ALIGN_CENTER);
}

static bool ui_capture_is_listening(void) {
    return current_state == UI_STATE_CAPTURE && (voice_capture_pending || voxid_audio_status() == VOXID_AUDIO_CAPTURING);
}

static bool ui_draw_footer(void) {
    bool can_move = current_state == UI_STATE_MENU || current_state == UI_STATE_ENROLL || current_state == UI_STATE_SELFTEST || (current_state == UI_STATE_CAPTURE && voxid_audio_status() == VOXID_AUDIO_READY);
    bool can_ok = !ui_capture_is_listening();
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

// =============================================================================
//  Indicador de captura: circulos pastel
// =============================================================================

// =============================================================================
//  Grafica reutilizable de trece coeficientes MFCC
// =============================================================================

static bool ui_draw_mfcc_bars(const float *values) {
    if (values == NULL) {
        return false;
    }

    float maximum = 0.0f;

    for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        float value = fabsf(values[k]);

        if (value > maximum) {
            maximum = value;
        }
    }

    if (maximum < 1e-10f) {
        maximum = 1.0f;
    }

    if (!gfx_fill_rect(16U, 78U, UI_GRAPH_WIDTH, 1U, UI_BORDER)) {
        return false;
    }

    // Escala simetrica alrededor de cero; se normaliza al mayor |MFCC|.
    for (uint8_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        float value = values[k];
        uint16_t magnitude = (uint16_t)(fabsf(value) * 19.0f / maximum + 0.5f);

        if (magnitude == 0U) {
            continue;
        }

        uint16_t x = (uint16_t)(17U + k * 10U);
        uint16_t y = value >= 0.0f ? (uint16_t)(78U - magnitude) : 79U;
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

static bool ui_draw_capture_hint(uint8_t remaining) {
    char message[26];
    (void)snprintf(message, sizeof(message), "ESCUCHANDO: %us", (unsigned)remaining);

    if (!gfx_fill_rect(9U, 101U, 142U, 9U, UI_BG)) {
        return false;
    }

    return gfx_draw_text_aligned(80, 102, message, UI_ACCENT, UI_BG, 1U, GFX_ALIGN_CENTER);
}

static bool ui_draw_capture(void) {
    char line[30];
    VoxIDAudioState state = voxid_audio_status();
    rendered_audio_state = state;

    if (state == VOXID_AUDIO_READY && capture_page == 1U) {
        (void)snprintf(line, sizeof(line), "MFCC VOZ REAL");
    } else if (capture_origin == UI_STATE_ENROLL) {
        (void)snprintf(line, sizeof(line), "LOCUTOR %02u / MARIPOSA", (unsigned)(speaker_slot + 1U));
    } else {
        (void)snprintf(line, sizeof(line), "MARIPOSA / PA2");
    }

    if (!ui_draw_title(line)) {
        return false;
    }

    // Los resultados y errores son pantallas estaticas: no deben
    // conservar los circulos ni la cuenta regresiva de la captura.
    if (state == VOXID_AUDIO_READY || state == VOXID_AUDIO_ERROR) {
        if (!gfx_fill_rect(8U, 45U, 144U, 65U, UI_BG)) {
            return false;
        }

        if (!ui_draw_card()) {
            return false;
        }

        if (state == VOXID_AUDIO_ERROR) {
            if (!gfx_draw_text(16, 75, "ERROR DE CAPTURA", UI_WARNING)) {
                return false;
            }

            return gfx_draw_text(16, 93, "OK: REPETIR", UI_MUTED);
        }

        VoxIDAudioStats adc;
        VoxIDVoiceStats mfcc;

        if (!voxid_audio_read_stats(&adc) || !voxid_voice_read_stats(&mfcc)) {
            return gfx_draw_text(16, 77, "ERROR MFCC REAL", UI_WARNING);
        }

        if (capture_page == 1U) {
            // Grafica de la media MFCC de los 298 frames, no de un locutor entrenado.
            return ui_draw_mfcc_bars(mfcc.mean_mfcc);
        }

        if (!gfx_draw_text(16, 58, "3 SEG / MFCC OK", UI_ACCENT)) {
            return false;
        }

        (void)snprintf(line, sizeof(line), "FRAMES: %lu/%u", (unsigned long)mfcc.frames, (unsigned)VOXID_VOICE_EXPECTED_FRAMES);

        if (!gfx_draw_text(16, 70, line, UI_TEXT)) {
            return false;
        }

        (void)snprintf(line, sizeof(line), "MIN:%u MAX:%u", (unsigned)adc.minimum, (unsigned)adc.maximum);

        if (!gfx_draw_text(16, 82, line, UI_MUTED)) {
            return false;
        }

        (void)snprintf(line, sizeof(line), "PROMEDIO:%u", (unsigned)adc.average);
        return gfx_draw_text(16, 94, line, UI_MUTED);
    }

    // PREPARADO: los circulos se animan desde ui_task().
    // ESCUCHANDO: se dibujan una sola vez y permanecen inmoviles.
    // RESULTADO/ERROR: la pantalla no muestra circulos.
    if (!ui_voice_draw(voice_phase, UI_BG)) {
        return false;
    }

    if (voice_capture_pending || state == VOXID_AUDIO_CAPTURING) {
        uint32_t samples = voxid_audio_sample_count();
        uint32_t remain = VOXID_AUDIO_TOTAL_SAMPLES > samples ? VOXID_AUDIO_TOTAL_SAMPLES - samples : 0U;
        voice_seconds_rendered = (uint8_t)((remain + VOXID_AUDIO_SAMPLE_RATE - 1U) / VOXID_AUDIO_SAMPLE_RATE);
        return ui_draw_capture_hint(voice_seconds_rendered);
    }

    return gfx_draw_text_aligned(80, 102, "OK: HABLAR 3 SEG", UI_ACCENT, UI_BG, 1U, GFX_ALIGN_CENTER);
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
            return "OK";
        case VOXID_SELFTEST_INIT_ERROR:
            return "INIT ERR";
        case VOXID_SELFTEST_PROCESS_ERROR:
            return "DSP ERR";
        case VOXID_SELFTEST_SEQUENCE_ERROR:
            return "SEQ ERR";
        case VOXID_SELFTEST_VALUE_ERROR:
            return "NAN/INF";
        default:
            return "SIN TEST";
    }
}

static bool ui_draw_test_summary(void) {
    char line[26];

    if (!ui_draw_title("PRUEBA MFCC") || !ui_draw_card()) {
        return false;
    }

    // Etiquetas a la izquierda y valores alineados a la derecha.
    if (!gfx_draw_text(16, 61, "SENAL", UI_MUTED)) {
        return false;
    }

    if (!gfx_draw_text_aligned(144, 61, "440Hz / 16kHz", UI_TEXT, UI_PANEL, 1U, GFX_ALIGN_RIGHT)) {
        return false;
    }

    if (!gfx_draw_text(16, 76, "MUESTRAS", UI_MUTED)) {
        return false;
    }

    (void)snprintf(line, sizeof(line), "%lu", (unsigned long)selftest_result.samples);

    if (!gfx_draw_text_aligned(144, 76, line, UI_TEXT, UI_PANEL, 1U, GFX_ALIGN_RIGHT)) {
        return false;
    }

    if (!gfx_draw_text(16, 91, "FRAMES", UI_MUTED)) {
        return false;
    }

    bool passed = selftest_result.status == VOXID_SELFTEST_OK;
    (void)snprintf(line, sizeof(line), "%lu/%u %s", (unsigned long)selftest_result.frames, (unsigned)VOXID_SELFTEST_FRAMES, ui_test_status(selftest_result.status));
    return gfx_draw_text_aligned(144, 91, line, passed ? UI_ACCENT : UI_WARNING, UI_PANEL, 1U, GFX_ALIGN_RIGHT);
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
//  Grafica de los 13 coeficientes MFCC
// =============================================================================

static bool ui_draw_mfcc_graph(void) {
    if (!ui_draw_title("GRAFICA MFCC") || !ui_draw_card()) {
        return false;
    }

    if (selftest_result.status != VOXID_SELFTEST_OK) {
        return gfx_draw_text(16, 78, "SIN RESULTADOS", UI_WARNING);
    }

    return ui_draw_mfcc_bars(selftest_result.mfcc);
}

static bool ui_draw_selftest(void) {
    if (test_page == 0U) {
        return ui_draw_test_summary();
    }

    if (test_page == 1U) {
        return ui_draw_mfcc_graph();
    }

    uint8_t first = test_page == 2U ? 0U : 7U;
    uint8_t last = test_page == 2U ? 7U : VOXID_MFCC_COUNT;

    if (!ui_draw_title(test_page == 2U ? "MFCC C00-C06" : "MFCC C07-C12")) {
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
            voxid_audio_stop();
            voice_capture_pending = false;
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

    if (current_state == UI_STATE_CAPTURE && voxid_audio_status() == VOXID_AUDIO_READY) {
        if (event == BUTTON_UP || event == BUTTON_DOWN) {
            capture_page = (uint8_t)(1U - capture_page);
            needs_redraw = true;
            return;
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
            capture_page = 0U;
            voice_phase = 0U;
            voice_last_tick = HAL_GetTick();
            voice_capture_pending = false;
            needs_redraw = true;
            break;
        case UI_STATE_CAPTURE:
            if (!ui_capture_is_listening()) {
                capture_page = 0U;
                voice_phase = 0U;
                voice_last_tick = HAL_GetTick();
                voice_capture_pending = true;
                needs_redraw = true;
            }
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
    rendered_audio_state = VOXID_AUDIO_IDLE;
    voice_phase = 0U;
    voice_last_tick = HAL_GetTick();
    voice_capture_pending = false;
    voice_seconds_rendered = VOXID_AUDIO_DURATION_S;
    capture_page = 0U;
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

    // Transicion de ESCUCHANDO a RESULTADO/ERROR: renderizar una sola
    // pantalla nueva y no volver a dibujar anillos tras la captura.
    if (current_state == UI_STATE_CAPTURE && !voice_capture_pending && voxid_audio_status() != rendered_audio_state) {
        needs_redraw = true;
    }

    if (needs_redraw) {
        ui_ready = ui_render();
        needs_redraw = !ui_ready;

        // Pintar la cuenta regresiva y ocultar OK antes de iniciar DMA.
        // Asi el footer refleja el estado real durante toda la captura.
        if (ui_ready && current_state == UI_STATE_CAPTURE && voice_capture_pending) {
            voice_capture_pending = false;

            if (voxid_audio_start()) {
                rendered_audio_state = VOXID_AUDIO_CAPTURING;
            } else {
                needs_redraw = true;
            }
        }

        return;
    }

    if (current_state == UI_STATE_MENU && menu_selection != rendered_menu_selection) {
        ui_ready = ui_update_menu_selection();
        return;
    }

    if (current_state != UI_STATE_CAPTURE) {
        return;
    }

    VoxIDAudioState state = voxid_audio_status();

    if (state == VOXID_AUDIO_CAPTURING) {
        // Durante los 3 segundos los anillos no se redibujan.
        // Unicamente se actualiza el texto al cambiar de segundo.
        uint32_t samples = voxid_audio_sample_count();
        uint32_t remain = VOXID_AUDIO_TOTAL_SAMPLES > samples ? VOXID_AUDIO_TOTAL_SAMPLES - samples : 0U;
        uint8_t seconds = (uint8_t)((remain + VOXID_AUDIO_SAMPLE_RATE - 1U) / VOXID_AUDIO_SAMPLE_RATE);

        if (seconds != voice_seconds_rendered) {
            voice_seconds_rendered = seconds;
            ui_ready = ui_draw_capture_hint(seconds);
        }

        return;
    }

    // La animacion de bienvenida solo funciona antes de iniciar la captura.
    // Los estados READY y ERROR mantienen pantallas estaticas.
    if (state == VOXID_AUDIO_IDLE && !voice_capture_pending) {
        uint32_t now = HAL_GetTick();

        if ((uint32_t)(now - voice_last_tick) >= UI_VOICE_TICK_MS) {
            voice_last_tick = now;
            voice_phase = (uint8_t)((voice_phase + 1U) % UI_VOICE_PHASES);
            ui_ready = ui_voice_draw(voice_phase, UI_BG);
        }
    }
}

UIState ui_state(void) {
    return current_state;
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/ui.c
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

static UIState estado_actual = UI_STATE_MENU;
static UIState origen_captura = UI_STATE_IDENTIFY;

static uint8_t opcion_menu = 0U;
static uint8_t opcion_dibujada = 0U;
static uint8_t espacio_locutor = 0U;
static uint8_t pagina_prueba = 0U;
static VoxIDAudioState estado_audio_dibujado = VOXID_AUDIO_IDLE;
static uint8_t fase_circulos = 0U;
static uint32_t ultimo_tick_circulos = 0U;
static bool captura_pendiente = false;
static uint8_t segundos_dibujados = 3U;
static uint8_t pagina_captura = 0U;
static VoxIDSelfTestResult resultado_prueba;

static bool interfaz_lista = false;
static bool redibujar = true;

static const char *const etiquetas_menu[UI_MENU_ITEMS] = {
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
    return estado_actual == UI_STATE_CAPTURE && (captura_pendiente || voxid_audio_status() == VOXID_AUDIO_CAPTURING);
}

static bool ui_draw_footer(void) {
    bool can_move = estado_actual == UI_STATE_MENU || estado_actual == UI_STATE_ENROLL || estado_actual == UI_STATE_SELFTEST || (estado_actual == UI_STATE_CAPTURE && voxid_audio_status() == VOXID_AUDIO_READY);
    bool can_ok = !ui_capture_is_listening();
    bool can_back = estado_actual != UI_STATE_MENU;

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

    return gfx_draw_text_aligned(80, (int16_t)(top + 5), etiquetas_menu[index], foreground, background, 1U, GFX_ALIGN_CENTER);
}

static bool ui_draw_menu(void) {
    if (!ui_draw_title("MENU PRINCIPAL")) {
        return false;
    }

    for (uint8_t i = 0U; i < UI_MENU_ITEMS; ++i) {
        if (!ui_draw_menu_item(i, i == opcion_menu)) {
            return false;
        }
    }

    opcion_dibujada = opcion_menu;
    return true;
}

static bool ui_update_menu_selection(void) {
    if (opcion_dibujada == opcion_menu) {
        return true;
    }

    if (!ui_draw_menu_item(opcion_dibujada, false)) {
        return false;
    }

    if (!ui_draw_menu_item(opcion_menu, true)) {
        return false;
    }

    opcion_dibujada = opcion_menu;
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

    (void)snprintf(slot, sizeof(slot), "LOCUTOR %02u / 04", (unsigned)(espacio_locutor + 1U));

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
    VoxIDAudioState state = captura_pendiente ? VOXID_AUDIO_CAPTURING : voxid_audio_status();
    estado_audio_dibujado = state;

    if (state == VOXID_AUDIO_READY && pagina_captura == 1U) {
        (void)snprintf(line, sizeof(line), "MEDIA MFCC / VOZ");
    } else if (state == VOXID_AUDIO_READY) {
        (void)snprintf(line, sizeof(line), "3 SEG / MFCC OK");
    } else if (origen_captura == UI_STATE_ENROLL) {
        (void)snprintf(line, sizeof(line), "LOCUTOR %02u / MARIPOSA", (unsigned)(espacio_locutor + 1U));
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

        if (pagina_captura == 1U) {
            if (mfcc.voiced_frames == 0U) {
                return gfx_draw_text(16, 75, "SIN VOZ DETECTADA", UI_WARNING);
            }

            // Media de los frames que superan el umbral de energia.
            return ui_draw_mfcc_bars(mfcc.mean_mfcc);
        }

        (void)snprintf(line, sizeof(line), "FRAMES: %lu/%u", (unsigned long)mfcc.frames, (unsigned)VOXID_VOICE_EXPECTED_FRAMES);

        if (!gfx_draw_text(16, 58, line, UI_TEXT)) {
            return false;
        }

        (void)snprintf(line, sizeof(line), "CON VOZ: %lu", (unsigned long)mfcc.voiced_frames);

        if (!gfx_draw_text(16, 70, line, mfcc.voiced_frames > 0U ? UI_ACCENT : UI_WARNING)) {
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
    if (!ui_voice_draw(fase_circulos, UI_BG)) {
        return false;
    }

    if (captura_pendiente || state == VOXID_AUDIO_CAPTURING) {
        uint32_t samples = captura_pendiente ? 0U : voxid_audio_sample_count();
        uint32_t remain = VOXID_AUDIO_TOTAL_SAMPLES > samples ? VOXID_AUDIO_TOTAL_SAMPLES - samples : 0U;
        segundos_dibujados = (uint8_t)((remain + VOXID_AUDIO_SAMPLE_RATE - 1U) / VOXID_AUDIO_SAMPLE_RATE);
        return ui_draw_capture_hint(segundos_dibujados);
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

    (void)snprintf(line, sizeof(line), "%lu", (unsigned long)resultado_prueba.samples);

    if (!gfx_draw_text_aligned(144, 76, line, UI_TEXT, UI_PANEL, 1U, GFX_ALIGN_RIGHT)) {
        return false;
    }

    if (!gfx_draw_text(16, 91, "FRAMES", UI_MUTED)) {
        return false;
    }

    bool passed = resultado_prueba.status == VOXID_SELFTEST_OK;
    (void)snprintf(line, sizeof(line), "%lu/%u %s", (unsigned long)resultado_prueba.frames, (unsigned)VOXID_SELFTEST_FRAMES, ui_test_status(resultado_prueba.status));
    return gfx_draw_text_aligned(144, 91, line, passed ? UI_ACCENT : UI_WARNING, UI_PANEL, 1U, GFX_ALIGN_RIGHT);
}

static bool ui_draw_coefficient(uint8_t index, int16_t y) {
    char line[26];
    float value = resultado_prueba.mfcc[index];
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

    if (resultado_prueba.status != VOXID_SELFTEST_OK) {
        return gfx_draw_text(16, 78, "SIN RESULTADOS", UI_WARNING);
    }

    return ui_draw_mfcc_bars(resultado_prueba.mfcc);
}

static bool ui_draw_selftest(void) {
    if (pagina_prueba == 0U) {
        return ui_draw_test_summary();
    }

    if (pagina_prueba == 1U) {
        return ui_draw_mfcc_graph();
    }

    uint8_t first = pagina_prueba == 2U ? 0U : 7U;
    uint8_t last = pagina_prueba == 2U ? 7U : VOXID_MFCC_COUNT;

    if (!ui_draw_title(pagina_prueba == 2U ? "MFCC C00-C06" : "MFCC C07-C12")) {
        return false;
    }

    if (resultado_prueba.status != VOXID_SELFTEST_OK) {
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

    switch (estado_actual) {
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
            estado_actual = UI_STATE_MENU;
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

    if (estado_actual == UI_STATE_MENU) {
        switch (event) {
            case BUTTON_UP:
                opcion_menu = (uint8_t)((opcion_menu + UI_MENU_ITEMS - 1U) % UI_MENU_ITEMS);
                break;
            case BUTTON_DOWN:
                opcion_menu = (uint8_t)((opcion_menu + 1U) % UI_MENU_ITEMS);
                break;
            case BUTTON_OK:
                estado_actual = opcion_menu == 0U ? UI_STATE_IDENTIFY : opcion_menu == 1U ? UI_STATE_ENROLL : UI_STATE_SYSTEM;
                redibujar = true;
                break;
            default:
                break;
        }

        return;
    }

    if (event == BUTTON_BACK) {
        if (estado_actual == UI_STATE_CAPTURE) {
            voxid_audio_stop();
            captura_pendiente = false;
            estado_actual = origen_captura;
        } else if (estado_actual == UI_STATE_SELFTEST) {
            estado_actual = UI_STATE_SYSTEM;
        } else {
            estado_actual = UI_STATE_MENU;
        }

        redibujar = true;
        return;
    }

    if (estado_actual == UI_STATE_ENROLL) {
        if (event == BUTTON_UP) {
            espacio_locutor = (uint8_t)((espacio_locutor + 3U) % 4U);
            redibujar = true;
        } else if (event == BUTTON_DOWN) {
            espacio_locutor = (uint8_t)((espacio_locutor + 1U) % 4U);
            redibujar = true;
        }
    }

    if (estado_actual == UI_STATE_CAPTURE && voxid_audio_status() == VOXID_AUDIO_READY) {
        if (event == BUTTON_UP || event == BUTTON_DOWN) {
            pagina_captura = (uint8_t)(1U - pagina_captura);
            redibujar = true;
            return;
        }
    }

    if (estado_actual == UI_STATE_SELFTEST) {
        if (event == BUTTON_UP) {
            pagina_prueba = (uint8_t)((pagina_prueba + UI_TEST_PAGES - 1U) % UI_TEST_PAGES);
            redibujar = true;
        } else if (event == BUTTON_DOWN) {
            pagina_prueba = (uint8_t)((pagina_prueba + 1U) % UI_TEST_PAGES);
            redibujar = true;
        }
    }

    if (event != BUTTON_OK) {
        return;
    }

    switch (estado_actual) {
        case UI_STATE_IDENTIFY:
        case UI_STATE_ENROLL:
            origen_captura = estado_actual;
            estado_actual = UI_STATE_CAPTURE;
            pagina_captura = 0U;
            fase_circulos = 0U;
            ultimo_tick_circulos = HAL_GetTick();
            captura_pendiente = false;
            redibujar = true;
            break;
        case UI_STATE_CAPTURE:
            if (!ui_capture_is_listening()) {
                pagina_captura = 0U;
                fase_circulos = 0U;
                ultimo_tick_circulos = HAL_GetTick();
                captura_pendiente = true;
                segundos_dibujados = VOXID_AUDIO_DURATION_S;
                redibujar = true;
            }
            break;

        case UI_STATE_SYSTEM:
        case UI_STATE_SELFTEST:
            estado_actual = UI_STATE_SELFTEST;
            pagina_prueba = 0U;
            voxid_selftest_run(&resultado_prueba);
            redibujar = true;
            break;
        default:
            break;
    }
}

bool ui_init(void) {
    estado_actual = UI_STATE_MENU;
    origen_captura = UI_STATE_IDENTIFY;
    opcion_menu = 0U;
    opcion_dibujada = 0U;
    espacio_locutor = 0U;
    pagina_prueba = 0U;
    estado_audio_dibujado = VOXID_AUDIO_IDLE;
    fase_circulos = 0U;
    ultimo_tick_circulos = HAL_GetTick();
    captura_pendiente = false;
    segundos_dibujados = VOXID_AUDIO_DURATION_S;
    pagina_captura = 0U;
    resultado_prueba.status = VOXID_SELFTEST_NOT_RUN;
    resultado_prueba.samples = 0U;
    resultado_prueba.frames = 0U;
    redibujar = true;

    interfaz_lista = ui_render();
    redibujar = !interfaz_lista;

    return interfaz_lista;
}

void ui_task(void) {
    if (!interfaz_lista) {
        return;
    }

    ButtonEvent event;

    while (buttons_next_event(&event)) {
        ui_dispatch(event);
    }

    // Transicion de ESCUCHANDO a RESULTADO/ERROR: renderizar una sola
    // pantalla nueva y no volver a dibujar anillos tras la captura.
    if (estado_actual == UI_STATE_CAPTURE && !captura_pendiente && voxid_audio_status() != estado_audio_dibujado) {
        redibujar = true;
    }

    if (redibujar) {
        interfaz_lista = ui_render();
        redibujar = !interfaz_lista;

        // Pintar la cuenta regresiva y ocultar OK antes de iniciar DMA.
        // Asi el footer refleja el estado real durante toda la captura.
        if (interfaz_lista && estado_actual == UI_STATE_CAPTURE && captura_pendiente) {
            captura_pendiente = false;

            if (voxid_audio_start()) {
                estado_audio_dibujado = VOXID_AUDIO_CAPTURING;
            } else {
                redibujar = true;
            }
        }

        return;
    }

    if (estado_actual == UI_STATE_MENU && opcion_menu != opcion_dibujada) {
        interfaz_lista = ui_update_menu_selection();
        return;
    }

    if (estado_actual != UI_STATE_CAPTURE) {
        return;
    }

    VoxIDAudioState state = voxid_audio_status();

    if (state == VOXID_AUDIO_CAPTURING) {
        // Durante los 3 segundos los anillos no se redibujan.
        // Unicamente se actualiza el texto al cambiar de segundo.
        uint32_t samples = voxid_audio_sample_count();
        uint32_t remain = VOXID_AUDIO_TOTAL_SAMPLES > samples ? VOXID_AUDIO_TOTAL_SAMPLES - samples : 0U;
        uint8_t seconds = (uint8_t)((remain + VOXID_AUDIO_SAMPLE_RATE - 1U) / VOXID_AUDIO_SAMPLE_RATE);

        if (seconds != segundos_dibujados) {
            segundos_dibujados = seconds;
            interfaz_lista = ui_draw_capture_hint(seconds);
        }

        return;
    }

    // La animacion de bienvenida solo funciona antes de iniciar la captura.
    // Los estados READY y ERROR mantienen pantallas estaticas.
    if (state == VOXID_AUDIO_IDLE && !captura_pendiente) {
        uint32_t now = HAL_GetTick();

        if ((uint32_t)(now - ultimo_tick_circulos) >= UI_VOICE_TICK_MS) {
            ultimo_tick_circulos = now;
            fase_circulos = (uint8_t)((fase_circulos + 1U) % UI_VOICE_PHASES);
            interfaz_lista = ui_voice_draw(fase_circulos, UI_BG);
        }
    }
}

UIState ui_state(void) {
    return estado_actual;
}

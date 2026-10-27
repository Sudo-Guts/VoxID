/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/ui.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Interfaz pastel para la TFT de Ophyra (160 x 128).
 *  Utiliza la API grafica gfx_* y los eventos de buttons.c.
 *  Registro persistente, consulta de perfiles e identificacion de locutores.
 *
 * =============================================================================
 */

#include "ui.h"

#include "buttons.h"
#include "gfx.h"
#include "voxid_audio.h"
#include "voxid_voice.h"
#include "voxid_enroll.h"
#include "voxid_identify.h"
#include "voxid_storage.h"
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

#define UI_MENU_ITEMS            3U
#define UI_GRAPH_WIDTH         129U
#define UI_PROFILE_AXIS         80U
#define UI_PROFILE_HALF_HEIGHT 13U
#define UI_PROFILE_THIRD_COLOR 0x34B1U

// -----------------------------------------------------------------------------
//  Estado privado
// -----------------------------------------------------------------------------

static UIState estado_actual = UI_STATE_MENU;
static UIState origen_captura = UI_STATE_IDENTIFY;

static uint8_t opcion_menu = 0U;
static uint8_t opcion_dibujada = 0U;
static uint8_t espacio_locutor = 0U;
static VoxIDAudioState estado_audio_dibujado = VOXID_AUDIO_IDLE;
static uint8_t fase_circulos = 0U;
static uint32_t ultimo_tick_circulos = 0U;
static bool captura_pendiente = false;
static uint8_t segundos_dibujados = 3U;
static uint8_t pagina_captura = 0U;
static bool error_guardado = false;

static bool interfaz_lista = false;
static bool redibujar = true;

static const char *const etiquetas_menu[UI_MENU_ITEMS] = {
    "IDENTIFICAR",
    "REGISTRAR",
    "LOCUTORES"
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
    bool escuchando = ui_capture_is_listening();
    bool can_move = estado_actual == UI_STATE_MENU || estado_actual == UI_STATE_ENROLL || estado_actual == UI_STATE_PROFILES || (estado_actual == UI_STATE_CAPTURE && voxid_audio_status() == VOXID_AUDIO_READY);
    bool can_ok = !ui_capture_is_listening();
    bool can_back = estado_actual != UI_STATE_MENU;

    if (!gfx_fill_rect(8U, 112U, 144U, 1U, UI_BORDER)) {
        return false;
    }

    if (!escuchando) {
        if (!ui_draw_footer_button(4U, "UP", can_move)) {
            return false;
        }

        if (!ui_draw_footer_button(44U, "DN", can_move)) {
            return false;
        }

        if (estado_actual != UI_STATE_PROFILES && !ui_draw_footer_button(84U, "OK", can_ok)) {
            return false;
        }
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
    char linea[26];
    uint8_t perfiles = 0U;

    for (uint8_t locutor = 0U; locutor < VOXID_SPEAKER_COUNT; ++locutor) {
        if (voxid_enroll_profile(locutor) != NULL) {
            ++perfiles;
        }
    }

    if (!ui_draw_title("IDENTIFICAR") || !ui_draw_card()) {
        return false;
    }

    (void)snprintf(linea, sizeof(linea), "LOCUTORES: %u/%u", (unsigned)perfiles, (unsigned)VOXID_SPEAKER_COUNT);

    if (!gfx_draw_text(16, 62, linea, UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 80, perfiles > 0U ? "DECIR MARIPOSA" : "SIN PERFILES", UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, "OK: PREPARAR", UI_ACCENT);
}

static bool ui_draw_enroll(void) {
    char slot[26];
    const VoxIDSpeakerProfile *perfil = voxid_enroll_profile(espacio_locutor);

    if (!ui_draw_title("REGISTRAR") || !ui_draw_card()) {
        return false;
    }

    (void)snprintf(slot, sizeof(slot), "LOCUTOR %02u / %02u", (unsigned)(espacio_locutor + 1U), (unsigned)VOXID_SPEAKER_COUNT);

    if (!gfx_draw_text(16, 62, slot, UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 80, perfil != NULL ? "GUARDADO EN EEPROM" : "ESPACIO VACIO", perfil != NULL ? UI_ACCENT : UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, perfil != NULL ? "OK: REGRABAR" : "OK: REGISTRAR", UI_ACCENT);
}

/**
 * @brief Muestra la toma pendiente; OK la acepta o permite repetirla.
 * @param toma Resultado completo de la captura actual.
 * @return true si el resumen se dibuja correctamente.
 */
static bool ui_draw_enroll_take(const VoxIDVoiceStats *toma) {
    char linea[30];
    bool valida = voxid_enroll_take_valid(toma);

    if (error_guardado) {
        return gfx_draw_text(16, 58, "ERROR AL GUARDAR", UI_WARNING) && gfx_draw_text(16, 70, "EEPROM NO CONFIRMA", UI_WARNING) && gfx_draw_text(16, 82, "OK: REINTENTAR", UI_ACCENT) && gfx_draw_text(16, 94, "BACK: CANCELAR", UI_MUTED);
    }

    if (valida) {
        (void)snprintf(linea, sizeof(linea), "FRAMES: %lu/%u", (unsigned long)toma->frames, (unsigned)VOXID_VOICE_EXPECTED_FRAMES);
    } else {
        (void)snprintf(linea, sizeof(linea), "VOZ INSUFICIENTE");
    }

    if (!gfx_draw_text(16, 58, linea, valida ? UI_TEXT : UI_WARNING)) {
        return false;
    }

    (void)snprintf(linea, sizeof(linea), "CON VOZ: %lu", (unsigned long)toma->voiced_frames);

    if (!gfx_draw_text(16, 70, linea, valida ? UI_ACCENT : UI_WARNING)) {
        return false;
    }

    const char *accion = "OK: REPETIR VOZ";

    if (valida) {
        accion = voxid_enroll_count() + 1U == VOXID_PROTOTYPES_PER_SPEAKER ? "OK: GUARDAR" : "OK: SIGUIENTE";
    }

    if (!gfx_draw_text(16, 82, accion, UI_ACCENT)) {
        return false;
    }

    (void)snprintf(linea, sizeof(linea), "ACEPTADAS: %u/%u", (unsigned)voxid_enroll_count(), (unsigned)VOXID_PROTOTYPES_PER_SPEAKER);
    return gfx_draw_text(16, 94, linea, UI_MUTED);
}

/**
 * @brief Presenta identidad o rechazo sin convertir la distancia en confianza.
 * @param toma Captura terminada que se comparara con los perfiles RAM.
 * @return true si el resultado se dibuja correctamente.
 */
static bool ui_draw_identify_result(const VoxIDVoiceStats *toma) {
    VoxIDIdentifyResult resultado;
    char linea[30];

    if (!voxid_identify_run(toma, &resultado)) {
        return false;
    }

    if (resultado.status == VOXID_IDENTIFY_INVALID_VOICE) {
        (void)snprintf(linea, sizeof(linea), "CON VOZ: %lu", (unsigned long)toma->voiced_frames);
        return gfx_draw_text(16, 58, "VOZ INSUFICIENTE", UI_WARNING) && gfx_draw_text(16, 70, linea, UI_MUTED) && gfx_draw_text(16, 94, "OK: REPETIR VOZ", UI_ACCENT);
    }

    if (resultado.status == VOXID_IDENTIFY_NO_PROFILES) {
        return gfx_draw_text(16, 58, "SIN LOCUTORES", UI_WARNING) && gfx_draw_text(16, 76, "REGISTRA PRIMERO", UI_MUTED) && gfx_draw_text(16, 94, "BACK: VOLVER", UI_ACCENT);
    }

    if (resultado.status == VOXID_IDENTIFY_MATCH) {
        (void)snprintf(linea, sizeof(linea), "LOCUTOR %02u", (unsigned)(resultado.locutor + 1U));
    } else {
        (void)snprintf(linea, sizeof(linea), "%s", resultado.status == VOXID_IDENTIFY_AMBIGUOUS ? "VOZ AMBIGUA" : "DESCONOCIDO");
    }

    if (!gfx_draw_text(16, 58, linea, resultado.status == VOXID_IDENTIFY_MATCH ? UI_ACCENT : UI_WARNING)) {
        return false;
    }

    uint32_t distancia = (uint32_t)(resultado.distancia * 1000.0f + 0.5f);
    (void)snprintf(linea, sizeof(linea), "DIST: %lu.%03lu", (unsigned long)(distancia / 1000U), (unsigned long)(distancia % 1000U));

    if (!gfx_draw_text(16, 70, linea, UI_TEXT)) {
        return false;
    }

    if (resultado.perfiles > 1U) {
        uint32_t margen = (uint32_t)(resultado.margen * 1000.0f + 0.5f);
        (void)snprintf(linea, sizeof(linea), "MARGEN: %lu.%03lu", (unsigned long)(margen / 1000U), (unsigned long)(margen % 1000U));
    } else {
        (void)snprintf(linea, sizeof(linea), "UN SOLO PERFIL");
    }

    return gfx_draw_text(16, 82, linea, UI_MUTED) && gfx_draw_text(16, 94, "OK: OTRA TOMA", UI_ACCENT);
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

static bool ui_draw_capture_stats(const VoxIDVoiceStats *mfcc) {
    VoxIDAudioStats adc;
    char linea[30];

    if (!voxid_audio_read_stats(&adc)) {
        return gfx_draw_text(16, 77, "ERROR ESTADISTICAS", UI_WARNING);
    }

    (void)snprintf(linea, sizeof(linea), "FRAMES: %lu/%u", (unsigned long)mfcc->frames, (unsigned)VOXID_VOICE_EXPECTED_FRAMES);

    if (!gfx_draw_text(16, 58, linea, UI_TEXT)) {
        return false;
    }

    (void)snprintf(linea, sizeof(linea), "CON VOZ: %lu", (unsigned long)mfcc->voiced_frames);

    if (!gfx_draw_text(16, 70, linea, mfcc->voiced_frames > 0U ? UI_ACCENT : UI_WARNING)) {
        return false;
    }

    (void)snprintf(linea, sizeof(linea), "MIN:%u MAX:%u", (unsigned)adc.minimum, (unsigned)adc.maximum);

    if (!gfx_draw_text(16, 82, linea, UI_MUTED)) {
        return false;
    }

    (void)snprintf(linea, sizeof(linea), "PROMEDIO:%u", (unsigned)adc.average);
    return gfx_draw_text(16, 94, linea, UI_MUTED);
}

static bool ui_draw_capture(void) {
    char line[30];
    VoxIDAudioState state = captura_pendiente ? VOXID_AUDIO_CAPTURING : voxid_audio_status();
    estado_audio_dibujado = state;

    if (origen_captura == UI_STATE_ENROLL) {
        (void)snprintf(line, sizeof(line), "LOC%02u / TOMA %u/%u", (unsigned)(espacio_locutor + 1U), (unsigned)(voxid_enroll_count() + 1U), (unsigned)VOXID_PROTOTYPES_PER_SPEAKER);
    } else if (state == VOXID_AUDIO_READY && pagina_captura == 1U) {
        (void)snprintf(line, sizeof(line), "MEDIA MFCC / VOZ");
    } else if (state == VOXID_AUDIO_READY && pagina_captura == 2U) {
        (void)snprintf(line, sizeof(line), "CAPTURA / ADC");
    } else if (state == VOXID_AUDIO_READY) {
        (void)snprintf(line, sizeof(line), "RESULTADO / VOXID");
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

        VoxIDVoiceStats mfcc;

        if (!voxid_voice_read_stats(&mfcc)) {
            return gfx_draw_text(16, 77, "ERROR MFCC REAL", UI_WARNING);
        }

        if (pagina_captura == 1U) {
            if (mfcc.voiced_frames == 0U) {
                return gfx_draw_text(16, 75, "SIN VOZ DETECTADA", UI_WARNING);
            }

            // Media de los frames que superan el umbral de energia.
            return ui_draw_mfcc_bars(mfcc.mean_mfcc);
        }

        if (origen_captura == UI_STATE_ENROLL) {
            return ui_draw_enroll_take(&mfcc);
        }

        return pagina_captura == 2U ? ui_draw_capture_stats(&mfcc) : ui_draw_identify_result(&mfcc);
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

    return gfx_draw_text_aligned(80, 102, origen_captura == UI_STATE_ENROLL ? "OK: DECIR MARIPOSA" : "OK: HABLAR 3 SEG", UI_ACCENT, UI_BG, 1U, GFX_ALIGN_CENTER);
}

// =============================================================================
//  Perfiles de locutor: referencias usadas al identificar
// =============================================================================

/**
 * @brief Muestra el estado y las tres referencias del locutor seleccionado.
 * @return true si el dibujo termina correctamente.
 * @note Escala fija -1..1 para C01..C12; el color identifica la toma.
 *       Los valores se obtienen del mismo normalizador de identificacion.
 */
static bool ui_draw_profiles(void) {
    static const uint16_t colores[VOXID_PROTOTYPES_PER_SPEAKER] = {UI_ACCENT, UI_WARNING, UI_PROFILE_THIRD_COLOR};
    const VoxIDSpeakerProfile *perfil = voxid_enroll_profile(espacio_locutor);
    char linea[26];
    (void)snprintf(linea, sizeof(linea), "LOCUTOR %02u / %02u", (unsigned)(espacio_locutor + 1U), (unsigned)VOXID_SPEAKER_COUNT);

    if (!ui_draw_title(linea) || !ui_draw_card()) {
        return false;
    }

    if (!voxid_enroll_profile_valid(perfil)) {
        VoxIDStorageStatus memoria = voxid_storage_status();
        const char *mensaje = "SIN REGISTRAR";
        bool error_memoria = memoria == VOXID_STORAGE_IO_ERROR || memoria == VOXID_STORAGE_CORRUPT || memoria == VOXID_STORAGE_NOT_INITIALIZED;

        if (memoria == VOXID_STORAGE_IO_ERROR) {
            mensaje = "ERROR EEPROM / I2C";
        } else if (memoria == VOXID_STORAGE_CORRUPT) {
            mensaje = "EEPROM: DATOS DANADOS";
        } else if (memoria == VOXID_STORAGE_NOT_INITIALIZED) {
            mensaje = "EEPROM NO INICIADA";
        }

        return gfx_draw_text(10, 46, "SIN PERFIL CARGADO", UI_MUTED) && gfx_draw_text(16, 68, mensaje, error_memoria ? UI_WARNING : UI_MUTED) && gfx_draw_text(16, 88, error_memoria ? "REVISA LA MEMORIA" : "REGISTRA TRES TOMAS", UI_MUTED);
    }

    if (!gfx_draw_text(10, 46, "REGISTRADO / L2", UI_ACCENT) || !gfx_fill_rect(16U, UI_PROFILE_AXIS, UI_GRAPH_WIDTH, 1U, UI_BORDER)) {
        return false;
    }

    float valores[VOXID_MFCC_COUNT];

    for (uint8_t toma = 0U; toma < VOXID_PROTOTYPES_PER_SPEAKER; ++toma) {
        if (!voxid_identify_read_prototype(espacio_locutor, toma, valores)) {
            return false;
        }

        (void)snprintf(linea, sizeof(linea), "T%u", (unsigned)(toma + 1U));

        if (!gfx_draw_text((int16_t)(23U + toma * 43U), 57, linea, colores[toma])) {
            return false;
        }

        for (uint8_t k = 1U; k < VOXID_MFCC_COUNT; ++k) {
            uint16_t alto = (uint16_t)(fabsf(valores[k]) * (float)UI_PROFILE_HALF_HEIGHT + 0.5f);

            if (alto == 0U) {
                continue;
            }

            uint16_t x = (uint16_t)(17U + (k - 1U) * 10U + toma * 3U);
            uint16_t y = valores[k] >= 0.0f ? (uint16_t)(UI_PROFILE_AXIS - alto) : UI_PROFILE_AXIS + 1U;

            if (!gfx_fill_rect(x, y, 2U, alto, colores[toma])) {
                return false;
            }
        }
    }

    return gfx_draw_text(17, 98, "C01", UI_MUTED) && gfx_draw_text(122, 98, "C12", UI_MUTED);
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

        case UI_STATE_PROFILES:
            return ui_draw_profiles();

        case UI_STATE_CAPTURE:
            return ui_draw_capture();

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
                estado_actual = opcion_menu == 0U ? UI_STATE_IDENTIFY : opcion_menu == 1U ? UI_STATE_ENROLL : UI_STATE_PROFILES;
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

            if (origen_captura == UI_STATE_ENROLL) {
                voxid_enroll_cancel();
            }

            captura_pendiente = false;
            error_guardado = false;
            estado_actual = origen_captura;
        } else {
            estado_actual = UI_STATE_MENU;
        }

        redibujar = true;
        return;
    }

    // BACK ya fue atendido. Los demas eventos se ignoran tambien mientras
    // la siguiente toma esta pendiente de arrancar, aunque ADC siga en READY.
    if (ui_capture_is_listening()) {
        return;
    }

    if (estado_actual == UI_STATE_ENROLL || estado_actual == UI_STATE_PROFILES) {
        if (event == BUTTON_UP) {
            espacio_locutor = (uint8_t)((espacio_locutor + VOXID_SPEAKER_COUNT - 1U) % VOXID_SPEAKER_COUNT);
            redibujar = true;
        } else if (event == BUTTON_DOWN) {
            espacio_locutor = (uint8_t)((espacio_locutor + 1U) % VOXID_SPEAKER_COUNT);
            redibujar = true;
        }
    }

    if (estado_actual == UI_STATE_CAPTURE && voxid_audio_status() == VOXID_AUDIO_READY) {
        if (event == BUTTON_UP || event == BUTTON_DOWN) {
            uint8_t paginas = origen_captura == UI_STATE_ENROLL ? 2U : 3U;
            pagina_captura = (uint8_t)((pagina_captura + (event == BUTTON_UP ? paginas - 1U : 1U)) % paginas);
            redibujar = true;
            return;
        }
    }

    if (event != BUTTON_OK) {
        return;
    }

    switch (estado_actual) {
        case UI_STATE_IDENTIFY:
        case UI_STATE_ENROLL:
            if (estado_actual == UI_STATE_ENROLL && !voxid_enroll_begin(espacio_locutor)) {
                break;
            }

            voxid_audio_stop();
            origen_captura = estado_actual;
            estado_actual = UI_STATE_CAPTURE;
            pagina_captura = 0U;
            fase_circulos = 0U;
            ultimo_tick_circulos = HAL_GetTick();
            captura_pendiente = false;
            error_guardado = false;
            redibujar = true;
            break;
        case UI_STATE_CAPTURE:
            if (origen_captura == UI_STATE_ENROLL && voxid_audio_status() == VOXID_AUDIO_READY) {
                VoxIDVoiceStats toma;

                if (voxid_voice_read_stats(&toma)) {
                    VoxIDEnrollResult resultado = voxid_enroll_add(&toma);

                    if (resultado == VOXID_ENROLL_STORAGE_ERROR) {
                        error_guardado = true;
                        pagina_captura = 0U;
                        redibujar = true;
                        break;
                    }

                    if (resultado == VOXID_ENROLL_COMPLETE) {
                        voxid_audio_stop();
                        error_guardado = false;
                        estado_actual = UI_STATE_ENROLL;
                        redibujar = true;
                        break;
                    }
                }
            }

            // Una toma rechazada se repite sin aumentar el contador.
            pagina_captura = 0U;
            fase_circulos = 0U;
            ultimo_tick_circulos = HAL_GetTick();
            captura_pendiente = true;
            error_guardado = false;
            segundos_dibujados = VOXID_AUDIO_DURATION_S;
            redibujar = true;
            break;

        default:
            break;
    }
}

bool ui_init(void) {
    voxid_enroll_cancel();
    estado_actual = UI_STATE_MENU;
    origen_captura = UI_STATE_IDENTIFY;
    opcion_menu = 0U;
    opcion_dibujada = 0U;
    espacio_locutor = 0U;
    estado_audio_dibujado = VOXID_AUDIO_IDLE;
    fase_circulos = 0U;
    ultimo_tick_circulos = HAL_GetTick();
    captura_pendiente = false;
    segundos_dibujados = VOXID_AUDIO_DURATION_S;
    pagina_captura = 0U;
    error_guardado = false;
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

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

// -----------------------------------------------------------------------------
//  Estado privado
// -----------------------------------------------------------------------------

static UIState current_state = UI_STATE_MENU;
static UIState notice_origin = UI_STATE_MENU;

static uint8_t menu_selection = 0U;
static uint8_t rendered_menu_selection = 0U;

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

    if (!gfx_fill_rect(8U, 112U, 144U, 1U, UI_BORDER)) {
        return false;
    }

    const char *footer = current_state == UI_STATE_MENU ? "UP/DN   OK   BACK" : "BACK MENU    OK ESTADO";
    return gfx_draw_text_aligned(80, 117, footer, UI_MUTED, UI_BG, 1U, GFX_ALIGN_CENTER);
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

static bool ui_draw_identify(void) {
    if (!ui_draw_title("IDENTIFICAR") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 63, "CAPTURA DE VOZ", UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 81, "MFCC sin vincular", UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, "OK: ver estado", UI_ACCENT);
}

static bool ui_draw_enroll(void) {
    if (!ui_draw_title("REGISTRAR") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 63, "NUEVO HABLANTE", UI_TEXT)) {
        return false;
    }

    if (!gfx_draw_text(16, 81, "Registro pendiente", UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, "OK: ver estado", UI_ACCENT);
}

static bool ui_draw_system(void) {
    char diagnostic[32];

    if (!ui_draw_title("DIAGNOSTICO") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 62, "STM32F407 / 168M", UI_TEXT)) {
        return false;
    }

    bool dsp_ok = voxid_dsp_is_ready();
    const char *dsp_status = dsp_ok ? "MFCC: LISTO" : "MFCC: ERROR";

    if (!gfx_draw_text(16, 80, dsp_status, dsp_ok ? UI_ACCENT : UI_WARNING)) {
        return false;
    }

    (void)snprintf(diagnostic, sizeof(diagnostic), "ADC OFF | Perd: %lu", (unsigned long)buttons_dropped());
    return gfx_draw_text(16, 94, diagnostic, UI_MUTED);
}

static bool ui_draw_notice(void) {
    if (!ui_draw_title("ESTADO DEL MODULO") || !ui_draw_card()) {
        return false;
    }

    if (!gfx_draw_text(16, 62, "AUN NO DISPONIBLE", UI_WARNING)) {
        return false;
    }

    const char *reason = notice_origin == UI_STATE_IDENTIFY ? "MFCC por conectar" : "Registro por crear";

    if (!gfx_draw_text(16, 80, reason, UI_MUTED)) {
        return false;
    }

    return gfx_draw_text(16, 94, "BACK: regresar", UI_ACCENT);
}

// -----------------------------------------------------------------------------
//  Renderizado por estado
// -----------------------------------------------------------------------------

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

        case UI_STATE_NOTICE:
            return ui_draw_notice();

        default:
            current_state = UI_STATE_MENU;
            return ui_draw_menu();
    }
}

// -----------------------------------------------------------------------------
//  Navegacion
// -----------------------------------------------------------------------------

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
                current_state = menu_selection == 0U ? UI_STATE_IDENTIFY :
                                menu_selection == 1U ? UI_STATE_ENROLL : UI_STATE_SYSTEM;
                needs_redraw = true;
                break;

            default:
                break;
        }
        return;
    }

    if (event == BUTTON_BACK) {
        current_state = current_state == UI_STATE_NOTICE ? notice_origin : UI_STATE_MENU;
        needs_redraw = true;
    } else if (event == BUTTON_OK) {
        if (current_state == UI_STATE_IDENTIFY || current_state == UI_STATE_ENROLL) {
            notice_origin = current_state;
            current_state = UI_STATE_NOTICE;
            needs_redraw = true;
        } else if (current_state == UI_STATE_SYSTEM) {
            needs_redraw = true;
        }
    }
}

// -----------------------------------------------------------------------------
//  API publica
// -----------------------------------------------------------------------------

bool ui_init(void) {
    current_state = UI_STATE_MENU;
    notice_origin = UI_STATE_MENU;
    menu_selection = 0U;
    rendered_menu_selection = 0U;
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

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Src/ui.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  UI para Ophyra, orientacion horizontal 160 x 128.
 *  Tema oscuro, acentos turquesa y navegacion por cuatro botones.
 *
 *  Las funciones de identificar y registrar muestran su disponibilidad real:
 *  el reconocimiento de hablantes aun no esta conectado al firmware.
 *
 * =============================================================================
 */

#include "ui.h"

#include "buttons.h"
#include "TFT_Ophyra.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/* La libreria de Ophyra recibe colores BGR de 24 bits. */
#define UI_BGR(r, g, b)  (((uint32_t)(b) << 16) | ((uint32_t)(g) << 8) | (uint32_t)(r))

#define UI_BG       UI_BGR(10, 16, 26)
#define UI_PANEL    UI_BGR(20, 34, 48)
#define UI_ACTIVE   UI_BGR(18, 58, 62)
#define UI_BORDER   UI_BGR(40, 65, 77)
#define UI_WHITE    UI_BGR(240, 246, 248)
#define UI_MUTED    UI_BGR(133, 160, 169)
#define UI_ACCENT   UI_BGR(59, 229, 193)
#define UI_WARNING  UI_BGR(255, 190, 100)

#define UI_WIDTH    160U
#define UI_HEIGHT   128U
#define MENU_ITEMS    3U

static UIState current_state = UI_STATE_MENU;
static UIState notice_origin = UI_STATE_MENU;
static uint8_t menu_selection = 0U;
static bool needs_redraw = true;

/* Adaptador al driver heredado, cuya interfaz usa CHAR* no const. */
static void ui_text(uint8_t x, uint8_t y, const char *message,
                    uint32_t color, uint32_t background)
{
    (void)TFT_PrintStr(x, y, (CHAR *)message, color, background);
}

static void ui_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint32_t color)
{
    (void)TFT_PrintLine(x1, y1, x2, y2, color);
}

/* Relleno basado en lineas: la biblioteca no expone rectangulos solidos. */
static void ui_fill(uint8_t x, uint8_t y, uint8_t width, uint8_t height,
                    uint32_t color)
{
    if (width == 0U || height == 0U) {
        return;
    }

    uint8_t x2 = (uint8_t)(x + width - 1U);
    for (uint16_t row = y; row < (uint16_t)y + height; ++row) {
        ui_line(x, (uint8_t)row, x2, (uint8_t)row, color);
    }
}

static void ui_chrome(void)
{
    (void)TFT_ClearDisplay(UI_BG);
    ui_fill(0U, 0U, 3U, 27U, UI_ACCENT);

    ui_text(9U, 7U, "VOXID", UI_WHITE, UI_BG);
    ui_text(103U, 7U, "OPHYRA", UI_ACCENT, UI_BG);

    ui_line(8U, 26U, 151U, 26U, UI_BORDER);
    ui_line(8U, 111U, 151U, 111U, UI_BORDER);
    ui_text(8U, 116U, "UP/DN  OK  BACK", UI_MUTED, UI_BG);
}

static void ui_title(const char *title)
{
    ui_text(8U, 33U, title, UI_ACCENT, UI_BG);
}

static void ui_menu(void)
{
    static const char *const labels[MENU_ITEMS] = {
        "01 IDENTIFICAR", "02 REGISTRAR", "03 DIAGNOSTICO"
    };

    ui_title("MENU PRINCIPAL");

    for (uint8_t i = 0U; i < MENU_ITEMS; ++i) {
        const uint8_t top = (uint8_t)(47U + i * 21U);
        const bool selected = (i == menu_selection);
        const uint32_t bg = selected ? UI_ACTIVE : UI_PANEL;
        const uint32_t fg = selected ? UI_WHITE : UI_MUTED;

        ui_fill(7U, top, 146U, 19U, bg);
        if (selected) {
            ui_fill(7U, top, 3U, 19U, UI_ACCENT);
        }
        ui_text(15U, (uint8_t)(top + 4U), labels[i], fg, bg);
    }
}

static void ui_identify(void)
{
    ui_title("IDENTIFICAR");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 57U, "CAPTURA DE VOZ", UI_WHITE, UI_BG);
    ui_text(8U, 76U, "Motor no vinculado", UI_MUTED, UI_BG);
    ui_text(8U, 94U, "OK: ver estado", UI_ACCENT, UI_BG);
}

static void ui_enroll(void)
{
    ui_title("REGISTRAR");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 57U, "NUEVO HABLANTE", UI_WHITE, UI_BG);
    ui_text(8U, 76U, "Registro pendiente", UI_MUTED, UI_BG);
    ui_text(8U, 94U, "OK: ver estado", UI_ACCENT, UI_BG);
}

static void ui_system(void)
{
    char diagnostic[26];

    ui_title("DIAGNOSTICO");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 56U, "STM32F407 / 168M", UI_WHITE, UI_BG);
    ui_text(8U, 74U, "ADC: sin iniciar", UI_MUTED, UI_BG);

    (void)snprintf(diagnostic, sizeof(diagnostic),
                   "Eventos perd.: %lu", (unsigned long)buttons_dropped());
    ui_text(8U, 93U, diagnostic, UI_ACCENT, UI_BG);
}

static void ui_notice(void)
{
    ui_title("ESTADO DEL MODULO");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 57U, "AUN NO DISPONIBLE", UI_WARNING, UI_BG);

    if (notice_origin == UI_STATE_IDENTIFY) {
        ui_text(8U, 77U, "MFCC por conectar", UI_MUTED, UI_BG);
    } else {
        ui_text(8U, 77U, "Registro por crear", UI_MUTED, UI_BG);
    }

    ui_text(8U, 94U, "BACK: regresar", UI_ACCENT, UI_BG);
}

static void ui_render(void)
{
    ui_chrome();

    switch (current_state) {
        case UI_STATE_MENU:
            ui_menu();
            break;
        case UI_STATE_IDENTIFY:
            ui_identify();
            break;
        case UI_STATE_ENROLL:
            ui_enroll();
            break;
        case UI_STATE_SYSTEM:
            ui_system();
            break;
        case UI_STATE_NOTICE:
            ui_notice();
            break;
        default:
            current_state = UI_STATE_MENU;
            ui_menu();
            break;
    }

    needs_redraw = false;
}

static void ui_dispatch(ButtonEvent event)
{
    if (event == BUTTON_NONE) {
        return;
    }

    if (current_state == UI_STATE_MENU) {
        switch (event) {
            case BUTTON_UP:
                menu_selection = (uint8_t)((menu_selection + MENU_ITEMS - 1U) % MENU_ITEMS);
                needs_redraw = true;
                break;
            case BUTTON_DOWN:
                menu_selection = (uint8_t)((menu_selection + 1U) % MENU_ITEMS);
                needs_redraw = true;
                break;
            case BUTTON_OK:
                current_state = (menu_selection == 0U) ? UI_STATE_IDENTIFY :
                                (menu_selection == 1U) ? UI_STATE_ENROLL : UI_STATE_SYSTEM;
                needs_redraw = true;
                break;
            default:
                break;
        }
        return;
    }

    if (event == BUTTON_BACK) {
        current_state = (current_state == UI_STATE_NOTICE) ?
                        notice_origin : UI_STATE_MENU;
        needs_redraw = true;
    } else if (event == BUTTON_OK) {
        if (current_state == UI_STATE_IDENTIFY || current_state == UI_STATE_ENROLL) {
            notice_origin = current_state;
            current_state = UI_STATE_NOTICE;
            needs_redraw = true;
        } else if (current_state == UI_STATE_SYSTEM) {
            needs_redraw = true; /* Actualizar diagnostico al presionar OK. */
        }
    }
}

void ui_init(void)
{
    current_state = UI_STATE_MENU;
    notice_origin = UI_STATE_MENU;
    menu_selection = 0U;

    (void)TFT_Init(0U); /* 0: horizontal, 160 x 128 */
    (void)pfTFT_BACKLIGHT_GPIO(1U);

    needs_redraw = true;
    ui_render();
}

void ui_task(void)
{
    ButtonEvent event;

    while (buttons_next_event(&event)) {
        ui_dispatch(event);
    }

    /* El SPI de la pantalla se utiliza solo fuera de las ISR. */
    if (needs_redraw) {
        ui_render();
    }
}

UIState ui_state(void)
{
    return current_state;
}

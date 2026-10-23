/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Src/ui.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  UI para Ophyra, orientacion horizontal 160 x 128.
 *  Tema pastel claro y navegacion por cuatro botones.
 *  El menu solo actualiza los indicadores y textos que cambian.
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

/* Colores pastel RGB; UI_BGR conserva el orden que utiliza el driver. */
#define UI_BG       UI_BGR(251, 248, 245)   /* marfil */
#define UI_PANEL    UI_BGR(237, 230, 245)   /* lavanda */
#define UI_BORDER   UI_BGR(207, 196, 219)   /* lila suave */
#define UI_TEXT     UI_BGR(70, 62, 88)     /* ciruela oscuro */
#define UI_MUTED    UI_BGR(108, 101, 124)  /* gris violeta */
#define UI_ACCENT   UI_BGR(55, 139, 125)   /* verde menta oscuro */
#define UI_MARKER   UI_BGR(159, 220, 198)  /* menta pastel */
#define UI_WARNING  UI_BGR(177, 99, 99)    /* coral oscuro */

#define UI_WIDTH    160U
#define UI_HEIGHT   128U
#define MENU_ITEMS    3U

static UIState current_state = UI_STATE_MENU;
static UIState notice_origin = UI_STATE_MENU;
static uint8_t menu_selection = 0U;
static uint8_t rendered_menu_selection = 0U;
static bool needs_redraw = true;

static const char *const menu_labels[MENU_ITEMS] = {
    "01 IDENTIFICAR", "02 REGISTRAR", "03 DIAGNOSTICO"
};

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

    ui_text(9U, 7U, "VOXID", UI_TEXT, UI_BG);
    ui_text(103U, 7U, "OPHYRA", UI_ACCENT, UI_BG);

    ui_line(8U, 26U, 151U, 26U, UI_BORDER);
    ui_line(8U, 111U, 151U, 111U, UI_BORDER);
    ui_text(8U, 116U, "UP/DN  OK  BACK", UI_MUTED, UI_BG);
}

static void ui_title(const char *title)
{
    ui_text(8U, 33U, title, UI_ACCENT, UI_BG);
}

/*
 * El fondo de todas las filas es constante. En cada movimiento solo cambian
 * la barra lateral y el color del texto, sin repintar el panel completo.
 */
static void ui_draw_menu_item(uint8_t index, bool selected, bool first_draw)
{
    const uint8_t top = (uint8_t)(47U + index * 21U);

    if (first_draw) {
        ui_fill(7U, top, 146U, 19U, UI_PANEL);
    }

    ui_fill(7U, top, 3U, 19U, selected ? UI_MARKER : UI_PANEL);
    ui_text(15U, (uint8_t)(top + 4U), menu_labels[index],
            selected ? UI_ACCENT : UI_MUTED, UI_PANEL);
}

static void ui_menu(void)
{
    ui_title("MENU PRINCIPAL");

    for (uint8_t i = 0U; i < MENU_ITEMS; ++i) {
        ui_draw_menu_item(i, i == menu_selection, true);
    }

    rendered_menu_selection = menu_selection;
}

/* Redibujar solamente las dos opciones cuyo estado visual cambia. */
static void ui_update_menu_selection(void)
{
    ui_draw_menu_item(rendered_menu_selection, false, false);
    ui_draw_menu_item(menu_selection, true, false);
    rendered_menu_selection = menu_selection;
}

static void ui_identify(void)
{
    ui_title("IDENTIFICAR");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 57U, "CAPTURA DE VOZ", UI_TEXT, UI_BG);
    ui_text(8U, 76U, "Motor no vinculado", UI_MUTED, UI_BG);
    ui_text(8U, 94U, "OK: ver estado", UI_ACCENT, UI_BG);
}

static void ui_enroll(void)
{
    ui_title("REGISTRAR");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 57U, "NUEVO HABLANTE", UI_TEXT, UI_BG);
    ui_text(8U, 76U, "Registro pendiente", UI_MUTED, UI_BG);
    ui_text(8U, 94U, "OK: ver estado", UI_ACCENT, UI_BG);
}

static void ui_system(void)
{
    char diagnostic[26];

    ui_title("DIAGNOSTICO");
    ui_line(8U, 49U, 151U, 49U, UI_BORDER);
    ui_text(8U, 56U, "STM32F407 / 168M", UI_TEXT, UI_BG);
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
                break;
            case BUTTON_DOWN:
                menu_selection = (uint8_t)((menu_selection + 1U) % MENU_ITEMS);
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
    rendered_menu_selection = 0U;

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
    } else if (current_state == UI_STATE_MENU &&
               menu_selection != rendered_menu_selection) {
        ui_update_menu_selection();
    }
}

UIState ui_state(void)
{
    return current_state;
}

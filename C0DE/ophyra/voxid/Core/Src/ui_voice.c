/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/ui_voice.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Animacion no bloqueante de tres anillos concentricos inspirados en el
 *  indicador visual de VoxID: marfil, lavanda y turquesa pastel.
 *
 * =============================================================================
 */

#include "ui_voice.h"

#include "gfx.h"

// =============================================================================
//  Posicion y paleta grafica
// =============================================================================

#define VOICE_CENTER_X      80
#define VOICE_CENTER_Y      74
#define VOICE_AREA_X        48U
#define VOICE_AREA_Y        45U
#define VOICE_AREA_WIDTH    64U
#define VOICE_AREA_HEIGHT   56U

#define VOICE_OUTER_COLOR   0xDEDFU
#define VOICE_MIDDLE_COLOR  0xAB77U
#define VOICE_INNER_COLOR   0x4658U

static const uint8_t voice_outer_radius[UI_VOICE_PHASES] = {23U, 24U, 25U, 24U};
static const uint8_t voice_middle_radius[UI_VOICE_PHASES] = {15U, 16U, 17U, 16U};
static const uint8_t voice_inner_radius[UI_VOICE_PHASES] = {7U, 8U, 9U, 8U};

// =============================================================================
//  Dibujar una fase de los circulos
// =============================================================================

bool ui_voice_draw(uint8_t phase, uint16_t background) {
    if (phase >= UI_VOICE_PHASES) {
        return false;
    }

    // Redibujar unicamente el espacio de la animacion: nunca tocar footer o titulo.
    if (!gfx_fill_rect(VOICE_AREA_X, VOICE_AREA_Y, VOICE_AREA_WIDTH, VOICE_AREA_HEIGHT, background)) {
        return false;
    }

    if (!gfx_draw_circle(VOICE_CENTER_X, VOICE_CENTER_Y, voice_outer_radius[phase], VOICE_OUTER_COLOR)) {
        return false;
    }

    if (!gfx_draw_circle(VOICE_CENTER_X, VOICE_CENTER_Y, voice_middle_radius[phase], VOICE_MIDDLE_COLOR)) {
        return false;
    }

    return gfx_draw_circle(VOICE_CENTER_X, VOICE_CENTER_Y, voice_inner_radius[phase], VOICE_INNER_COLOR);
}

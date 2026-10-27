/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_gfx.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Primitivas GFX sobre memoria; comprueba recorte de coordenadas negativas.
 *
 * =============================================================================
 */

#include "gfx.h"
#include "st7735.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

static uint16_t pixeles[ST7735_HEIGHT][ST7735_WIDTH];

bool st7735_fill_screen(uint16_t color) {
    for (uint16_t y = 0U; y < ST7735_HEIGHT; ++y) {
        for (uint16_t x = 0U; x < ST7735_WIDTH; ++x) {
            pixeles[y][x] = color;
        }
    }
    return true;
}

bool st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    assert(x < ST7735_WIDTH && y < ST7735_HEIGHT);
    pixeles[y][x] = color;
    return true;
}

bool st7735_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    assert((uint32_t)x + width <= ST7735_WIDTH && (uint32_t)y + height <= ST7735_HEIGHT);
    for (uint16_t fila = y; fila < y + height; ++fila) {
        for (uint16_t columna = x; columna < x + width; ++columna) {
            pixeles[fila][columna] = color;
        }
    }
    return true;
}

int main(void) {
    assert(gfx_fill_screen(0U));
    assert(gfx_draw_text_scaled_bg(-2, 0, " ", 1U, 2U, 1U));
    assert(pixeles[0][0] == 2U && pixeles[0][2] == 2U && pixeles[0][3] == 0U);

    assert(gfx_draw_line(-10, 10, 10, 10, 3U));
    for (uint16_t x = 0U; x <= 10U; ++x) {
        assert(pixeles[10][x] == 3U);
    }

    assert(gfx_fill_circle(0, 0, 5U, 4U));
    assert(pixeles[0][0] == 4U);
    assert(gfx_draw_circle(159, 127, 8U, 5U));
    assert(gfx_fill_round_rect(-5, -5, 20U, 20U, 4U, 6U));
    assert(gfx_draw_text_scaled(-2, -2, "A", 7U, 2U));
    assert(!gfx_draw_text(0, 0, NULL, 0U));
    assert(!gfx_draw_char_scaled(0, 0, 'A', 0U, 0U));

    puts("OK: GFX, recorte negativo, texto, lineas y circulos dentro de pantalla.");
    return 0;
}

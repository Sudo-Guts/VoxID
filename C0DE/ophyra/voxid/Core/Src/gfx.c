/*
* =============================================================================
*  Proyecto : VoxID
*  Archivo  : gfx.c
*  Autor    : Gustavo Ruiz Luis
* =============================================================================
*
*  Primitivas graficas para VoxID.
*
* =============================================================================
*/

#include "gfx.h"
#include "st7735.h"

// -----------------------------------------------------------------------------
//  Operaciones basicas
// -----------------------------------------------------------------------------

bool gfx_fill_screen(uint16_t color) {
    return st7735_fill_screen(color);
}

bool gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    return st7735_draw_pixel(x, y, color);
}

bool gfx_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    return st7735_fill_rect(x, y, width, height, color);
}

// -----------------------------------------------------------------------------
//  Dibujar linea - Algoritmo de Bresenham
// -----------------------------------------------------------------------------

bool gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int32_t x = x0;
    int32_t y = y0;

    int32_t dx = (int32_t)x1 - x0;
    int32_t dy = (int32_t)y1 - y0;

    int32_t sx = dx >= 0 ? 1 : -1;
    int32_t sy = dy >= 0 ? 1 : -1;

    dx = dx >= 0 ? dx : -dx;
    dy = dy <= 0 ? dy : -dy;

    int32_t error = dx + dy;

    while (true) {
        if (x >= 0 && x < ST7735_WIDTH && y >= 0 && y < ST7735_HEIGHT) {
            if (!gfx_draw_pixel((uint16_t)x, (uint16_t)y, color)) {
                return false;
            }
        }

        if (x == x1 && y == y1) {
            break;
        }

        int32_t error2 = 2 * error;

        if (error2 >= dy) {
            error += dy;
            x += sx;
        }

        if (error2 <= dx) {
            error += dx;
            y += sy;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
//  Dibujar rectangulo
// -----------------------------------------------------------------------------

bool gfx_draw_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if (width == 0U || height == 0U) {
        return true;
    }

    int32_t x1 = (int32_t)x + width - 1;
    int32_t y1 = (int32_t)y + height - 1;

    // Evitar coordenadas fuera del rango int16_t.
    if (x1 > INT16_MAX || y1 > INT16_MAX) {
        return false;
    }

    // Dibujar los cuatro lados.
    if (!gfx_draw_line(x, y, (int16_t)x1, y, color)) {
        return false;
    }

    if (!gfx_draw_line((int16_t)x1, y, (int16_t)x1, (int16_t)y1, color)) {
        return false;
    }

    if (!gfx_draw_line((int16_t)x1, (int16_t)y1, x, (int16_t)y1, color)) {
        return false;
    }

    return gfx_draw_line(x, (int16_t)y1, x, y, color);
}

// -----------------------------------------------------------------------------
//  Dibujar pixel con recorte de coordenadas
// -----------------------------------------------------------------------------

static bool gfx_plot_pixel(int32_t x, int32_t y, uint16_t color) {
    if (x < 0 || x >= ST7735_WIDTH || y < 0 || y >= ST7735_HEIGHT) {
        return true;
    }

    return gfx_draw_pixel((uint16_t)x, (uint16_t)y, color);
}

// -----------------------------------------------------------------------------
//  Dibujar circulo - Algoritmo del punto medio
// -----------------------------------------------------------------------------

bool gfx_draw_circle(int16_t cx, int16_t cy, uint16_t radius, uint16_t color) {
    int32_t x = 0;
    int32_t y = radius;
    int32_t decision = 1 - y;

    if (radius == 0U) {
        return gfx_plot_pixel(cx, cy, color);
    }

    while (x <= y) {

        // Dibujar los ocho octantes del circulo.
        if (!gfx_plot_pixel(cx + x, cy + y, color) ||
            !gfx_plot_pixel(cx - x, cy + y, color) ||
            !gfx_plot_pixel(cx + x, cy - y, color) ||
            !gfx_plot_pixel(cx - x, cy - y, color) ||
            !gfx_plot_pixel(cx + y, cy + x, color) ||
            !gfx_plot_pixel(cx - y, cy + x, color) ||
            !gfx_plot_pixel(cx + y, cy - x, color) ||
            !gfx_plot_pixel(cx - y, cy - x, color)) {
            return false;
        }

        x++;

        if (decision < 0) {
            decision += 2 * x + 1;
        } else {
            y--;
            decision += 2 * (x - y) + 1;
        }
    }

    return true;
}

// -----------------------------------------------------------------------------
//  Dibujar linea horizontal con recorte
// -----------------------------------------------------------------------------

static bool gfx_horizontal_span(int32_t x0, int32_t x1, int32_t y, uint16_t color) {
    if (y < 0 || y >= ST7735_HEIGHT || x1 < 0 || x0 >= ST7735_WIDTH) {
        return true;
    }

    if (x0 < 0) {
        x0 = 0;
    }

    if (x1 >= ST7735_WIDTH) {
        x1 = ST7735_WIDTH - 1;
    }

    return gfx_fill_rect((uint16_t)x0, (uint16_t)y, (uint16_t)(x1 - x0 + 1), 1U, color);
}

// -----------------------------------------------------------------------------
//  Dibujar circulo relleno - Algoritmo del punto medio
// -----------------------------------------------------------------------------

bool gfx_fill_circle(int16_t cx, int16_t cy, uint16_t radius, uint16_t color) {
    int32_t x = 0;
    int32_t y = radius;
    int32_t decision = 1 - y;

    while (x <= y) {
        if (!gfx_horizontal_span(cx - x, cx + x, cy + y, color) ||
            !gfx_horizontal_span(cx - x, cx + x, cy - y, color) ||
            !gfx_horizontal_span(cx - y, cx + y, cy + x, color) ||
            !gfx_horizontal_span(cx - y, cx + y, cy - x, color)) {
            return false;
        }

        x++;

        if (decision < 0) {
            decision += 2 * x + 1;
        } else {
            y--;
            decision += 2 * (x - y) + 1;
        }
    }

    return true;
}

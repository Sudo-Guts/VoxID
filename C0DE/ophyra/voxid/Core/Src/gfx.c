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
#include "font5x7.h"

#include <stddef.h>

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

// -----------------------------------------------------------------------------
//  Dibujar bloque de pixel escalado
// -----------------------------------------------------------------------------

static bool gfx_scaled_pixel(int32_t x, int32_t y, uint8_t scale, uint16_t color) {
   if (scale == 1U) {
       return gfx_plot_pixel(x, y, color);
   }

   int32_t x0 = x;
   int32_t y0 = y;
   int32_t x1 = x + scale;
   int32_t y1 = y + scale;

   // Descartar bloques completamente fuera de pantalla.
   if (x1 <= 0 || y1 <= 0 || x0 >= ST7735_WIDTH || y0 >= ST7735_HEIGHT) {
       return true;
   }

   // Recortar el bloque a los limites de pantalla.
   if (x0 < 0) {
       x0 = 0;
   }

   if (y0 < 0) {
       y0 = 0;
   }

   if (x1 > ST7735_WIDTH) {
       x1 = ST7735_WIDTH;
   }

   if (y1 > ST7735_HEIGHT) {
       y1 = ST7735_HEIGHT;
   }

   return gfx_fill_rect((uint16_t)x0, (uint16_t)y0, (uint16_t)(x1 - x0), (uint16_t)(y1 - y0), color);
}

// -----------------------------------------------------------------------------
//  Dibujar caracter escalado
// -----------------------------------------------------------------------------

bool gfx_draw_char_scaled(int16_t x, int16_t y, char character, uint16_t color, uint8_t scale) {
   if (scale == 0U) {
       return false;
   }

   uint8_t ascii = (uint8_t)character;

   if (ascii < FONT5X7_FIRST_CHAR || ascii > FONT5X7_LAST_CHAR) {
       ascii = '?';
   }

   uint8_t index = ascii - FONT5X7_FIRST_CHAR;

   for (uint8_t col = 0U; col < FONT5X7_WIDTH; ++col) {
       uint8_t column = font5x7[index][col];

       for (uint8_t row = 0U; row < FONT5X7_HEIGHT; ++row) {
           if ((column & (1U << row)) != 0U) {
               int32_t px = (int32_t)x + (int32_t)col * scale;
               int32_t py = (int32_t)y + (int32_t)row * scale;

               if (!gfx_scaled_pixel(px, py, scale, color)) {
                   return false;
               }
           }
       }
   }

   return true;
}

// -----------------------------------------------------------------------------
//  Dibujar texto escalado
// -----------------------------------------------------------------------------

bool gfx_draw_text_scaled(int16_t x, int16_t y, const char *text, uint16_t color, uint8_t scale) {
   if (text == NULL || scale == 0U) {
       return false;
   }

   int32_t cursor_x = x;
   int32_t cursor_y = y;

   const int32_t advance = (FONT5X7_WIDTH + FONT5X7_SPACING) * scale;
   const int32_t line_step = (FONT5X7_HEIGHT + FONT5X7_SPACING) * scale;

   while (*text != '\0') {
       if (*text == '\n') {
           cursor_x = x;
           cursor_y += line_step;
       } else if (*text != '\r') {
           // Dibujar caracteres que puedan ser visibles.
           if (cursor_x > -(int32_t)FONT5X7_WIDTH * scale && cursor_x < ST7735_WIDTH &&
               cursor_y > -(int32_t)FONT5X7_HEIGHT * scale && cursor_y < ST7735_HEIGHT) {

               if (!gfx_draw_char_scaled((int16_t)cursor_x, (int16_t)cursor_y, *text, color, scale)) {
                   return false;
               }
           }

           cursor_x += advance;
       }

       ++text;
   }

   return true;
}

// -----------------------------------------------------------------------------
//  Texto de tamaño normal
// -----------------------------------------------------------------------------

bool gfx_draw_char(int16_t x, int16_t y, char character, uint16_t color) {
   return gfx_draw_char_scaled(x, y, character, color, 1U);
}

bool gfx_draw_text(int16_t x, int16_t y, const char *text, uint16_t color) {
   return gfx_draw_text_scaled(x, y, text, color, 1U);
}

// -----------------------------------------------------------------------------
//  Rectangulo con recorte de coordenadas
// -----------------------------------------------------------------------------

static bool gfx_fill_rect_clipped(int32_t x, int32_t y, int32_t width, int32_t height, uint16_t color) {
   if (width <= 0 || height <= 0) {
       return true;
   }

   int32_t x0 = x;
   int32_t y0 = y;
   int32_t x1 = x + width;
   int32_t y1 = y + height;

   // Descartar regiones fuera de pantalla.
   if (x1 <= 0 || y1 <= 0 || x0 >= ST7735_WIDTH || y0 >= ST7735_HEIGHT) {
       return true;
   }

   // Ajustar los limites visibles.
   if (x0 < 0) {
       x0 = 0;
   }

   if (y0 < 0) {
       y0 = 0;
   }

   if (x1 > ST7735_WIDTH) {
       x1 = ST7735_WIDTH;
   }

   if (y1 > ST7735_HEIGHT) {
       y1 = ST7735_HEIGHT;
   }

   return gfx_fill_rect((uint16_t)x0, (uint16_t)y0, (uint16_t)(x1 - x0), (uint16_t)(y1 - y0), color);
}

// -----------------------------------------------------------------------------
//  Texto escalable con fondo
// -----------------------------------------------------------------------------

bool gfx_draw_text_scaled_bg(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale)
{
    if (text == NULL || scale == 0U) {
        return false;
    }

    int32_t cursor_x = x;
    int32_t cursor_y = y;

    const int32_t glyph_w = FONT5X7_WIDTH * scale;
    const int32_t glyph_h = FONT5X7_HEIGHT * scale;
    const int32_t advance = (FONT5X7_WIDTH + FONT5X7_SPACING) * scale;
    const int32_t line_step = (FONT5X7_HEIGHT + FONT5X7_SPACING) * scale;

    while (*text != '\0') {
        if (*text == '\n') {
            cursor_x = x;
            cursor_y += line_step;
        } else if (*text != '\r') {
            if (!gfx_fill_rect_clipped(cursor_x - 1, cursor_y - 1, advance, glyph_h + 2, bg)) {
                return false;
            }

            if (!gfx_draw_char_scaled((int16_t)cursor_x, (int16_t)cursor_y, *text, color, scale)) {
                return false;
            }

            cursor_x += advance;
        }

        ++text;
    }

    return true;
}
// -----------------------------------------------------------------------------
//  Texto normal con fondo
// -----------------------------------------------------------------------------

bool gfx_draw_text_bg(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg) {
   return gfx_draw_text_scaled_bg(x, y, text, color, bg, 1U);
}

// -----------------------------------------------------------------------------
//  Calcular ancho de una linea de texto
// -----------------------------------------------------------------------------

static int32_t gfx_text_line_width(const char *text, uint8_t scale) {
   int32_t count = 0;

   while (*text != '\0' && *text != '\n') {
       if (*text != '\r') {
           count++;
       }

       text++;
   }

   if (count == 0) {
       return 0;
   }

   const int32_t advance = (FONT5X7_WIDTH + FONT5X7_SPACING) * scale;
   return count * advance - FONT5X7_SPACING * scale;
}

// -----------------------------------------------------------------------------
//  Dibujar texto alineado
// -----------------------------------------------------------------------------

bool gfx_draw_text_aligned(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale, gfx_align_t align) {
   if (text == NULL || scale == 0U) {
       return false;
   }

   if (align != GFX_ALIGN_LEFT && align != GFX_ALIGN_CENTER && align != GFX_ALIGN_RIGHT) {
       return false;
   }

   const int32_t advance = (FONT5X7_WIDTH + FONT5X7_SPACING) * scale;
   const int32_t line_step = (FONT5X7_HEIGHT + FONT5X7_SPACING) * scale;

   int32_t cursor_y = y;

   while (*text != '\0') {
       int32_t width = gfx_text_line_width(text, scale);
       int32_t cursor_x = x;

       // Calcular posicion inicial segun alineacion.
       switch (align) {
           case GFX_ALIGN_CENTER:
               cursor_x -= width / 2;
               break;

           case GFX_ALIGN_RIGHT:
               cursor_x -= width;
               break;

           case GFX_ALIGN_LEFT:
           default:
               break;
       }

       // Dibujar los caracteres de la linea actual.
       while (*text != '\0' && *text != '\n') {
           if (*text != '\r') {
               if (cursor_x >= INT16_MIN && cursor_x <= INT16_MAX &&
                   cursor_y >= INT16_MIN && cursor_y <= INT16_MAX) {

                   char character[2] = {*text, '\0'};

                   if (!gfx_draw_text_scaled_bg((int16_t)cursor_x, (int16_t)cursor_y, character, color, bg, scale)) {
                       return false;
                   }
               }

               cursor_x += advance;
           }

           text++;
       }

       // Avanzar a la siguiente linea.
       if (*text == '\n') {
           text++;
           cursor_y += line_step;
       }
   }

   return true;
}

// -----------------------------------------------------------------------------
//  Rectangulos redondeados - Algoritmo del punto medio
// -----------------------------------------------------------------------------

static bool gfx_round_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t color, bool filled) {
   if (width == 0U || height == 0U) {
       return true;
   }

   // Limitar el radio a las dimensiones del rectangulo.
   uint16_t min_size = width < height ? width : height;
   uint16_t max_radius = (min_size - 1U) / 2U;

   if (radius > max_radius) {
       radius = max_radius;
   }

   // Un radio de cero produce un rectangulo normal.
   if (radius == 0U) {
       return filled ? gfx_fill_rect_clipped(x, y, width, height, color) : gfx_draw_rect(x, y, width, height, color);
   }

   // Limites y centros de las cuatro esquinas.
   int32_t left = x;
   int32_t top = y;
   int32_t right = left + width - 1;
   int32_t bottom = top + height - 1;

   int32_t cx0 = left + radius;
   int32_t cx1 = right - radius;
   int32_t cy0 = top + radius;
   int32_t cy1 = bottom - radius;

   // Dibujar la parte rectangular.
   if (filled) {
       if (!gfx_fill_rect_clipped(left, cy0, width, height - 2U * radius, color)) {
           return false;
       }
   } else {
       if (!gfx_horizontal_span(cx0, cx1, top, color) ||
           !gfx_horizontal_span(cx0, cx1, bottom, color) ||
           !gfx_fill_rect_clipped(left, cy0, 1U, height - 2U * radius, color) ||
           !gfx_fill_rect_clipped(right, cy0, 1U, height - 2U * radius, color)) {
           return false;
       }
   }

   // Calcular los cuatro arcos mediante el algoritmo del punto medio.
   int32_t dx = 0;
   int32_t dy = radius;
   int32_t decision = 1 - dy;

   while (dx <= dy) {
       if (filled) {
           // Rellenar las filas entre los arcos izquierdo y derecho.
           if (!gfx_horizontal_span(cx0 - dx, cx1 + dx, cy0 - dy, color) ||
               !gfx_horizontal_span(cx0 - dy, cx1 + dy, cy0 - dx, color) ||
               !gfx_horizontal_span(cx0 - dx, cx1 + dx, cy1 + dy, color) ||
               !gfx_horizontal_span(cx0 - dy, cx1 + dy, cy1 + dx, color)) {
               return false;
           }
       } else {
           // Dibujar los dos puntos de cada esquina.
           if (!gfx_plot_pixel(cx0 - dx, cy0 - dy, color) ||
               !gfx_plot_pixel(cx0 - dy, cy0 - dx, color) ||
               !gfx_plot_pixel(cx1 + dx, cy0 - dy, color) ||
               !gfx_plot_pixel(cx1 + dy, cy0 - dx, color) ||
               !gfx_plot_pixel(cx0 - dx, cy1 + dy, color) ||
               !gfx_plot_pixel(cx0 - dy, cy1 + dx, color) ||
               !gfx_plot_pixel(cx1 + dx, cy1 + dy, color) ||
               !gfx_plot_pixel(cx1 + dy, cy1 + dx, color)) {
               return false;
           }
       }

       dx++;

       if (decision < 0) {
           decision += 2 * dx + 1;
       } else {
           dy--;
           decision += 2 * (dx - dy) + 1;
       }
   }

   return true;
}

// -----------------------------------------------------------------------------
//  Dibujar contorno redondeado
// -----------------------------------------------------------------------------

bool gfx_draw_round_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t color) {
   return gfx_round_rect(x, y, width, height, radius, color, false);
}

// -----------------------------------------------------------------------------
//  Dibujar rectangulo redondeado relleno
// -----------------------------------------------------------------------------

bool gfx_fill_round_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t color) {
   return gfx_round_rect(x, y, width, height, radius, color, true);
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : gfx.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  API grafica para la pantalla de VoxID.
 *  Las funciones de dibujo son independientes de la interfaz STM32 HAL.
 *
 * =============================================================================
 */

#ifndef GFX_H
#define GFX_H

#include <stdbool.h>
#include <stdint.h>

// -----------------------------------------------------------------------------
//  Operaciones basicas
// -----------------------------------------------------------------------------

bool gfx_fill_screen(uint16_t color);
bool gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
bool gfx_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

// -----------------------------------------------------------------------------
//  Lineas
// -----------------------------------------------------------------------------

bool gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

// -----------------------------------------------------------------------------
//  Rectangulos
// -----------------------------------------------------------------------------

bool gfx_draw_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);

// -----------------------------------------------------------------------------
//  Circulos
// -----------------------------------------------------------------------------

bool gfx_draw_circle(int16_t x, int16_t y, uint16_t radius, uint16_t color);
bool gfx_fill_circle(int16_t x, int16_t y, uint16_t radius, uint16_t color);

#endif /* GFX_H */

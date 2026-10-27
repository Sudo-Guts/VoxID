/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/gfx.h
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
//  Alineacion de texto
// -----------------------------------------------------------------------------

typedef enum {
    GFX_ALIGN_LEFT,
    GFX_ALIGN_CENTER,
    GFX_ALIGN_RIGHT
} gfx_align_t;

// -----------------------------------------------------------------------------
//  Operaciones basicas
// -----------------------------------------------------------------------------

/**
 * @brief Rellena toda la pantalla.
 * @param color Color RGB565.
 * @return true si el dibujo termina correctamente.
 */
bool gfx_fill_screen(uint16_t color);
/**
 * @brief Dibuja un pixel dentro de la pantalla.
 * @param x Columna del pixel.
 * @param y Fila del pixel.
 * @param color Color RGB565.
 * @return true si el pixel es valido y se dibuja.
 */
bool gfx_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
/**
 * @brief Rellena un rectangulo.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param width Anchura en pixeles.
 * @param height Altura en pixeles.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

// -----------------------------------------------------------------------------
//  Lineas
// -----------------------------------------------------------------------------

/**
 * @brief Dibuja una linea entre dos puntos.
 * @param x0 Columna inicial.
 * @param y0 Fila inicial.
 * @param x1 Columna final.
 * @param y1 Fila final.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

// -----------------------------------------------------------------------------
//  Rectangulos
// -----------------------------------------------------------------------------

/**
 * @brief Dibuja el contorno de un rectangulo.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param width Anchura en pixeles.
 * @param height Altura en pixeles.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_draw_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);

// -----------------------------------------------------------------------------
//  Circulos
// -----------------------------------------------------------------------------

/**
 * @brief Dibuja el contorno de un circulo.
 * @param x Columna del centro.
 * @param y Fila del centro.
 * @param radius Radio en pixeles.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_draw_circle(int16_t x, int16_t y, uint16_t radius, uint16_t color);
/**
 * @brief Rellena un circulo.
 * @param x Columna del centro.
 * @param y Fila del centro.
 * @param radius Radio en pixeles.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_fill_circle(int16_t x, int16_t y, uint16_t radius, uint16_t color);

// -----------------------------------------------------------------------------
//  Texto
// -----------------------------------------------------------------------------

/**
 * @brief Dibuja un caracter ASCII con la fuente 5x7.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param character Caracter solicitado.
 * @param color Color RGB565.
 * @return true si el caracter se dibuja.
 */
bool gfx_draw_char(int16_t x, int16_t y, char character, uint16_t color);
/**
 * @brief Dibuja texto ASCII sin borrar el fondo.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param text Cadena terminada en cero.
 * @param color Color RGB565.
 * @return true si el texto se dibuja.
 */
bool gfx_draw_text(int16_t x, int16_t y, const char *text, uint16_t color);

/**
 * @brief Dibuja un caracter con escala entera.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param character Caracter ASCII.
 * @param color Color RGB565.
 * @param scale Factor de escala mayor que cero.
 * @return true si el caracter se dibuja.
 */
bool gfx_draw_char_scaled(int16_t x, int16_t y, char character, uint16_t color, uint8_t scale);
/**
 * @brief Dibuja texto con escala entera.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param text Cadena ASCII terminada en cero.
 * @param color Color RGB565.
 * @param scale Factor de escala mayor que cero.
 * @return true si el texto se dibuja.
 */
bool gfx_draw_text_scaled(int16_t x, int16_t y, const char *text, uint16_t color, uint8_t scale);

// -----------------------------------------------------------------------------
//  Texto con fondo
// -----------------------------------------------------------------------------

/**
 * @brief Dibuja texto y rellena el fondo de cada caracter.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param text Cadena ASCII terminada en cero.
 * @param color Color RGB565 del texto.
 * @param bg Color RGB565 del fondo.
 * @return true si el texto se dibuja.
 */
bool gfx_draw_text_bg(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg);

/**
 * @brief Dibuja texto escalado con fondo.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param text Cadena ASCII terminada en cero.
 * @param color Color RGB565 del texto.
 * @param bg Color RGB565 del fondo.
 * @param scale Factor de escala mayor que cero.
 * @return true si el texto se dibuja.
 */
bool gfx_draw_text_scaled_bg(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale);

/**
 * @brief Dibuja texto con fondo y alineacion horizontal.
 * @param x Punto de referencia horizontal.
 * @param y Fila inicial.
 * @param text Cadena ASCII terminada en cero.
 * @param color Color RGB565 del texto.
 * @param bg Color RGB565 del fondo.
 * @param scale Factor de escala mayor que cero.
 * @param align Alineacion respecto de x.
 * @return true si el texto se dibuja.
 */
bool gfx_draw_text_aligned(int16_t x, int16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale, gfx_align_t align);

// -----------------------------------------------------------------------------
//  Rectangulos redondeados
// -----------------------------------------------------------------------------

/**
 * @brief Dibuja el contorno de un rectangulo redondeado.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param width Anchura en pixeles.
 * @param height Altura en pixeles.
 * @param radius Radio de las esquinas.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_draw_round_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t color);
/**
 * @brief Rellena un rectangulo redondeado.
 * @param x Columna inicial.
 * @param y Fila inicial.
 * @param width Anchura en pixeles.
 * @param height Altura en pixeles.
 * @param radius Radio de las esquinas.
 * @param color Color RGB565.
 * @return true si la operacion termina correctamente.
 */
bool gfx_fill_round_rect(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t radius, uint16_t color);

#endif /* GFX_H */

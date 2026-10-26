/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : st7735.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Controlador TFT ST7735 para la placa Ophyra.
 *  Comunicacion mediante STM32 HAL.
 *
 * =============================================================================
 */

#ifndef ST7735_H
#define ST7735_H

#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"

// -----------------------------------------------------------------------------
//  Dimensiones de pantalla
// -----------------------------------------------------------------------------

#define ST7735_WIDTH          160U
#define ST7735_HEIGHT         128U

// -----------------------------------------------------------------------------
//  Comandos ST7735
// -----------------------------------------------------------------------------

#define ST7735_SWRESET        0x01U
#define ST7735_SLPOUT         0x11U
#define ST7735_NORON          0x13U
#define ST7735_DISPON         0x29U
#define ST7735_CASET          0x2AU
#define ST7735_RASET          0x2BU
#define ST7735_RAMWR          0x2CU
#define ST7735_MADCTL         0x36U
#define ST7735_COLMOD         0x3AU

// -----------------------------------------------------------------------------
//  Configuracion SPI
// -----------------------------------------------------------------------------

#define ST7735_SPI_TIMEOUT    100U
#define ST7735_BLOCK_PIXELS    64U

// -----------------------------------------------------------------------------
//  Configuracion de pantalla
// -----------------------------------------------------------------------------

#define ST7735_X_OFFSET        0U
#define ST7735_Y_OFFSET        0U
#define ST7735_ROTATION      0xA8U

// -----------------------------------------------------------------------------
//  Configuracion GPIO - Ophyra
// -----------------------------------------------------------------------------

// SPI Chip Select
#define ST7735_CS_PORT        GPIOA
#define ST7735_CS_PIN         GPIO_PIN_15

// Data / Command
#define ST7735_DC_PORT        GPIOD
#define ST7735_DC_PIN         GPIO_PIN_6

// Hardware Reset
#define ST7735_RST_PORT       GPIOD
#define ST7735_RST_PIN        GPIO_PIN_7

// Backlight
#define ST7735_BL_PORT        GPIOA
#define ST7735_BL_PIN         GPIO_PIN_7

// -----------------------------------------------------------------------------
//  Inicializacion
// -----------------------------------------------------------------------------

/* Inicializar pantalla utilizando un SPI configurado. */
bool st7735_init(SPI_HandleTypeDef *hspi);

// -----------------------------------------------------------------------------
//  Pantalla
// -----------------------------------------------------------------------------

/* Rellenar pantalla con un color RGB565. */
bool st7735_fill_screen(uint16_t color);

/* Encender o apagar retroiluminacion. */
void st7735_set_backlight(bool enabled);

// -----------------------------------------------------------------------------
//  Primitivas graficas
// -----------------------------------------------------------------------------

bool st7735_draw_pixel(uint16_t x, uint16_t y, uint16_t color);

bool st7735_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);

#endif /* ST7735_H */

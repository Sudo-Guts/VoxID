/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : st7735.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Controlador TFT ST7735 para la placa Ophyra.
 *  Comunicacion mediante STM32 HAL.
 *
 * =============================================================================
 */

#include "st7735.h"

#include <stddef.h>

// -----------------------------------------------------------------------------
//  Variables privadas
// -----------------------------------------------------------------------------

static SPI_HandleTypeDef *tft_spi = NULL;
static bool tft_ready = false;

// -----------------------------------------------------------------------------
//  Comunicacion SPI
// -----------------------------------------------------------------------------

static bool st7735_spi_write(const uint8_t *data, uint16_t size)
{
    if (tft_spi == NULL || (size > 0U && data == NULL)) {
        return false;
    }

    if (size == 0U) {
        return true;
    }

    return HAL_SPI_Transmit(tft_spi, (uint8_t *)data, size, ST7735_SPI_TIMEOUT) == HAL_OK;
}

// -----------------------------------------------------------------------------
//  Envio de comandos
// -----------------------------------------------------------------------------

static bool st7735_write_command(uint8_t command, const uint8_t *data, uint16_t size)
{
	HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_RESET);

    bool success = st7735_spi_write(&command, 1U);

    if (success && size > 0U) {
    	HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
        success = st7735_spi_write(data, size);
    }

    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);

    return success;
}

// -----------------------------------------------------------------------------
//  Retroiluminacion
// -----------------------------------------------------------------------------

void st7735_set_backlight(bool enabled)
{
    HAL_GPIO_WritePin(ST7735_BL_PORT, ST7735_BL_PIN, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// -----------------------------------------------------------------------------
//  Reinicio fisico
// -----------------------------------------------------------------------------

static void st7735_reset(void)
{
    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(10U);

    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_RESET);
    HAL_Delay(20U);

    HAL_GPIO_WritePin(ST7735_RST_PORT, ST7735_RST_PIN, GPIO_PIN_SET);
    HAL_Delay(120U);
}

// -----------------------------------------------------------------------------
//  Inicializacion
// -----------------------------------------------------------------------------

bool st7735_init(SPI_HandleTypeDef *hspi)
{
    if (hspi == NULL) {
        return false;
    }

    tft_spi = hspi;
    tft_ready = false;

    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);
    st7735_set_backlight(false);

    // Reiniciar pantalla.
    st7735_reset();

    // Reinicio por software.
    if (!st7735_write_command(ST7735_SWRESET, NULL, 0U)) {
        return false;
    }
    HAL_Delay(150U);

    // Salir del modo de reposo.
    if (!st7735_write_command(ST7735_SLPOUT, NULL, 0U)) {
        return false;
    }
    HAL_Delay(120U);

    // Configurar formato RGB565.
    const uint8_t color_mode = 0x05U;

    if (!st7735_write_command(ST7735_COLMOD, &color_mode, 1U)) {
        return false;
    }

    // Configurar orientacion.
    const uint8_t rotation = ST7735_ROTATION;

    if (!st7735_write_command(ST7735_MADCTL, &rotation, 1U)) {
        return false;
    }

    // Activar modo normal.
    if (!st7735_write_command(ST7735_NORON, NULL, 0U)) {
    	return false;
    }
    HAL_Delay(10U);

    // Encender pantalla.
    if (!st7735_write_command(ST7735_DISPON, NULL, 0U)) {
        return false;
    }
    HAL_Delay(100U);

    tft_ready = true;

    // Limpiar pantalla antes de encender la iluminacion.
    if (!st7735_fill_screen(0x0000U)) {
        tft_ready = false;
        return false;
    }

    st7735_set_backlight(true);

    return true;
}

// -----------------------------------------------------------------------------
//  Rellenar pantalla
// -----------------------------------------------------------------------------

bool st7735_fill_screen(uint16_t color)
{
    if (!tft_ready) {
        return false;
    }

    const uint16_t x0 = ST7735_X_OFFSET;
    const uint16_t y0 = ST7735_Y_OFFSET;

    const uint16_t x1 = x0 + ST7735_WIDTH - 1U;
    const uint16_t y1 = y0 + ST7735_HEIGHT - 1U;

    // Limites de columnas.
    uint8_t columns[4] = {
        (uint8_t)(x0 >> 8),
        (uint8_t)x0,
        (uint8_t)(x1 >> 8),
        (uint8_t)x1
    };

    // Limites de filas.
    uint8_t rows[4] = {
        (uint8_t)(y0 >> 8),
        (uint8_t)y0,
        (uint8_t)(y1 >> 8),
        (uint8_t)y1
    };

    if (!st7735_write_command(ST7735_CASET, columns, sizeof(columns))) {
        return false;
    }

    if (!st7735_write_command(ST7735_RASET, rows, sizeof(rows))) {
        return false;
    }

    // Preparar un bloque de pixeles RGB565.
    uint8_t buffer[ST7735_BLOCK_PIXELS * 2U];

    for (uint16_t i = 0U; i < ST7735_BLOCK_PIXELS; ++i) {
        buffer[2U * i] = (uint8_t)(color >> 8);
        buffer[2U * i + 1U] = (uint8_t)color;
    }

    // Iniciar una escritura continua en memoria.
    const uint8_t command = ST7735_RAMWR;

    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_RESET);

    if (!st7735_spi_write(&command, 1U)) {
    	HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
        return false;
    }

    HAL_GPIO_WritePin(ST7735_DC_PORT, ST7735_DC_PIN, GPIO_PIN_SET);

    uint32_t remaining = ST7735_WIDTH * ST7735_HEIGHT;

    while (remaining > 0U) {
        uint16_t count = (remaining >= ST7735_BLOCK_PIXELS) ? ST7735_BLOCK_PIXELS : (uint16_t)remaining;

        if (!st7735_spi_write(buffer, count * 2U)) {
        	HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);
            return false;
        }

        remaining -= count;
    }

    HAL_GPIO_WritePin(ST7735_CS_PORT, ST7735_CS_PIN, GPIO_PIN_SET);

    return true;
}

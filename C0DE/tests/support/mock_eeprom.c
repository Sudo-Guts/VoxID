/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/support/mock_eeprom.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  HAL I2C exclusiva de pruebas; verifica direcciones, paginas y tiempos.
 *
 * =============================================================================
 */

#include "mock_eeprom.h"
#include "m24c32.h"
#include "voxid_config.h"

#include <assert.h>
#include <string.h>

uint8_t mock_eeprom_data[4096];
bool mock_eeprom_present;
bool mock_eeprom_protected;
bool mock_eeprom_corrupt_write;
int32_t mock_eeprom_write_budget;
uint32_t mock_eeprom_write_calls;
uint32_t mock_eeprom_read_calls;
uint32_t mock_eeprom_fail_read_call;
uint32_t mock_eeprom_busy_ms;

static uint32_t escritura_iniciada;
static bool escribiendo;

void mock_eeprom_reboot(void) {
    mock_eeprom_present = true;
    mock_eeprom_protected = false;
    mock_eeprom_corrupt_write = false;
    mock_eeprom_write_budget = -1;
    mock_eeprom_write_calls = 0U;
    mock_eeprom_read_calls = 0U;
    mock_eeprom_fail_read_call = 0U;
    mock_eeprom_busy_ms = 3U;
    escribiendo = false;
}

void mock_eeprom_reset(void) {
    (void)memset(mock_eeprom_data, 0xFF, sizeof(mock_eeprom_data));
    mock_eeprom_reboot();
}

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *bus, uint16_t dispositivo, uint32_t intentos, uint32_t timeout) {
    assert(bus == &hi2c1 && dispositivo == (VOXID_EEPROM_ADDRESS_7BIT << 1U));
    assert(intentos == 1U && timeout == VOXID_EEPROM_TIMEOUT_MS);

    if (escribiendo && (uint32_t)(mock_tick - escritura_iniciada) >= mock_eeprom_busy_ms) {
        escribiendo = false;
    }

    return mock_eeprom_present && !escribiendo ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *bus, uint16_t dispositivo, uint16_t direccion, uint16_t ancho, uint8_t *datos, uint16_t longitud, uint32_t timeout) {
    assert(bus == &hi2c1 && dispositivo == (VOXID_EEPROM_ADDRESS_7BIT << 1U));
    assert(ancho == I2C_MEMADD_SIZE_16BIT && timeout == VOXID_EEPROM_TIMEOUT_MS);
    assert(longitud > 0U && longitud <= M24C32_PAGE_SIZE && (uint32_t)direccion + longitud <= sizeof(mock_eeprom_data));
    ++mock_eeprom_read_calls;

    if (!mock_eeprom_present || escribiendo || mock_eeprom_read_calls == mock_eeprom_fail_read_call) {
        return HAL_ERROR;
    }

    (void)memcpy(datos, &mock_eeprom_data[direccion], longitud);
    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *bus, uint16_t dispositivo, uint16_t direccion, uint16_t ancho, uint8_t *datos, uint16_t longitud, uint32_t timeout) {
    assert(bus == &hi2c1 && dispositivo == (VOXID_EEPROM_ADDRESS_7BIT << 1U));
    assert(ancho == I2C_MEMADD_SIZE_16BIT && timeout == VOXID_EEPROM_TIMEOUT_MS);
    assert(longitud > 0U && longitud <= M24C32_PAGE_SIZE && (uint32_t)direccion + longitud <= sizeof(mock_eeprom_data));
    assert(direccion / M24C32_PAGE_SIZE == (direccion + longitud - 1U) / M24C32_PAGE_SIZE);
    ++mock_eeprom_write_calls;

    if (!mock_eeprom_present || mock_eeprom_protected || escribiendo) {
        return HAL_ERROR;
    }

    if (mock_eeprom_write_budget >= 0 && mock_eeprom_write_budget <= longitud) {
        (void)memcpy(&mock_eeprom_data[direccion], datos, (size_t)mock_eeprom_write_budget);
        mock_eeprom_write_budget = 0;
        mock_eeprom_present = false;
        return HAL_ERROR;
    }

    (void)memcpy(&mock_eeprom_data[direccion], datos, longitud);

    if (mock_eeprom_write_budget > 0) {
        mock_eeprom_write_budget -= longitud;
    }

    if (mock_eeprom_corrupt_write) {
        mock_eeprom_data[direccion] ^= 0x80U;
        mock_eeprom_corrupt_write = false;
    }

    escritura_iniciada = mock_tick;
    escribiendo = true;
    return HAL_OK;
}

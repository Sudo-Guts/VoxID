/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/m24c32.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Lectura y escritura M24C32 con direcciones internas de dos bytes,
 *  limites de pagina y sondeo ACK con tiempo de espera finito.
 *
 * =============================================================================
 */

#include "m24c32.h"
#include "voxid_config.h"

#include <stddef.h>
#include <string.h>

_Static_assert(VOXID_EEPROM_ADDRESS_7BIT >= 0x50U && VOXID_EEPROM_ADDRESS_7BIT <= 0x57U, "Direccion M24C32 invalida");

static I2C_HandleTypeDef *bus_memoria;

/**
 * @brief Sondea ACK con un limite temporal que admite el desbordamiento de tick.
 * @return true cuando termina el ciclo interno; false al agotar la espera.
 * @note SysTick debe estar activo; las esperas solo se usan fuera de captura.
 */
static bool m24c32_wait_ready(void) {
    uint32_t inicio = HAL_GetTick();

    do {
        if (HAL_I2C_IsDeviceReady(bus_memoria, VOXID_EEPROM_ADDRESS_7BIT << 1U, 1U, VOXID_EEPROM_TIMEOUT_MS) == HAL_OK) {
            return true;
        }

        HAL_Delay(1U);
    } while ((uint32_t)(HAL_GetTick() - inicio) < VOXID_EEPROM_WRITE_WAIT_MS);

    return false;
}

static bool m24c32_range_valid(uint16_t direccion, const void *datos, uint16_t longitud) {
    return bus_memoria != NULL && datos != NULL && longitud > 0U && direccion < M24C32_CAPACITY && longitud <= M24C32_CAPACITY - direccion;
}

bool m24c32_init(I2C_HandleTypeDef *bus) {
    bus_memoria = bus;
    return bus_memoria != NULL && m24c32_wait_ready();
}

bool m24c32_read(uint16_t direccion, uint8_t *datos, uint16_t longitud) {
    if (!m24c32_range_valid(direccion, datos, longitud)) {
        return false;
    }

    while (longitud > 0U) {
        uint16_t bloque = longitud < M24C32_PAGE_SIZE ? longitud : M24C32_PAGE_SIZE;

        // Lecturas pequenas mantienen acotado el timeout tambien a 100 kHz.
        if (HAL_I2C_Mem_Read(bus_memoria, VOXID_EEPROM_ADDRESS_7BIT << 1U, direccion, I2C_MEMADD_SIZE_16BIT, datos, bloque, VOXID_EEPROM_TIMEOUT_MS) != HAL_OK) {
            return false;
        }

        direccion = (uint16_t)(direccion + bloque);
        datos += bloque;
        longitud = (uint16_t)(longitud - bloque);
    }

    return true;
}

bool m24c32_write(uint16_t direccion, const uint8_t *datos, uint16_t longitud) {
    uint8_t pagina[M24C32_PAGE_SIZE];

    if (!m24c32_range_valid(direccion, datos, longitud)) {
        return false;
    }

    while (longitud > 0U) {
        uint16_t disponibles = (uint16_t)(M24C32_PAGE_SIZE - direccion % M24C32_PAGE_SIZE);
        uint16_t bloque = longitud < disponibles ? longitud : disponibles;
        (void)memcpy(pagina, datos, bloque);

        if (HAL_I2C_Mem_Write(bus_memoria, VOXID_EEPROM_ADDRESS_7BIT << 1U, direccion, I2C_MEMADD_SIZE_16BIT, pagina, bloque, VOXID_EEPROM_TIMEOUT_MS) != HAL_OK || !m24c32_wait_ready()) {
            return false;
        }

        direccion = (uint16_t)(direccion + bloque);
        datos += bloque;
        longitud = (uint16_t)(longitud - bloque);
    }

    return true;
}

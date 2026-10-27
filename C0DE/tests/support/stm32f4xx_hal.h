/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/support/stm32f4xx_hal.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Tipos HAL minimos para probar GFX y EEPROM sin hardware.
 *
 * =============================================================================
 */

#ifndef VOXID_TEST_HAL_H
#define VOXID_TEST_HAL_H

#include "main.h"

typedef struct {
    void *Instance;
} SPI_HandleTypeDef;

#endif /* VOXID_TEST_HAL_H */

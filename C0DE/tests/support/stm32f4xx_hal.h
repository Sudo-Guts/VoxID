/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/support/stm32f4xx_hal.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Tipo SPI minimo para probar GFX sin compilar el controlador de hardware.
 *
 * =============================================================================
 */

#ifndef VOXID_TEST_HAL_H
#define VOXID_TEST_HAL_H

typedef struct {
    void *Instance;
} SPI_HandleTypeDef;

#endif /* VOXID_TEST_HAL_H */

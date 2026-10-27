/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/m24c32.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Acceso acotado a la EEPROM M24C32 mediante I2C bloqueante de HAL.
 *
 * =============================================================================
 */

#ifndef M24C32_H
#define M24C32_H

#include "stm32f4xx_hal.h"

#include <stdbool.h>
#include <stdint.h>

#define M24C32_CAPACITY  4096U
#define M24C32_PAGE_SIZE   32U

/**
 * @brief Vincula el bus y comprueba la respuesta de la EEPROM.
 * @param bus Handle I2C ya inicializado; NULL desvincula el controlador.
 * @return true si el dispositivo responde; false si falta bus o respuesta.
 * @note Usar desde el bucle principal con SysTick activo, nunca desde IRQ.
 */
bool m24c32_init(I2C_HandleTypeDef *bus);

/**
 * @brief Lee un intervalo sin superar los 4096 bytes de memoria.
 * @param direccion Primera direccion de memoria, entre 0 y 4095.
 * @param datos Destino con capacidad para longitud bytes.
 * @param longitud Cantidad no nula de bytes.
 * @return true si toda la lectura termina correctamente.
 * @note Una lectura fallida puede haber modificado parte del destino.
 */
bool m24c32_read(uint16_t direccion, uint8_t *datos, uint16_t longitud);

/**
 * @brief Escribe por paginas y espera el ACK de cada ciclo interno.
 * @param direccion Primera direccion de memoria.
 * @param datos Bytes que se desean guardar.
 * @param longitud Cantidad no nula, contenida en la memoria.
 * @return true si todas las paginas terminan; false ante parametros o bus invalidos.
 * @note No cruza paginas de 32 bytes en una transferencia. Un fallo puede
 *       dejar bytes escritos; el formato transaccional corresponde al llamador.
 */
bool m24c32_write(uint16_t direccion, const uint8_t *datos, uint16_t longitud);

#endif /* M24C32_H */

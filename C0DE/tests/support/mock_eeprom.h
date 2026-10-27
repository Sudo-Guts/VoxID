/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/support/mock_eeprom.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Memoria I2C de prueba con fallos de bus, proteccion y cortes de escritura.
 *
 * =============================================================================
 */

#ifndef MOCK_EEPROM_H
#define MOCK_EEPROM_H

#include "main.h"

extern uint8_t mock_eeprom_data[4096];
extern bool mock_eeprom_present;
extern bool mock_eeprom_protected;
extern bool mock_eeprom_corrupt_write;
extern int32_t mock_eeprom_write_budget;
extern uint32_t mock_eeprom_write_calls;
extern uint32_t mock_eeprom_read_calls;
extern uint32_t mock_eeprom_fail_read_call;
extern uint32_t mock_eeprom_busy_ms;

/** @brief Inicializa EEPROM vacia y reinicia todos los controles de fallo. */
void mock_eeprom_reset(void);

/** @brief Restablece el bus y sus controles sin alterar los bytes persistidos. */
void mock_eeprom_reboot(void);

#endif /* MOCK_EEPROM_H */

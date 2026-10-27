/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_storage.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Persistencia de perfiles con version, CRC y dos copias por locutor.
 *
 * =============================================================================
 */

#ifndef VOXID_STORAGE_H
#define VOXID_STORAGE_H

#include "m24c32.h"
#include "voxid_enroll.h"

typedef enum {
    VOXID_STORAGE_NOT_INITIALIZED = 0,
    VOXID_STORAGE_READY,
    VOXID_STORAGE_EMPTY,
    VOXID_STORAGE_CORRUPT,
    VOXID_STORAGE_IO_ERROR
} VoxIDStorageStatus;

/**
 * @brief Carga los perfiles validos de EEPROM al iniciar el equipo.
 * @param bus Bus I2C1 configurado para la memoria integrada en Ophyra.
 * @return true si todas las lecturas terminaron; false ante error de bus.
 * @note Reinicia el banco RAM. No escribe ni formatea una memoria vacia o
 *       incompatible. Conserva los perfiles que si pudo recuperar.
 */
bool voxid_storage_init(I2C_HandleTypeDef *bus);

/**
 * @brief Guarda y verifica un perfil sin sobrescribir su ultima copia valida.
 * @param locutor Indice del espacio de locutor.
 * @param perfil Perfil completo y valido.
 * @return true si existe una copia verificada del perfil solicitado.
 * @note Invocar con la captura detenida. La RAM la actualiza el registro
 *       solamente tras recibir true. No se usa la pagina de identificacion.
 */
bool voxid_storage_save(uint8_t locutor, const VoxIDSpeakerProfile *perfil);

/**
 * @brief Consulta el resultado del arranque o del ultimo guardado.
 * @return Estado de memoria; CORRUPT indica espacios sin una copia valida.
 */
VoxIDStorageStatus voxid_storage_status(void);

#endif /* VOXID_STORAGE_H */

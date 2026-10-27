/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_enroll.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Registro de tres prototipos MFCC por locutor, exclusivamente en RAM.
 *  Cada prototipo es la media seleccionada de una toma independiente.
 *
 * =============================================================================
 */

#ifndef VOXID_ENROLL_H
#define VOXID_ENROLL_H

#include <stdbool.h>
#include <stdint.h>

#include "voxid_voice.h"

typedef struct {
    float mfcc[VOXID_PROTOTYPES_PER_SPEAKER][VOXID_MFCC_COUNT];
    uint16_t frames_con_voz[VOXID_PROTOTYPES_PER_SPEAKER];
    bool completo;
} VoxIDSpeakerProfile;

typedef enum {
    VOXID_ENROLL_REJECTED = 0,
    VOXID_ENROLL_ACCEPTED,
    VOXID_ENROLL_COMPLETE
} VoxIDEnrollResult;

/**
 * @brief Inicia un registro nuevo sin borrar el perfil anterior del locutor.
 * @param locutor Indice entre cero y VOXID_SPEAKER_COUNT menos uno.
 * @return true si el indice es valido; false sin modificar la sesion actual.
 * @note Una nueva sesion valida descarta las tomas pendientes de la anterior.
 */
bool voxid_enroll_begin(uint8_t locutor);

/**
 * @brief Comprueba continuidad, energia suficiente y media MFCC finita.
 * @param toma Resultado de una captura completa.
 * @return true si puede utilizarse como prototipo.
 * @note El criterio no verifica palabras ni distingue voz de todo tipo de ruido.
 */
bool voxid_enroll_take_valid(const VoxIDVoiceStats *toma);

/**
 * @brief Acepta la media de una toma y completa el perfil al recibir la tercera.
 * @param toma Resultado de una grabacion distinta de las anteriores.
 * @return REJECTED sin avanzar, ACCEPTED o COMPLETE con el perfil actualizado.
 * @note COMPLETE reemplaza el perfil anterior de una sola vez. Los perfiles
 *       son estaticos, se pierden al reiniciar y no se escriben en EEPROM.
 *       Invocar una vez por toma aceptada desde el bucle principal.
 */
VoxIDEnrollResult voxid_enroll_add(const VoxIDVoiceStats *toma);

/**
 * @brief Consulta las tomas aceptadas de la sesion mas reciente.
 * @return Cantidad entre cero y tres; cancelar la reinicia a cero.
 */
uint8_t voxid_enroll_count(void);

/**
 * @brief Consulta un perfil completo sin modificarlo.
 * @param locutor Indice del espacio de locutor.
 * @return Perfil estatico o NULL si el espacio esta vacio o es invalido.
 * @note El contenido cambia al completar otro registro del mismo locutor.
 */
const VoxIDSpeakerProfile *voxid_enroll_profile(uint8_t locutor);

/**
 * @brief Descarta el registro pendiente y conserva los perfiles completos.
 */
void voxid_enroll_cancel(void);

#endif /* VOXID_ENROLL_H */

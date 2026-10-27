/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_identify.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Identificacion por distancia MFCC normalizada y rechazo configurable.
 *
 * =============================================================================
 */

#ifndef VOXID_IDENTIFY_H
#define VOXID_IDENTIFY_H

#include "voxid_voice.h"

typedef enum {
    VOXID_IDENTIFY_INVALID_VOICE = 0,
    VOXID_IDENTIFY_NO_PROFILES,
    VOXID_IDENTIFY_UNKNOWN,
    VOXID_IDENTIFY_AMBIGUOUS,
    VOXID_IDENTIFY_MATCH
} VoxIDIdentifyStatus;

typedef struct {
    VoxIDIdentifyStatus status;
    uint8_t locutor;
    uint8_t perfiles;
    float distancia;
    float margen;
} VoxIDIdentifyResult;

/**
 * @brief Compara una toma valida con los perfiles completos del banco RAM.
 * @param toma Captura nueva, distinta de las usadas para registrar.
 * @param resultado Destino del resultado y de las distancias.
 * @return true si pudo evaluar la solicitud, incluso ante rechazo; false si
 *         alguno de los punteros es NULL.
 * @note Usa C01..C12 unitarios y promedia las tres distancias por locutor.
 *       locutor solo es valido con MATCH; en otro caso vale UINT8_MAX.
 *       distancia y margen se informan al comparar perfiles; con uno solo
 *       margen vale INFINITY. No reconoce palabras ni mide probabilidad.
 */
bool voxid_identify_run(const VoxIDVoiceStats *toma, VoxIDIdentifyResult *resultado);

/**
 * @brief Copia un prototipo normalizado tal como se usa al identificar.
 * @param locutor Indice del locutor registrado.
 * @param prototipo Indice de la toma, entre cero y dos.
 * @param valores Destino con capacidad para VOXID_MFCC_COUNT floats.
 * @return true si existe un perfil valido; false sin modificar el destino.
 * @note C00 queda en cero; C01..C12 tienen norma L2 unitaria. Consulta RAM
 *       y no modifica perfiles, EEPROM ni el estado del extractor MFCC.
 */
bool voxid_identify_read_prototype(uint8_t locutor, uint8_t prototipo, float *valores);

#endif /* VOXID_IDENTIFY_H */

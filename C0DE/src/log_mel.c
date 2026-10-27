/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/log_mel.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion de la transformacion logaritmica
 *  de energias del banco de filtros Mel.
 *
 * =============================================================================
 */

#include "log_mel.h"

#include <math.h>

// =============================================================================
//  Transformacion logaritmica de energias Mel
// =============================================================================

bool log_mel_apply(const float *input, float *output, size_t count, float epsilon) {
    // Validar punteros y dimensiones.
    if (input == NULL || output == NULL || count == 0) {
        return false;
    }

    // Validar el limite inferior del logaritmo.
    if (!isfinite(epsilon) || epsilon <= 0.0f) {
        return false;
    }

    // Validar que las energias sean finitas y no negativas.
    for (size_t i = 0; i < count; i++) {
        if (!isfinite(input[i]) || input[i] < 0.0f) {
            return false;
        }
    }

    // Aplicar el logaritmo natural a cada energia.
    for (size_t i = 0; i < count; i++) {
        float energy = input[i];

        if (energy < epsilon) {
            energy = epsilon;
        }

        output[i] = logf(energy);
    }

    return true;
}

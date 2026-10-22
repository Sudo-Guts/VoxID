/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/dct.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion de la Transformada Discreta del Coseno tipo II.
 *  Utiliza coeficientes precalculados y normalizacion ortonormal.
 *
 * =============================================================================
 */

#include "dct.h"

#include <math.h>
#include <stdint.h>

// =============================================================================
//  Inicializacion de la DCT-II
// =============================================================================

bool dct_init(DCT *dct, float *basis,
              size_t input_size, size_t coefficient_count)
{
    // Validar punteros.
    if (dct == NULL || basis == NULL) return false;

    // Validar dimensiones.
    if (input_size == 0 || coefficient_count == 0) return false;
    if (coefficient_count > input_size) return false;

    // Evitar desbordamiento del numero de coeficientes.
    if (input_size > SIZE_MAX / coefficient_count) return false;

    // Constante matematica PI.
    const float pi = 3.14159265358979323846f;

    // Calcular los factores de normalizacion.
    const float scale_dc = sqrtf(1.0f / (float)input_size);
    const float scale_ac = sqrtf(2.0f / (float)input_size);

    // Precalcular los coeficientes de la DCT-II.
    for (size_t k = 0; k < coefficient_count; k++) {
        float scale = (k == 0) ? scale_dc : scale_ac;

        for (size_t m = 0; m < input_size; m++) {
            float angle = pi * (float)k *
                          ((float)m + 0.5f) / (float)input_size;

            basis[k * input_size + m] = scale * cosf(angle);
        }
    }

    // Guardar la configuracion.
    dct->input_size = input_size;
    dct->coefficient_count = coefficient_count;
    dct->basis = basis;

    return true;
}

// =============================================================================
//  Aplicacion de la DCT-II
// =============================================================================

bool dct_apply(const DCT *dct, const float *input, float *output)
{
    // Validar punteros.
    if (dct == NULL || input == NULL || output == NULL) return false;

    // Validar la configuracion.
    if (dct->basis == NULL || dct->input_size == 0 ||
        dct->coefficient_count == 0 ||
        dct->coefficient_count > dct->input_size) {
        return false;
    }

    // Calcular los coeficientes MFCC.
    for (size_t k = 0; k < dct->coefficient_count; k++) {
        float sum = 0.0f;

        for (size_t m = 0; m < dct->input_size; m++) {
            sum += input[m] * dct->basis[k * dct->input_size + m];
        }

        output[k] = sum;
    }

    return true;
}

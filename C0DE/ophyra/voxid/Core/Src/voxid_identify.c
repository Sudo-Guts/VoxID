/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_identify.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Distancia euclidiana entre vectores MFCC unitarios C01..C12.
 *  Rechaza por distancia maxima y por proximidad entre dos candidatos.
 *
 * =============================================================================
 */

#include "voxid_identify.h"
#include "voxid_enroll.h"

#include <math.h>
#include <stddef.h>

_Static_assert(VOXID_MFCC_COUNT > 1U, "La identificacion necesita C01..Cn");

/**
 * @brief Normaliza sin elevar al cuadrado coeficientes de magnitud arbitraria.
 * @param mfcc Vector original validado por el modulo de registro.
 * @param unitario Destino; la posicion C00 queda excluida de la distancia.
 */
static void identify_normalize(const float *mfcc, float *unitario) {
    float maximo = 0.0f;

    for (uint8_t k = 1U; k < VOXID_MFCC_COUNT; ++k) {
        maximo = fmaxf(maximo, fabsf(mfcc[k]));
    }

    float suma = 0.0f;
    unitario[0] = 0.0f;

    for (uint8_t k = 1U; k < VOXID_MFCC_COUNT; ++k) {
        unitario[k] = mfcc[k] / maximo;
        suma += unitario[k] * unitario[k];
    }

    float norma = sqrtf(suma);

    for (uint8_t k = 1U; k < VOXID_MFCC_COUNT; ++k) {
        unitario[k] /= norma;
    }
}

bool voxid_identify_run(const VoxIDVoiceStats *toma, VoxIDIdentifyResult *resultado) {
    if (toma == NULL || resultado == NULL) {
        return false;
    }

    *resultado = (VoxIDIdentifyResult){.status = VOXID_IDENTIFY_INVALID_VOICE, .locutor = UINT8_MAX, .distancia = INFINITY, .margen = INFINITY};

    if (!voxid_enroll_take_valid(toma)) {
        return true;
    }

    float muestra[VOXID_MFCC_COUNT];
    float referencia[VOXID_MFCC_COUNT];
    float mejor = INFINITY;
    float segunda = INFINITY;
    uint8_t candidato = UINT8_MAX;
    identify_normalize(toma->mean_mfcc, muestra);

    for (uint8_t locutor = 0U; locutor < VOXID_SPEAKER_COUNT; ++locutor) {
        const VoxIDSpeakerProfile *perfil = voxid_enroll_profile(locutor);

        if (!voxid_enroll_profile_valid(perfil)) {
            continue;
        }

        float distancia = 0.0f;

        for (uint8_t p = 0U; p < VOXID_PROTOTYPES_PER_SPEAKER; ++p) {
            identify_normalize(perfil->mfcc[p], referencia);
            float cuadrados = 0.0f;

            for (uint8_t k = 1U; k < VOXID_MFCC_COUNT; ++k) {
                float diferencia = muestra[k] - referencia[k];
                cuadrados += diferencia * diferencia;
            }

            distancia += sqrtf(cuadrados);
        }

        distancia /= (float)VOXID_PROTOTYPES_PER_SPEAKER;
        ++resultado->perfiles;

        if (distancia < mejor) {
            segunda = mejor;
            mejor = distancia;
            candidato = locutor;
        } else if (distancia < segunda) {
            segunda = distancia;
        }
    }

    if (resultado->perfiles == 0U) {
        resultado->status = VOXID_IDENTIFY_NO_PROFILES;
        return true;
    }

    resultado->distancia = mejor;
    resultado->margen = segunda - mejor;

    if (mejor > VOXID_IDENTIFY_MAX_DISTANCE) {
        resultado->status = VOXID_IDENTIFY_UNKNOWN;
    } else if (resultado->margen < VOXID_IDENTIFY_MIN_MARGIN) {
        resultado->status = VOXID_IDENTIFY_AMBIGUOUS;
    } else {
        resultado->status = VOXID_IDENTIFY_MATCH;
        resultado->locutor = candidato;
    }

    return true;
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_selftest.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Interfaz de la prueba funcional sintetica del extractor MFCC.
 *
 * =============================================================================
 */

#ifndef VOXID_SELFTEST_H
#define VOXID_SELFTEST_H

#include <stdint.h>
#include "voxid_dsp.h"

// =============================================================================
//  Configuracion y resultados
// =============================================================================

#define VOXID_SELFTEST_SAMPLES (VOXID_FRAME_SIZE + 2U * VOXID_HOP_SIZE)
#define VOXID_SELFTEST_FRAMES  3U

typedef enum {
    VOXID_SELFTEST_NOT_RUN = 0,
    VOXID_SELFTEST_OK,
    VOXID_SELFTEST_INIT_ERROR,
    VOXID_SELFTEST_PROCESS_ERROR,
    VOXID_SELFTEST_SEQUENCE_ERROR,
    VOXID_SELFTEST_VALUE_ERROR
} VoxIDSelfTestStatus;

typedef struct {
    VoxIDSelfTestStatus status;
    uint32_t samples;
    uint32_t frames;
    float mfcc[VOXID_MFCC_COUNT];
} VoxIDSelfTestResult;

// =============================================================================
//  Prueba funcional MFCC
// =============================================================================

/**
 * @brief Procesa 720 muestras sinteticas de 440 Hz a 16 kHz.
 *
 * @param result Estructura de resultados de la prueba funcional.
 *
 * @note Valida MFCC_READY en las muestras 400, 560 y 720, y
 *       13 coeficientes finitos por frame. La salida corresponde
 *       al ultimo frame. Reinicializa el extractor MFCC compartido.
 *       No realiza adquisicion con ADC ni registra locutores.
 */
void voxid_selftest_run(VoxIDSelfTestResult *result);

#endif /* VOXID_SELFTEST_H */

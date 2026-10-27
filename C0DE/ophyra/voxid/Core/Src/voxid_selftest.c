/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_selftest.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Prueba funcional MFCC con senoide de 440 Hz sin adquisicion ADC.
 *
 * =============================================================================
 */

#include "voxid_selftest.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

// =============================================================================
//  Generacion de senal
// =============================================================================

#define SELFTEST_PI         3.14159265358979323846f
#define SELFTEST_HZ         440.0f
#define SELFTEST_AMPLITUDE  12000.0f

static int16_t test_signal[VOXID_SELFTEST_SAMPLES];

static void selftest_generate_signal(void) {
    for (uint32_t n = 0U; n < VOXID_SELFTEST_SAMPLES; ++n) {
        float angle = 2.0f * SELFTEST_PI * SELFTEST_HZ * (float)n / (float)VOXID_SAMPLE_RATE;
        test_signal[n] = (int16_t)(SELFTEST_AMPLITUDE * sinf(angle));
    }
}

// =============================================================================
//  Validacion del procesamiento
// =============================================================================

void voxid_selftest_run(VoxIDSelfTestResult *result) {
    if (result == NULL) {
        return;
    }

    (void)memset(result, 0, sizeof(*result));
    result->status = VOXID_SELFTEST_NOT_RUN;

    if (!voxid_dsp_init()) {
        result->status = VOXID_SELFTEST_INIT_ERROR;
        return;
    }

    selftest_generate_signal();

    for (uint32_t n = 0U; n < VOXID_SELFTEST_SAMPLES; ++n) {
        MFCCResult state = voxid_dsp_push(test_signal[n]);
        result->samples = n + 1U;

        if (state == MFCC_ERROR) {
            result->status = VOXID_SELFTEST_PROCESS_ERROR;
            return;
        }

        bool expected_ready = n + 1U == VOXID_FRAME_SIZE || n + 1U == VOXID_FRAME_SIZE + VOXID_HOP_SIZE || n + 1U == VOXID_SELFTEST_SAMPLES;

        if ((state == MFCC_READY) != expected_ready) {
            result->status = VOXID_SELFTEST_SEQUENCE_ERROR;
            return;
        }

        if (state != MFCC_READY) {
            continue;
        }

        ++result->frames;

        const float *values = voxid_dsp_coefficients();

        if (values == NULL) {
            result->status = VOXID_SELFTEST_PROCESS_ERROR;
            return;
        }

        for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
            if (!isfinite(values[k])) {
                result->status = VOXID_SELFTEST_VALUE_ERROR;
                return;
            }

            if (n + 1U == VOXID_SELFTEST_SAMPLES) {
                result->mfcc[k] = values[k];
            }
        }
    }

    result->status = result->frames == VOXID_SELFTEST_FRAMES ? VOXID_SELFTEST_OK : VOXID_SELFTEST_SEQUENCE_ERROR;
}

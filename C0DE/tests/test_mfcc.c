
/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_mfcc.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Prueba de integracion de la cadena de extraccion MFCC.
 *  Utiliza una senal senoidal sintetica de 440 Hz.
 *
 * =============================================================================
 */

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "mfcc.h"

// =============================================================================
//  Parametros de procesamiento
// =============================================================================

#define SAMPLE_RATE  16000
#define FRAME_SIZE   400
#define HOP_SIZE     160
#define FFT_SIZE     512
#define MEL_FILTERS  26
#define MFCC_COUNT   13
#define TOTAL_SAMPLES 720

// =============================================================================
//  Almacenamiento estatico
// =============================================================================

static int16_t ring[FRAME_SIZE];
static int16_t frame_samples[FRAME_SIZE];

static float window_coefficients[FRAME_SIZE];
static float fft_input[FFT_SIZE];

static FFTComplex twiddles[FFT_SIZE / 2];
static FFTComplex spectrum[FFT_SIZE];

static float power_spectrum[FFT_SIZE / 2 + 1];
static float mel_points[MEL_FILTERS + 2];
static float mel_energies[MEL_FILTERS];
static float dct_basis[MEL_FILTERS * MFCC_COUNT];

static float mfcc_output[MFCC_COUNT];

// =============================================================================
//  Prueba de integracion MFCC
// =============================================================================

int main(void)
{
    MFCC mfcc = {0};

    const MFCCConfig config = {
        .frame_size = FRAME_SIZE,
        .hop_size = HOP_SIZE,
        .fft_size = FFT_SIZE,
        .mel_filters = MEL_FILTERS,
        .mfcc_count = MFCC_COUNT,
        .sample_rate = SAMPLE_RATE,
        .min_hz = 0.0f,
        .max_hz = 8000.0f,
        .epsilon = 1e-10f
    };

    const MFCCBuffers buffers = {
        .ring = ring,
        .frame_samples = frame_samples,
        .window_coefficients = window_coefficients,
        .fft_input = fft_input,
        .twiddles = twiddles,
        .spectrum = spectrum,
        .power_spectrum = power_spectrum,
        .mel_points = mel_points,
        .mel_energies = mel_energies,
        .dct_basis = dct_basis
    };

    // Inicializar toda la cadena de procesamiento.
    assert(mfcc_init(&mfcc, &config, &buffers));

    const float pi = 3.14159265358979323846f;
    const float frequency = 440.0f;

    size_t frames_ready = 0;

    // Generar y procesar muestras PCM.
    for (size_t n = 0; n < TOTAL_SAMPLES; n++) {
        float angle = 2.0f * pi * frequency *
                      (float)n / (float)SAMPLE_RATE;

        int16_t sample = (int16_t)(12000.0f * sinf(angle));

        MFCCResult result = mfcc_push(&mfcc, sample, mfcc_output);

        // No deben producirse errores.
        assert(result != MFCC_ERROR);

        // Verificar cuando debe generarse un frame.
        bool expected_ready =
            (n + 1 >= FRAME_SIZE) &&
            ((n + 1 - FRAME_SIZE) % HOP_SIZE == 0);

        assert((result == MFCC_READY) == expected_ready);

        if (result == MFCC_READY) {
            frames_ready++;

            printf("\nFrame %zu - Muestra %zu\n",
                   frames_ready, n + 1);

            // Mostrar los coeficientes MFCC.
            for (size_t k = 0; k < MFCC_COUNT; k++) {
                assert(isfinite(mfcc_output[k]));

                printf("MFCC[%2zu] = %+.6f\n",
                       k, mfcc_output[k]);
            }
        }
    }

    // Comprobar el numero total de frames procesados.
    assert(frames_ready == 3);

    printf("\nOK: Integracion MFCC completada.\n");
    printf("Frames procesados: %zu\n", frames_ready);

    return 0;
}

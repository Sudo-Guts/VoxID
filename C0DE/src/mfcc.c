
/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/mfcc.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Integracion de la cadena de extraccion MFCC.
 *
 * =============================================================================
 */

#include "mfcc.h"

#include "power.h"
#include "log_mel.h"

#include <math.h>

// =============================================================================
//  Inicializacion del extractor MFCC
// =============================================================================

bool mfcc_init(MFCC *mfcc, const MFCCConfig *config,
               const MFCCBuffers *buffers)
{
    // Validar argumentos principales.
    if (mfcc == NULL || config == NULL || buffers == NULL) return false;

    mfcc->initialized = false;

    // Validar dimensiones.
    if (config->frame_size < 2 || config->hop_size == 0 ||
        config->hop_size > config->frame_size ||
        config->fft_size < config->frame_size ||
        config->mel_filters == 0 || config->mfcc_count == 0 ||
        config->mfcc_count > config->mel_filters) {
        return false;
    }

    // La FFT Radix-2 necesita una potencia de dos.
    if ((config->fft_size & (config->fft_size - 1)) != 0) {
        return false;
    }

    // Validar epsilon.
    if (!isfinite(config->epsilon) || config->epsilon <= 0.0f) {
        return false;
    }

    // Validar almacenamiento proporcionado.
    if (buffers->ring == NULL ||
        buffers->frame_samples == NULL ||
        buffers->window_coefficients == NULL ||
        buffers->fft_input == NULL ||
        buffers->twiddles == NULL ||
        buffers->spectrum == NULL ||
        buffers->power_spectrum == NULL ||
        buffers->mel_points == NULL ||
        buffers->mel_energies == NULL ||
        buffers->dct_basis == NULL) {
        return false;
    }

    // Inicializar las etapas de procesamiento.
    if (!frame_init(&mfcc->frame, buffers->ring,
                    config->frame_size, config->hop_size)) {
        return false;
    }

    if (!window_init(&mfcc->window, buffers->window_coefficients,
                     config->frame_size)) {
        return false;
    }

    if (!fft_init(&mfcc->fft, buffers->twiddles,
                  config->fft_size)) {
        return false;
    }

    if (!mel_init(&mfcc->mel, buffers->mel_points,
                  config->fft_size, config->mel_filters,
                  config->sample_rate, config->min_hz,
                  config->max_hz)) {
        return false;
    }

    if (!dct_init(&mfcc->dct, buffers->dct_basis,
                  config->mel_filters, config->mfcc_count)) {
        return false;
    }

    // Guardar los arreglos de procesamiento.
    mfcc->frame_samples = buffers->frame_samples;
    mfcc->fft_input = buffers->fft_input;
    mfcc->spectrum = buffers->spectrum;
    mfcc->power_spectrum = buffers->power_spectrum;
    mfcc->mel_energies = buffers->mel_energies;

    mfcc->epsilon = config->epsilon;
    mfcc->initialized = true;

    return true;
}

// =============================================================================
//  Extraccion de coeficientes MFCC
// =============================================================================

MFCCResult mfcc_push(MFCC *mfcc, int16_t sample, float *output)
{
    // Validar el estado y la salida.
    if (mfcc == NULL || output == NULL) return MFCC_ERROR;
    if (!mfcc->initialized) return MFCC_ERROR;

    // 1. Recibir la muestra y completar un frame.
    FrameResult result = frame_push(
        &mfcc->frame, sample, mfcc->frame_samples
    );

    if (result == FRAME_ERROR) return MFCC_ERROR;
    if (result == FRAME_WAITING) return MFCC_WAITING;

    // 2. Normalizar y aplicar Hamming.
    if (!window_apply(&mfcc->window,
                      mfcc->frame_samples,
                      mfcc->fft_input)) {
        return MFCC_ERROR;
    }

    // 3. Completar con ceros hasta fft_size.
    for (size_t i = mfcc->frame.frame_size;
         i < mfcc->fft.fft_size; i++) {
        mfcc->fft_input[i] = 0.0f;
    }

    // 4. Calcular la FFT.
    if (!fft_forward(&mfcc->fft,
                     mfcc->fft_input,
                     mfcc->spectrum)) {
        return MFCC_ERROR;
    }

    // 5. Obtener el espectro de potencia.
    if (!power_compute(mfcc->spectrum,
                       mfcc->power_spectrum,
                       mfcc->fft.fft_size)) {
        return MFCC_ERROR;
    }

    // 6. Aplicar el banco de filtros Mel.
    if (!mel_apply(&mfcc->mel,
                   mfcc->power_spectrum,
                   mfcc->mel_energies)) {
        return MFCC_ERROR;
    }

    // 7. Aplicar el logaritmo en el mismo arreglo.
    if (!log_mel_apply(mfcc->mel_energies,
                       mfcc->mel_energies,
                       mfcc->mel.filter_count,
                       mfcc->epsilon)) {
        return MFCC_ERROR;
    }

    // 8. Calcular los coeficientes MFCC con DCT-II.
    if (!dct_apply(&mfcc->dct,
                   mfcc->mel_energies,
                   output)) {
        return MFCC_ERROR;
    }

    return MFCC_READY;
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Src/voxid_dsp.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  Almacenamiento estatico del extractor MFCC.
 *  El algoritmo se reutiliza desde C0DE/src, sin copiarlo.
 * =============================================================================
 */

#include "voxid_dsp.h"

static MFCC extractor;

static int16_t ring[VOXID_FRAME_SIZE];
static int16_t frame_samples[VOXID_FRAME_SIZE];
static float window_coefficients[VOXID_FRAME_SIZE];
static float fft_input[VOXID_FFT_SIZE];
static FFTComplex twiddles[VOXID_FFT_SIZE / 2U];
static FFTComplex spectrum[VOXID_FFT_SIZE];
static float power_spectrum[VOXID_FFT_SIZE / 2U + 1U];
static float mel_points[VOXID_MEL_FILTERS + 2U];
static float mel_energies[VOXID_MEL_FILTERS];
static float dct_basis[VOXID_MEL_FILTERS * VOXID_MFCC_COUNT];
static float coefficients[VOXID_MFCC_COUNT];

bool voxid_dsp_init(void)
{
    const MFCCConfig config = {
        .frame_size = VOXID_FRAME_SIZE,
        .hop_size = VOXID_HOP_SIZE,
        .fft_size = VOXID_FFT_SIZE,
        .mel_filters = VOXID_MEL_FILTERS,
        .mfcc_count = VOXID_MFCC_COUNT,
        .sample_rate = VOXID_SAMPLE_RATE,
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

    return mfcc_init(&extractor, &config, &buffers);
}

MFCCResult voxid_dsp_push(int16_t sample)
{
    return mfcc_push(&extractor, sample, coefficients);
}

const float *voxid_dsp_coefficients(void)
{
    return coefficients;
}

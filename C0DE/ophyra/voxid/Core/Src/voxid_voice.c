/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_voice.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *  Convierte ADC a PCM con eliminacion de DC y ejecuta MFCC en streaming.
 *  No reserva buffers de tres segundos de audio.
 * =============================================================================
 */
#include "voxid_voice.h"
#include <math.h>
#include <stddef.h>
#include <string.h>

// Pasaaltas DC de primer orden: y[n] = x[n] - x[n-1] + 0.995*y[n-1].
#define VOXID_VOICE_DC_Q15 32604

static int32_t previous_input = 0;
static int32_t previous_output = 0;
static uint32_t voice_samples = 0U;
static uint32_t voice_frames = 0U;
static uint16_t voice_peak = 0U;
static float last_coefficients[VOXID_MFCC_COUNT];
static float sum_coefficients[VOXID_MFCC_COUNT];
static bool voice_active = false;
static bool voice_complete = false;

static int16_t voxid_voice_adc_to_pcm(uint16_t adc) {
    int32_t input = ((int32_t)adc - 2048) * 16;
    int32_t output = input - previous_input + (VOXID_VOICE_DC_Q15 * previous_output) / 32768;
    previous_input = input;

    if (output > 32767) {
        output = 32767;
    } else if (output < -32768) {
        output = -32768;
    }

    previous_output = output;
    return (int16_t)output;
}

bool voxid_voice_begin(void) {
    voice_active = false;
    voice_complete = false;
    previous_input = 0;
    previous_output = 0;
    voice_samples = 0U;
    voice_frames = 0U;
    voice_peak = 0U;
    (void)memset(last_coefficients, 0, sizeof(last_coefficients));
    (void)memset(sum_coefficients, 0, sizeof(sum_coefficients));
    voice_active = voxid_dsp_init();
    return voice_active;
}

bool voxid_voice_process_block(const uint16_t *samples, uint16_t count) {
    if (!voice_active || samples == NULL || count == 0U || voice_samples + count > VOXID_VOICE_EXPECTED_SAMPLES) {
        return false;
    }

    for (uint16_t i = 0U; i < count; ++i) {
        if (samples[i] > 4095U) {
            voice_active = false;
            return false;
        }

        int16_t pcm = voxid_voice_adc_to_pcm(samples[i]);
        uint32_t amplitude = pcm < 0 ? (uint32_t)(-(int32_t)pcm) : (uint32_t)pcm;

        if (amplitude > voice_peak) {
            voice_peak = (uint16_t)amplitude;
        }

        MFCCResult state = voxid_dsp_push(pcm);

        if (state == MFCC_ERROR) {
            voice_active = false;
            return false;
        }

        if (state == MFCC_READY) {
            const float *coeffs = voxid_dsp_coefficients();

            if (coeffs == NULL || voice_frames >= VOXID_VOICE_EXPECTED_FRAMES) {
                voice_active = false;
                return false;
            }

            for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
                if (!isfinite(coeffs[k])) {
                    voice_active = false;
                    return false;
                }

                last_coefficients[k] = coeffs[k];
                sum_coefficients[k] += coeffs[k];
            }

            ++voice_frames;
        }

        ++voice_samples;
    }

    return true;
}

bool voxid_voice_finish(void) {
    voice_complete = voice_active && voice_samples == VOXID_VOICE_EXPECTED_SAMPLES && voice_frames == VOXID_VOICE_EXPECTED_FRAMES;
    voice_active = false;
    return voice_complete;
}

bool voxid_voice_read_stats(VoxIDVoiceStats *stats) {
    if (!voice_complete || stats == NULL || voice_frames == 0U) {
        return false;
    }

    stats->samples = voice_samples;
    stats->frames = voice_frames;
    stats->pcm_peak = voice_peak;

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        stats->last_mfcc[k] = last_coefficients[k];
        stats->mean_mfcc[k] = sum_coefficients[k] / (float)voice_frames;

        if (!isfinite(stats->mean_mfcc[k])) {
            return false;
        }
    }

    return true;
}

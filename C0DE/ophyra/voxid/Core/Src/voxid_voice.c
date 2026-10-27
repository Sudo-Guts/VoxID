/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_voice.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Conversion ADC a PCM, filtro DC y media MFCC de frames seleccionados
 *  por energia. No almacena la grabacion completa ni perfiles de locutor.
 *
 * =============================================================================
 */

#include "voxid_voice.h"

#include <math.h>
#include <stddef.h>
#include <string.h>

// =============================================================================
//  Estado de una toma
// =============================================================================

#define VOXID_VOICE_DC_Q15 32604

static int32_t entrada_anterior;
static int32_t salida_anterior;
static bool filtro_iniciado;
static uint32_t muestras_recibidas;
static uint32_t frames_totales;
static uint32_t frames_con_voz;
static uint16_t pico_pcm;
static float energia_maxima;
static float ultimo_mfcc[VOXID_MFCC_COUNT];
static float suma_mfcc[VOXID_MFCC_COUNT];
static bool toma_activa;
static bool toma_completa;

/**
 * @brief Centra el ADC y elimina DC sin introducir un escalon al iniciar.
 * @param adc Cuenta ADC previamente validada, entre cero y 4095.
 * @return Muestra PCM saturada al intervalo de int16_t.
 */
static int16_t voxid_voice_adc_to_pcm(uint16_t adc) {
    int32_t entrada = ((int32_t)adc - 2048) * 16;

    if (!filtro_iniciado) {
        entrada_anterior = entrada;
        filtro_iniciado = true;
    }

    int32_t salida = entrada - entrada_anterior + (VOXID_VOICE_DC_Q15 * salida_anterior) / 32768;
    entrada_anterior = entrada;

    if (salida > 32767) {
        salida = 32767;
    } else if (salida < -32768) {
        salida = -32768;
    }

    salida_anterior = salida;
    return (int16_t)salida;
}

/**
 * @brief Valida un vector y lo acumula solo si supera el umbral de energia.
 * @return true si los coeficientes y la energia son finitos.
 */
static bool voxid_voice_accept_frame(void) {
    const float *coeficientes = voxid_dsp_coefficients();
    float energia = voxid_dsp_frame_energy();

    if (coeficientes == NULL || frames_totales >= VOXID_VOICE_EXPECTED_FRAMES || !isfinite(energia)) {
        return false;
    }

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        if (!isfinite(coeficientes[k])) {
            return false;
        }
    }

    bool tiene_voz = energia >= VOXID_VOICE_MIN_RMS * VOXID_VOICE_MIN_RMS;

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        ultimo_mfcc[k] = coeficientes[k];

        if (tiene_voz) {
            suma_mfcc[k] += coeficientes[k];

            if (!isfinite(suma_mfcc[k])) {
                return false;
            }
        }
    }

    if (energia > energia_maxima) {
        energia_maxima = energia;
    }

    ++frames_totales;

    if (tiene_voz) {
        ++frames_con_voz;
    }

    return true;
}

// =============================================================================
//  Control y procesamiento de la toma
// =============================================================================

bool voxid_voice_begin(void) {
    toma_activa = false;
    toma_completa = false;
    entrada_anterior = 0;
    salida_anterior = 0;
    filtro_iniciado = false;
    muestras_recibidas = 0U;
    frames_totales = 0U;
    frames_con_voz = 0U;
    pico_pcm = 0U;
    energia_maxima = 0.0f;
    (void)memset(ultimo_mfcc, 0, sizeof(ultimo_mfcc));
    (void)memset(suma_mfcc, 0, sizeof(suma_mfcc));
    toma_activa = voxid_dsp_init();
    return toma_activa;
}

bool voxid_voice_process_block(const uint16_t *samples, uint16_t count) {
    if (!toma_activa) {
        return false;
    }

    if (samples == NULL || count == 0U || count > VOXID_VOICE_EXPECTED_SAMPLES - muestras_recibidas) {
        toma_activa = false;
        return false;
    }

    for (uint16_t i = 0U; i < count; ++i) {
        if (samples[i] > 4095U) {
            toma_activa = false;
            return false;
        }

        int16_t pcm = voxid_voice_adc_to_pcm(samples[i]);
        uint32_t amplitud = pcm < 0 ? (uint32_t)(-(int32_t)pcm) : (uint32_t)pcm;

        if (amplitud > pico_pcm) {
            pico_pcm = (uint16_t)amplitud;
        }

        MFCCResult estado = voxid_dsp_push(pcm);

        if (estado == MFCC_ERROR || (estado == MFCC_READY && !voxid_voice_accept_frame())) {
            toma_activa = false;
            return false;
        }

        ++muestras_recibidas;
    }

    return true;
}

bool voxid_voice_finish(void) {
    toma_completa = toma_activa && muestras_recibidas == VOXID_VOICE_EXPECTED_SAMPLES && frames_totales == VOXID_VOICE_EXPECTED_FRAMES;
    toma_activa = false;
    return toma_completa;
}

bool voxid_voice_read_stats(VoxIDVoiceStats *stats) {
    if (!toma_completa || stats == NULL) {
        return false;
    }

    stats->samples = muestras_recibidas;
    stats->frames = frames_totales;
    stats->voiced_frames = frames_con_voz;
    stats->pcm_peak = pico_pcm;
    stats->rms_max = sqrtf(energia_maxima);

    for (uint32_t k = 0U; k < VOXID_MFCC_COUNT; ++k) {
        stats->last_mfcc[k] = ultimo_mfcc[k];
        stats->mean_mfcc[k] = frames_con_voz > 0U ? suma_mfcc[k] / (float)frames_con_voz : 0.0f;
    }

    return true;
}

void voxid_voice_cancel(void) {
    toma_activa = false;
    toma_completa = false;
}

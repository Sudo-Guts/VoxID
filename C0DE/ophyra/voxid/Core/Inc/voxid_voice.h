/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_voice.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *  ADC 12 bits -> PCM con signo -> MFCC en streaming.
 * =============================================================================
 */
#ifndef VOXID_VOICE_H
#define VOXID_VOICE_H

#include <stdbool.h>
#include <stdint.h>
#include "voxid_dsp.h"

#define VOXID_VOICE_EXPECTED_SAMPLES (VOXID_SAMPLE_RATE * 3U)
#define VOXID_VOICE_EXPECTED_FRAMES (1U + (VOXID_VOICE_EXPECTED_SAMPLES - VOXID_FRAME_SIZE) / VOXID_HOP_SIZE)

typedef struct {
    uint32_t samples;
    uint32_t frames;
    uint16_t pcm_peak;
    float last_mfcc[VOXID_MFCC_COUNT];
    float mean_mfcc[VOXID_MFCC_COUNT];
} VoxIDVoiceStats;

/**
 * @brief Reinicia el filtro DC y el extractor MFCC.
 * @return true si el extractor se inicializa correctamente.
 */
bool voxid_voice_begin(void);

/**
 * @brief Convierte un bloque ADC a PCM y lo procesa por MFCC.
 * @param samples Valores ADC sin signo entre 0 y 4095.
 * @param count Numero de muestras del bloque.
 * @return true si el procesamiento concluye sin errores.
 * @note Llamar desde el bucle principal, nunca desde ISR.
 */
bool voxid_voice_process_block(const uint16_t *samples, uint16_t count);

/**
 * @brief Verifica las 48000 muestras y 298 frames esperados.
 * @return true si se completo la toma.
 */
bool voxid_voice_finish(void);

/**
 * @brief Recupera el ultimo vector y la media MFCC de una toma valida.
 * @param stats Destino del resumen.
 * @return true si los resultados estan disponibles.
 */
bool voxid_voice_read_stats(VoxIDVoiceStats *stats);

#endif /* VOXID_VOICE_H */

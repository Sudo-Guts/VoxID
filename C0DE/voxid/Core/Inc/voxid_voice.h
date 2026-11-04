/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/voxid/Core/Inc/voxid_voice.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *  ADC 12 bits -> PCM con signo -> MFCC en streaming con buffers estaticos.
 * =============================================================================
 */
#ifndef VOXID_VOICE_H
#define VOXID_VOICE_H

#include <stdbool.h>
#include <stdint.h>
#include "mfcc.h"
#include "voxid_config.h"

#define VOXID_VOICE_EXPECTED_SAMPLES VOXID_CAPTURE_TOTAL_SAMPLES
#define VOXID_VOICE_EXPECTED_FRAMES (1U + (VOXID_VOICE_EXPECTED_SAMPLES - VOXID_FRAME_SIZE) / VOXID_HOP_SIZE)

typedef struct {
    uint32_t samples;
    uint32_t frames;
    uint32_t voiced_frames;
    uint16_t pcm_peak;
    float rms_max;
    float last_mfcc[VOXID_MFCC_COUNT];
    float mean_mfcc[VOXID_MFCC_COUNT];
} VoxIDVoiceStats;

/**
 * @brief Inicializa el extractor y sus buffers estaticos.
 * @return true si esta listo.
 */
bool voxid_dsp_init(void);

/**
 * @brief Alimenta el extractor con una muestra PCM de 16 bits.
 * @param sample Muestra con signo.
 * @return MFCC_READY, MFCC_WAITING o MFCC_ERROR.
 */
MFCCResult voxid_dsp_push(int16_t sample);

/**
 * @brief Consulta la inicializacion.
 * @return true si el extractor esta listo.
 */
bool voxid_dsp_is_ready(void);

/**
 * @brief Consulta el vector mas reciente.
 * @return Arreglo de 13 coeficientes; leerlo solamente tras MFCC_READY.
 */
const float *voxid_dsp_coefficients(void);

/**
 * @brief Calcula la energia media del frame PCM que produjo los ultimos MFCC.
 * @return Energia normalizada a escala completa, entre cero y uno.
 * @note Consultar inmediatamente despues de MFCC_READY, antes de otra muestra.
 */
float voxid_dsp_frame_energy(void);

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
 * @brief Verifica las 40000 muestras y 248 frames esperados.
 * @return true si se completo la toma.
 */
bool voxid_voice_finish(void);

/**
 * @brief Recupera el ultimo vector y la media de frames con energia suficiente.
 * @param stats Destino del resumen.
 * @return true si los resultados estan disponibles.
 * @note voiced_frames cuenta los frames sobre VOXID_VOICE_MIN_RMS.
 *       Si vale cero, mean_mfcc no representa voz y no debe utilizarse.
 *       El criterio energetico no verifica palabras ni identifica locutores.
 */
bool voxid_voice_read_stats(VoxIDVoiceStats *stats);

/**
 * @brief Cancela la toma y descarta la disponibilidad de sus resultados.
 */
void voxid_voice_cancel(void);

#endif /* VOXID_VOICE_H */

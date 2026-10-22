
/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/mfcc.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para la extraccion de coeficientes MFCC.
 *  Integra los modulos de procesamiento digital de audio.
 *
 * =============================================================================
 */

#ifndef MFCC_H
#define MFCC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "frame.h"
#include "window.h"
#include "fft.h"
#include "mel.h"
#include "dct.h"

/**
 * @brief Estados del extractor MFCC.
 */
typedef enum {
    MFCC_ERROR = -1,
    MFCC_WAITING = 0,
    MFCC_READY = 1
} MFCCResult;

/**
 * @brief Parametros de procesamiento.
 */
typedef struct {
    size_t frame_size;
    size_t hop_size;
    size_t fft_size;
    size_t mel_filters;
    size_t mfcc_count;

    uint32_t sample_rate;

    float min_hz;
    float max_hz;
    float epsilon;
} MFCCConfig;

/**
 * @brief Arreglos proporcionados por el usuario.
 *
 * @note Todos deben permanecer validos durante el procesamiento.
 *       Deben tener capacidad suficiente y no superponerse.
 */
typedef struct {
    int16_t *ring;
    int16_t *frame_samples;

    float *window_coefficients;
    float *fft_input;

    FFTComplex *twiddles;
    FFTComplex *spectrum;

    float *power_spectrum;
    float *mel_points;
    float *mel_energies;
    float *dct_basis;
} MFCCBuffers;

/**
 * @brief Estado del extractor MFCC.
 */
typedef struct {
    Frame frame;
    Window window;
    FFT fft;
    Mel mel;
    DCT dct;

    int16_t *frame_samples;
    float *fft_input;
    FFTComplex *spectrum;
    float *power_spectrum;
    float *mel_energies;

    float epsilon;
    bool initialized;
} MFCC;

/**
 * @brief Inicializa la cadena de extraccion MFCC.
 *
 * @param mfcc Estado del extractor.
 * @param config Parametros de procesamiento.
 * @param buffers Arreglos de almacenamiento.
 *
 * @return true si la inicializacion fue correcta.
 *
 * @note fft_size debe ser potencia de dos y >= frame_size.
 * @note La funcion no asigna memoria dinamica.
 */
bool mfcc_init(MFCC *mfcc, const MFCCConfig *config,
               const MFCCBuffers *buffers);

/**
 * @brief Agrega una muestra PCM y procesa un frame cuando esta listo.
 *
 * @param mfcc Extractor previamente inicializado.
 * @param sample Muestra PCM con signo de 16 bits.
 * @param output Arreglo con capacidad para mfcc_count coeficientes.
 *
 * @return MFCC_READY cuando output contiene un nuevo vector MFCC.
 * @return MFCC_WAITING cuando aun no hay un frame completo.
 * @return MFCC_ERROR si falla alguna etapa.
 *
 * @note El vector de salida se actualiza solo al procesar
 *       un frame completo y sin errores.
 */
MFCCResult mfcc_push(MFCC *mfcc, int16_t sample, float *output);

#endif

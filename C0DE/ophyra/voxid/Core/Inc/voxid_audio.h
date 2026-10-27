/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_audio.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Adquisicion acotada de muestras ADC1 disparadas por TIM2.
 *  Prueba de hardware previa al uso de DMA y al reconocimiento de voz.
 *
 * =============================================================================
 */

#ifndef VOXID_AUDIO_H
#define VOXID_AUDIO_H

#include <stdbool.h>
#include <stdint.h>

// =============================================================================
//  Configuracion de adquisicion
// =============================================================================

#define VOXID_AUDIO_SAMPLE_RATE 16000U
#define VOXID_AUDIO_SAMPLES     720U

typedef enum {
    VOXID_AUDIO_IDLE = 0,
    VOXID_AUDIO_CAPTURING,
    VOXID_AUDIO_READY,
    VOXID_AUDIO_ERROR
} VoxIDAudioState;

typedef struct {
    uint16_t minimum;
    uint16_t maximum;
    uint16_t average;
    uint16_t count;
} VoxIDAudioStats;

// =============================================================================
//  Interfaz publica
// =============================================================================

/**
 * @brief Inicia una captura acotada de 720 conversiones ADC1 a 16 kHz.
 *
 * @return true si el ADC quedo armado y el temporizador se inicio.
 * @return false si los perifericos o su configuracion son incorrectos.
 *
 * @note El ADC1 lee PA2 (IN2). TIM2 genera TRGO al actualizarse.
 *       Esta prueba usa una interrupcion por muestra; DMA queda pendiente.
 */
bool voxid_audio_start(void);

/**
 * @brief Detiene la conversion y descarta cualquier captura parcial.
 *
 * @note Utilizar desde el bucle principal, no desde interrupciones.
 */
void voxid_audio_stop(void);

/**
 * @brief Devuelve el estado actual de la captura.
 *
 * @return Estado de adquisicion actual.
 */
VoxIDAudioState voxid_audio_status(void);

/**
 * @brief Indica cuantas muestras se recibieron por el ADC.
 *
 * @return Numero de conversiones recibidas hasta VOXID_AUDIO_SAMPLES.
 */
uint16_t voxid_audio_sample_count(void);

/**
 * @brief Resume un bloque completado en unidades crudas del ADC.
 *
 * @param stats Estructura de salida para el minimo, maximo, promedio y total.
 *
 * @return true si se completo el bloque y el resumen es valido.
 * @return false cuando la adquisicion no ha terminado o fallo.
 *
 * @note Los datos van de 0 a 4095; aun no estan convertidos a PCM
 *       ni se han enviado al extractor MFCC.
 */
bool voxid_audio_read_stats(VoxIDAudioStats *stats);

#endif /* VOXID_AUDIO_H */

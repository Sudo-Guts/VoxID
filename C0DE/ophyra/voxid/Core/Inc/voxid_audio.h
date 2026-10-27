/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_audio.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Adquisicion ADC1 + TIM2 TRGO + DMA circular a 16 kHz.
 *
 * =============================================================================
 */

#ifndef VOXID_AUDIO_H
#define VOXID_AUDIO_H

#include <stdbool.h>
#include <stdint.h>

// =============================================================================
//  Parametros de adquisicion
// =============================================================================

#define VOXID_AUDIO_SAMPLE_RATE   16000U
#define VOXID_AUDIO_DURATION_S    3U
#define VOXID_AUDIO_TOTAL_SAMPLES (VOXID_AUDIO_SAMPLE_RATE * VOXID_AUDIO_DURATION_S)
#define VOXID_AUDIO_BLOCK_SAMPLES 640U
#define VOXID_AUDIO_DMA_SAMPLES   (2U * VOXID_AUDIO_BLOCK_SAMPLES)
#define VOXID_AUDIO_TOTAL_BLOCKS  (VOXID_AUDIO_TOTAL_SAMPLES / VOXID_AUDIO_BLOCK_SAMPLES)

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
    uint32_t count;
    uint32_t dropped_blocks;
} VoxIDAudioStats;

// =============================================================================
//  API publica
// =============================================================================

/**
 * @brief Inicia tres segundos de captura ADC1 sincronizada por TIM2.
 *
 * @return true si DMA circular y TIM2 iniciaron correctamente.
 * @return false si la configuracion es invalida o existe un fallo de HAL.
 *
 * @note Entrada PA2 (ADC1_IN2), rango de 0 a VDDA; 48000 muestras.
 *       No ejecuta MFCC ni guarda perfiles de locutor.
 */
bool voxid_audio_start(void);

/**
 * @brief Procesa fuera de ISR las mitades completas del buffer DMA.
 *
 * @note Ejecutar con frecuencia desde el bucle principal; nunca dentro de IRQ.
 *       Marca ERROR si una mitad corre riesgo de ser sobrescrita.
 */
void voxid_audio_task(void);

/**
 * @brief Detiene ADC y TIM2 y descarta la sesion actual.
 *
 * @note Solo debe llamarse desde el bucle principal.
 */
void voxid_audio_stop(void);

/**
 * @brief Consulta el estado de captura.
 *
 * @return VOXID_AUDIO_IDLE, CAPTURING, READY o ERROR.
 */
VoxIDAudioState voxid_audio_status(void);

/**
 * @brief Consulta el avance de captura validado por el bucle principal.
 *
 * @return Numero de muestras procesadas, entre 0 y 48000.
 */
uint32_t voxid_audio_sample_count(void);

/**
 * @brief Devuelve estadisticas del ultimo bloque completo de tres segundos.
 *
 * @param stats Estructura de salida, en cuentas crudas del ADC de 12 bits.
 *
 * @return true cuando la captura termino sin desbordamientos.
 */
bool voxid_audio_read_stats(VoxIDAudioStats *stats);

#endif /* VOXID_AUDIO_H */

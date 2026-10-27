/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_dsp.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  Adaptador de MFCC portable para el STM32F407. Sin memoria dinamica.
 * =============================================================================
 */
#ifndef VOXID_DSP_H
#define VOXID_DSP_H

#include <stdbool.h>
#include <stdint.h>
#include "mfcc.h"
#include "voxid_config.h"

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

#endif /* VOXID_DSP_H */

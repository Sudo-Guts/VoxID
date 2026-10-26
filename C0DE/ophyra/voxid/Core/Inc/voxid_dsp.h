/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Inc/voxid_dsp.h
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

#define VOXID_SAMPLE_RATE    16000U
#define VOXID_FRAME_SIZE       400U
#define VOXID_HOP_SIZE         160U
#define VOXID_FFT_SIZE         512U
#define VOXID_MEL_FILTERS       26U
#define VOXID_MFCC_COUNT        13U

bool voxid_dsp_init(void);
MFCCResult voxid_dsp_push(int16_t sample);

/* Reporta si mfcc_init() termino correctamente. */
bool voxid_dsp_is_ready(void);
const float *voxid_dsp_coefficients(void);

#endif /* VOXID_DSP_H */

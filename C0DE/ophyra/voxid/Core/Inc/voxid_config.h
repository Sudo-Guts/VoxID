/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_config.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Parametros compartidos de adquisicion y extraccion MFCC.
 *
 * =============================================================================
 */

#ifndef VOXID_CONFIG_H
#define VOXID_CONFIG_H

#define VOXID_SAMPLE_RATE          16000U
#define VOXID_CAPTURE_DURATION_S       3U
#define VOXID_FRAME_SIZE             400U
#define VOXID_HOP_SIZE               160U
#define VOXID_FFT_SIZE               512U
#define VOXID_MEL_FILTERS             26U
#define VOXID_MFCC_COUNT              13U

/* RMS normalizado a escala completa; ajustar con voz y ruido medidos en placa. */
#define VOXID_VOICE_MIN_RMS 0.01f

#endif /* VOXID_CONFIG_H */

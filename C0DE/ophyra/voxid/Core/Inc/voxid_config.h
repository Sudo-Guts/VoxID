/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/voxid_config.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Parametros compartidos de audio, registro, EEPROM e identificacion.
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

// Ajustar el minimo de frames con las tomas de la placa.
#define VOXID_SPEAKER_COUNT            4U
#define VOXID_PROTOTYPES_PER_SPEAKER    3U
#define VOXID_ENROLL_MIN_VOICED_FRAMES 20U

// Ophyra: E2/E1/E0 a cero. HAL recibe la direccion de 7 bits desplazada.
#define VOXID_EEPROM_ADDRESS_7BIT   0x50U
#define VOXID_EEPROM_TIMEOUT_MS       20U
#define VOXID_EEPROM_WRITE_WAIT_MS    10U

// Cambiar la revision si cambia la definicion del vector MFCC persistido.
#define VOXID_PROFILE_DSP_REVISION     1U

// Distancia euclidiana entre vectores unitarios C01..C12: rango 0..2.
// Son valores iniciales, pendientes de calibracion con tomas nuevas.
#define VOXID_IDENTIFY_MAX_DISTANCE 0.35f
#define VOXID_IDENTIFY_MIN_MARGIN   0.05f

#endif /* VOXID_CONFIG_H */

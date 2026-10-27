/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/fft.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para el calculo de la Transformada Rapida de Fourier.
 *  Implementa una FFT compleja Radix-2 de longitud configurable.
 *
 * =============================================================================
 */
#ifndef FFT_H
#define FFT_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Representacion de un numero complejo.
 */
typedef struct {
    float real;
    float imag;
} FFTComplex;

/**
 * @brief Estructura de configuracion de la FFT.
 */
typedef struct {
    size_t fft_size;      // Numero de puntos de la FFT.
    FFTComplex *twiddles; // Factores de rotacion precalculados.
} FFT;

/**
 * @brief Inicializa la configuracion de la FFT.
 *
 * @param fft Puntero a la estructura de configuracion.
 * @param twiddles Arreglo para almacenar los factores de rotacion.
 * @param fft_size Numero de puntos de la transformada.
 *
 * @return true si la inicializacion fue correcta.
 * @return false si los parametros son invalidos.
 *
 * @note fft_size debe ser una potencia de dos y mayor o igual que 2.
 * @note twiddles debe tener capacidad para fft_size / 2
 *       elementos y permanecer valido durante el uso del modulo.
 *       No se realiza asignacion dinamica de memoria.
 */
bool fft_init(FFT *fft, FFTComplex *twiddles, size_t fft_size);

/**
 * @brief Calcula la Transformada Rapida de Fourier.
 *
 * @param fft Puntero a la configuracion previamente inicializada.
 * @param input Arreglo de muestras reales en punto flotante.
 * @param output Arreglo donde se almacenara el espectro complejo.
 *
 * @return true si la transformada fue calculada correctamente.
 * @return false si se detecta un argumento o estado invalido.
 *
 * @note input debe tener capacidad para fft_size muestras.
 * @note output debe tener capacidad para fft_size elementos
 *       FFTComplex y no debe superponerse con input.
 * @note La transformada directa no aplica normalizacion 1 / N.
 */
bool fft_forward(const FFT *fft, const float *input, FFTComplex *output);

#endif

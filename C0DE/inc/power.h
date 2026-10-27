/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/power.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para el calculo del espectro de potencia.
 *  Procesa los coeficientes complejos generados por la FFT.
 *
 * =============================================================================
 */

#ifndef POWER_H
#define POWER_H

#include <stdbool.h>
#include <stddef.h>

#include "fft.h"

/**
 * @brief Calcula el espectro de potencia de una senal real.
 *
 * @param spectrum Arreglo de coeficientes complejos de la FFT.
 * @param output Arreglo donde se almacenara el espectro de potencia.
 * @param fft_size Numero de puntos de la FFT.
 *
 * @return true si el procesamiento fue correcto.
 * @return false si los argumentos son invalidos.
 *
 * @note spectrum debe contener fft_size coeficientes.
 * @note output debe tener capacidad para fft_size / 2 + 1 elementos.
 * @note La potencia se calcula como real^2 + imag^2,
 *       sin aplicar normalizacion.
 * @note No se realiza asignacion dinamica de memoria.
 */
bool power_compute(const FFTComplex *spectrum, float *output, size_t fft_size);

#endif

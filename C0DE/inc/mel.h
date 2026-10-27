/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/mel.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para el banco de filtros Mel.
 *  Procesa el espectro de potencia mediante filtros triangulares.
 *
 * =============================================================================
 */

#ifndef MEL_H
#define MEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Configuracion del banco de filtros Mel.
 */
typedef struct {
    size_t fft_size;
    size_t filter_count;
    uint32_t sample_rate;
    const float *points_hz;
} Mel;

/**
 * @brief Inicializa el banco de filtros Mel.
 *
 * @param mel Puntero a la configuracion.
 * @param points_hz Arreglo para almacenar las frecuencias de referencia.
 * @param fft_size Numero de puntos de la FFT.
 * @param filter_count Numero de filtros Mel.
 * @param sample_rate Frecuencia de muestreo en Hz.
 * @param min_hz Frecuencia minima del banco.
 * @param max_hz Frecuencia maxima del banco.
 *
 * @return true si la inicializacion fue correcta.
 * @return false si los parametros son invalidos.
 *
 * @note fft_size debe ser potencia de dos y mayor o igual que 2.
 * @note points_hz debe tener capacidad para filter_count + 2
 *       elementos y permanecer valido durante el uso del modulo.
 * @note max_hz no debe superar la frecuencia de Nyquist.
 * @note No se realiza asignacion dinamica de memoria.
 */
bool mel_init(Mel *mel, float *points_hz, size_t fft_size, size_t filter_count, uint32_t sample_rate, float min_hz, float max_hz);

/**
 * @brief Aplica el banco de filtros al espectro de potencia.
 *
 * @param mel Puntero a la configuracion inicializada.
 * @param power Espectro de potencia con fft_size / 2 + 1 elementos.
 * @param output Arreglo de energias con filter_count elementos.
 *
 * @return true si el procesamiento fue correcto.
 * @return false si los argumentos son invalidos.
 *
 * @note Los arreglos power y output no deben superponerse.
 * @note Los filtros tienen altura unitaria, sin normalizacion de area.
 */
bool mel_apply(const Mel *mel, const float *power, float *output);

#endif

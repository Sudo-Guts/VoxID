/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/window.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para el enventanado de muestras de audio.
 *  Implementa la ventana de Hamming de longitud configurable.
 *
 * =============================================================================
 */
#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Estructura de configuracion del enventanador.
 */
typedef struct {
    size_t window_size;   // Numero de muestras por ventana.
    float *coefficients;  // Coeficientes de la ventana de Hamming.
} Window;

/**
 * @brief Inicializa una ventana de Hamming.
 *
 * @param window Puntero a la estructura del enventanador.
 * @param coefficients Arreglo para almacenar los coeficientes.
 * @param window_size Numero de muestras de la ventana.
 *
 * @return true si la inicializacion fue correcta.
 * @return false si los parametros son invalidos.
 *
 * @note window_size debe ser mayor o igual que 2.
 * @note coefficients debe tener capacidad para window_size
 *       elementos y permanecer valido durante el uso del modulo.
 *       No se realiza asignacion dinamica de memoria.
 */
bool window_init(Window *window, float *coefficients, size_t window_size);

/**
 * @brief Aplica la ventana de Hamming a muestras de audio.
 *
 * @param window Puntero al enventanador inicializado.
 * @param input Arreglo de muestras PCM de 16 bits con signo.
 * @param output Arreglo donde se almacenaran las muestras procesadas.
 *
 * @return true si el procesamiento fue correcto.
 * @return false si se detecta un argumento o estado invalido.
 *
 * @note input y output deben tener capacidad para window_size
 *       elementos y no deben superponerse en memoria.
 * @note Las muestras se normalizan dividiendo entre 32768.0f
 *       antes de aplicar los coeficientes de Hamming.
 */
bool window_apply(const Window *window, const int16_t *input, float *output);

#endif

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/window.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion del modulo de enventanado de audio.
 *  Calcula y aplica coeficientes de la ventana de Hamming.
 *
 * =============================================================================
 */
#include "window.h"

#include <math.h>

// =============================================================================
//  Inicializacion del enventanador
// =============================================================================
bool window_init(Window *window, float *coefficients, size_t window_size) {
    // Validar punteros.
    if (window == NULL || coefficients == NULL) {
        return false;
    }

    // Validar la longitud de la ventana.
    if (window_size < 2) {
        return false;
    }

    // Configurar almacenamiento y parametros.
    window->coefficients = coefficients;
    window->window_size = window_size;

    // Constante matematica PI.
    const float pi = 3.14159265358979323846f;

    // Calcular los coeficientes de Hamming.
    for (size_t i = 0; i < window_size; i++) {
        float angle = (2.0f * pi * (float)i) / (float)(window_size - 1);

        coefficients[i] = 0.54f - 0.46f * cosf(angle);
    }

    return true;
}

// =============================================================================
//  Aplicacion de la ventana de Hamming
// =============================================================================
bool window_apply(const Window *window, const int16_t *input, float *output) {
    // Validar punteros.
    if (window == NULL || input == NULL || output == NULL) {
        return false;
    }

    // Validar la configuracion del enventanador.
    if (window->coefficients == NULL || window->window_size < 2) {
        return false;
    }

    // Normalizar las muestras y aplicar los coeficientes de Hamming.
    for (size_t i = 0; i < window->window_size; i++) {
        float sample = (float)input[i] / 32768.0f;

        output[i] = sample * window->coefficients[i];
    }

    return true;
}

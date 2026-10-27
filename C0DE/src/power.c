/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/power.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion del calculo del espectro de potencia.
 *  Obtiene la magnitud al cuadrado de los coeficientes FFT.
 *
 * =============================================================================
 */

#include "power.h"

// =============================================================================
//  Calculo del espectro de potencia
// =============================================================================

bool power_compute(const FFTComplex *spectrum, float *output, size_t fft_size) {
    // Validar punteros.
    if (spectrum == NULL || output == NULL) {
        return false;
    }

    // Validar el tamano minimo de la transformada.
    if (fft_size < 2) {
        return false;
    }

    // Determinar el numero de componentes no redundantes.
    size_t bins = fft_size / 2 + 1;

    // Calcular la potencia de cada componente espectral.
    for (size_t k = 0; k < bins; k++) {
        float real = spectrum[k].real;
        float imag = spectrum[k].imag;

        output[k] = real * real + imag * imag;
    }

    return true;
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/fft.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion de la Transformada Rapida de Fourier.
 *  Algoritmo Cooley-Tukey Radix-2.
 *
 * =============================================================================
 */
#include "fft.h"

#include <math.h>

// =============================================================================
//  Inicializacion de la FFT
// =============================================================================

bool fft_init(FFT *fft, FFTComplex *twiddles, size_t fft_size) {
    // Validar punteros.
    if (fft == NULL || twiddles == NULL) {
        return false;
    }

    // Validar el tamano minimo.
    if (fft_size < 2) {
        return false;
    }

    // Validar que fft_size sea potencia de dos.
    if ((fft_size & (fft_size - 1)) != 0) {
        return false;
    }

    // Configurar almacenamiento y parametros.
    fft->fft_size = fft_size;
    fft->twiddles = twiddles;

    // Constante matematica PI.
    const float pi = 3.14159265358979323846f;

    // Calcular los factores de rotacion.
    for (size_t k = 0; k < fft_size / 2; k++) {
        float angle = (2.0f * pi * (float)k) / (float)fft_size;

        twiddles[k].real = cosf(angle);
        twiddles[k].imag = -sinf(angle);
    }

    return true;
}

// =============================================================================
//  Transformada Rapida de Fourier
// =============================================================================

bool fft_forward(const FFT *fft, const float *input, FFTComplex *output) {
    // Validar punteros.
    if (fft == NULL || input == NULL || output == NULL) {
        return false;
    }

    // Validar la configuracion.
    if (fft->twiddles == NULL || fft->fft_size < 2) {
        return false;
    }

    size_t n = fft->fft_size;

    // Verificar que el tamano sea potencia de dos.
    if ((n & (n - 1)) != 0) {
        return false;
    }

    // Reordenar las muestras mediante inversion de bits.
    size_t reversed = 0;

    for (size_t i = 0; i < n; i++) {
        output[reversed].real = input[i];
        output[reversed].imag = 0.0f;

        size_t bit = n >> 1;

        while (bit != 0 && (reversed & bit) != 0) {
            reversed ^= bit;
            bit >>= 1;
        }

        reversed ^= bit;
    }

    // Ejecutar las etapas de la FFT Radix-2.
    for (size_t span = 2; span <= n; span <<= 1) {
        size_t half = span >> 1;
        size_t step = n / span;

        // Recorrer cada grupo de muestras.
        for (size_t start = 0; start < n; start += span) {

            // Calcular las operaciones mariposa.
            for (size_t k = 0; k < half; k++) {
                FFTComplex w = fft->twiddles[k * step];

                FFTComplex u = output[start + k];
                FFTComplex v = output[start + k + half];

                // Multiplicacion compleja.
                FFTComplex t;

                t.real = w.real * v.real - w.imag * v.imag;
                t.imag = w.real * v.imag + w.imag * v.real;

                // Combinacion de resultados.
                output[start + k].real = u.real + t.real;
                output[start + k].imag = u.imag + t.imag;

                output[start + k + half].real = u.real - t.real;
                output[start + k + half].imag = u.imag - t.imag;
            }
        }

        // Evitar desbordamiento al duplicar span.
        if (span == n) {
            break;
        }
    }

    return true;
}

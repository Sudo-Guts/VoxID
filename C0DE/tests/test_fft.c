
/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_fft.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Pruebas de inicializacion de la Transformada Rapida de Fourier.
 *
 * =============================================================================
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "fft.h"

int main(void)
{
    FFT fft = {0};
    FFTComplex twiddles[4] = {0};

    const float tolerance = 0.00001f;

    // Inicializar una FFT de 8 puntos.
    assert(fft_init(&fft, twiddles, 8));

    // Verificar la configuracion.
    assert(fft.fft_size == 8);
    assert(fft.twiddles == twiddles);

    // Verificar los factores de rotacion.
    const float expected_real[4] = {
        1.0f, 0.70710678f, 0.0f, -0.70710678f
    };

    const float expected_imag[4] = {
        0.0f, -0.70710678f, -1.0f, -0.70710678f
    };

    for (size_t k = 0; k < 4; k++) {
        assert(fabsf(twiddles[k].real - expected_real[k]) < tolerance);
        assert(fabsf(twiddles[k].imag - expected_imag[k]) < tolerance);
    }

    // Verificar punteros invalidos.
    assert(!fft_init(NULL, twiddles, 8));
    assert(!fft_init(&fft, NULL, 8));

    // Verificar tamaños invalidos.
    assert(!fft_init(&fft, twiddles, 0));
    assert(!fft_init(&fft, twiddles, 1));
    assert(!fft_init(&fft, twiddles, 3));
    assert(!fft_init(&fft, twiddles, 6));

    // Una inicializacion fallida no debe alterar la configuracion.
    assert(fft.fft_size == 8);
    assert(fft.twiddles == twiddles);

    printf("OK: fft_init() funciona correctamente.\n");

    return 0;
}

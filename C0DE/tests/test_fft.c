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

static void test_fft_forward_impulso();
static void test_fft_forward_constante();
static void test_fft_forward_coseno();
static void test_fft_forward_seno();
static void test_fft_forward_minimo();

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
    test_fft_forward_impulso();
    test_fft_forward_constante();
    test_fft_forward_coseno();
    test_fft_forward_seno();
    test_fft_forward_minimo();

    return 0;
}


/*
 * =============================================================================
 *  Prueba de FFT con impulso unitario
 * =============================================================================
 */
static void test_fft_forward_impulso()
{
    FFT fft = {0};

    FFTComplex twiddles[4] = {0};
    FFTComplex output[8] = {0};

    const float input[8] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };

    const float tolerance = 0.00001f;

    // Inicializar la FFT.
    assert(fft_init(&fft, twiddles, 8));

    // Calcular la transformada.
    assert(fft_forward(&fft, input, output));

    // Verificar los coeficientes espectrales.
    for (size_t k = 0; k < 8; k++) {
        assert(fabsf(output[k].real - 1.0f) < tolerance);
        assert(fabsf(output[k].imag) < tolerance);
    }

    printf("OK: fft_forward() - impulso unitario.\n");
}


/*
 * =============================================================================
 *  Prueba de FFT con señal constante
 * =============================================================================
 */
static void test_fft_forward_constante(void)
{
    FFT fft = {0};

    FFTComplex twiddles[4] = {0};
    FFTComplex output[8] = {0};

    const float input[8] = {
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };

    const float tolerance = 0.00001f;

    // Inicializar la FFT.
    assert(fft_init(&fft, twiddles, 8));

    // Calcular la transformada.
    assert(fft_forward(&fft, input, output));

    // Verificar la componente de corriente continua (DC).
    assert(fabsf(output[0].real - 8.0f) < tolerance);
    assert(fabsf(output[0].imag) < tolerance);

    // Verificar que las demas componentes sean cero.
    for (size_t k = 1; k < 8; k++) {
        assert(fabsf(output[k].real) < tolerance);
        assert(fabsf(output[k].imag) < tolerance);
    }

    printf("OK: fft_forward() - señal constante.\n");
}


/*
 * =============================================================================
 *  Prueba de FFT con señal cosenoidal
 * =============================================================================
 */
static void test_fft_forward_coseno(void)
{
    FFT fft = {0};

    FFTComplex twiddles[4] = {0};
    FFTComplex output[8] = {0};

    const float input[8] = {
         1.0f,  0.70710678f,  0.0f, -0.70710678f,
        -1.0f, -0.70710678f,  0.0f,  0.70710678f
    };

    const float tolerance = 0.0001f;

    // Inicializar la FFT.
    assert(fft_init(&fft, twiddles, 8));

    // Calcular la transformada.
    assert(fft_forward(&fft, input, output));

    // Verificar los picos espectrales.
    assert(fabsf(output[1].real - 4.0f) < tolerance);
    assert(fabsf(output[7].real - 4.0f) < tolerance);

    // Verificar el resto del espectro.
    for (size_t k = 0; k < 8; k++) {
        if (k != 1 && k != 7) {
            assert(fabsf(output[k].real) < tolerance);
        }

        assert(fabsf(output[k].imag) < tolerance);
    }

    printf("OK: fft_forward() - señal cosenoidal.\n");
}


/*
 * =============================================================================
 *  Prueba de FFT con senal senoidal
 * =============================================================================
 */
static void test_fft_forward_seno(void)
{
    FFT fft = {0};

    FFTComplex twiddles[4] = {0};
    FFTComplex output[8] = {0};

    const float input[8] = {
         0.0f,  0.70710678f,  1.0f,  0.70710678f,
         0.0f, -0.70710678f, -1.0f, -0.70710678f
    };

    const float tolerance = 0.0001f;

    // Inicializar la FFT.
    assert(fft_init(&fft, twiddles, 8));

    // Calcular la transformada.
    assert(fft_forward(&fft, input, output));

    // Verificar las componentes imaginarias principales.
    assert(fabsf(output[1].imag + 4.0f) < tolerance);
    assert(fabsf(output[7].imag - 4.0f) < tolerance);

    // Verificar que las partes reales sean cero.
    for (size_t k = 0; k < 8; k++) {
        assert(fabsf(output[k].real) < tolerance);
    }

    // Verificar las demas componentes imaginarias.
    for (size_t k = 0; k < 8; k++) {
        if (k != 1 && k != 7) {
            assert(fabsf(output[k].imag) < tolerance);
        }
    }

    printf("OK: fft_forward() - senal senoidal.\n");
}


/*
 * =============================================================================
 *  Prueba de FFT con longitud minima
 * =============================================================================
 */
static void test_fft_forward_minimo(void)
{
    FFT fft = {0};

    FFTComplex twiddles[1] = {0};
    FFTComplex output[2] = {0};

    const float input[2] = {
        3.0f, 1.0f
    };

    const float tolerance = 0.00001f;

    // Inicializar una FFT de 2 puntos.
    assert(fft_init(&fft, twiddles, 2));

    // Calcular la transformada.
    assert(fft_forward(&fft, input, output));

    // Verificar el primer coeficiente.
    assert(fabsf(output[0].real - 4.0f) < tolerance);
    assert(fabsf(output[0].imag) < tolerance);

    // Verificar el segundo coeficiente.
    assert(fabsf(output[1].real - 2.0f) < tolerance);
    assert(fabsf(output[1].imag) < tolerance);

    printf("OK: fft_forward() - longitud minima.\n");
}

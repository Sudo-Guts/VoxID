/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_window.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Pruebas de inicializacion de la ventana de Hamming.
 *
 * =============================================================================
 */
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "window.h"

/**
 * @brief Verifica la normalizacion y el enventanado de muestras PCM.
 *
 * Comprueba la aplicacion de coeficientes de Hamming sobre
 * muestras positivas y negativas, asi como el manejo de
 * argumentos y estados invalidos.
 */
static void test_window_apply(void);

int main(void) {
    Window window = {0};
    float coefficients[5] = {0};

    const float expected[5] = {
        0.08f, 0.54f, 1.00f, 0.54f, 0.08f
    };

    // Verificar una configuracion valida.
    assert(window_init(&window, coefficients, 5));

    assert(window.window_size == 5);
    assert(window.coefficients == coefficients);

    // Verificar los coeficientes de Hamming.
    for (size_t i = 0; i < 5; i++) {
        assert(fabsf(coefficients[i] - expected[i]) < 0.00001f);
    }

    // Verificar parametros invalidos.
    assert(!window_init(NULL, coefficients, 5));
    assert(!window_init(&window, NULL, 5));
    assert(!window_init(&window, coefficients, 0));
    assert(!window_init(&window, coefficients, 1));

    // Una inicializacion fallida no debe modificar la configuracion.
    assert(window.window_size == 5);
    assert(window.coefficients == coefficients);

    printf("OK: window_init() funciona correctamente.\n");
    test_window_apply();

    return 0;
}

/*
 * =============================================================================
 *  Prueba de aplicacion de la ventana de Hamming
 * =============================================================================
 */
static void test_window_apply(void) {
    Window window = {0};

    float coefficients[5] = {0};
    float output[5] = {0};

    const int16_t input[5] = {
        -32768, -16384, 16384, 8192, 32767
    };

    const float expected[5] = {
        -0.08f, -0.27f, 0.50f, 0.135f, 0.07999756f
    };

    // Inicializar la ventana de Hamming.
    assert(window_init(&window, coefficients, 5));

    // Aplicar la ventana a las muestras de audio.
    assert(window_apply(&window, input, output));

    // Verificar los resultados del procesamiento.
    for (size_t i = 0; i < 5; i++) {
        assert(fabsf(output[i] - expected[i]) < 0.00001f);
    }

    // Verificar punteros invalidos.
    assert(!window_apply(NULL, input, output));
    assert(!window_apply(&window, NULL, output));
    assert(!window_apply(&window, input, NULL));

    // Verificar una estructura sin inicializar.
    Window invalid = {0};
    assert(!window_apply(&invalid, input, output));

    // Verificar una longitud de ventana invalida.
    invalid.coefficients = coefficients;
    invalid.window_size = 1;
    assert(!window_apply(&invalid, input, output));

    printf("OK: window_apply() funciona correctamente.\n");
}

/* 
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_frame.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Pruebas de inicializacion del segmentador de audio.
 *
 * =============================================================================
 */
#include <assert.h>
#include <stdio.h>

#include "frame.h"

/**
 * @brief Verifica la generacion de ventanas de audio.
 *
 * Comprueba el orden de las muestras, el desplazamiento entre
 * ventanas y los estados devueltos por el segmentador.
 */
static void test_frame_push(void);

/**
 * @brief Verifica la generacion de ventanas sin traslape.
 *
 * Comprueba que cada ventana contenga muestras independientes
 * cuando el desplazamiento es igual al tamaño de ventana.
 */
static void test_frame_no_overlap(void);

/**
 * @brief Verifica la segmentacion con desplazamiento de una muestra.
 *
 * Comprueba que se genere una ventana por cada muestra nueva
 * despues de completar la primera ventana.
 */
static void test_frame_min_hop(void);

/**
 * @brief Verifica el reinicio y cambio de configuracion del segmentador.
 *
 * Comprueba que los contadores se reinicien y que las muestras
 * anteriores no afecten la generacion de nuevas ventanas.
 */
static void test_frame_reinit(void);

/**
 * @brief Verifica el manejo de estados internos invalidos.
 *
 * Comprueba que frame_push() rechace estructuras sin inicializar,
 * punteros nulos y valores fuera de los limites configurados.
 */
static void test_frame_invalid_state(void);

int main(void)
{
    Frame frame = {0};
    int16_t buffer[8] = {0};

    // Verificar una configuracion valida.
    assert(frame_init(&frame, buffer, 8, 4));

    assert(frame.buffer == buffer);
    assert(frame.frame_size == 8);
    assert(frame.hop_size == 4);

    assert(frame.write_index == 0);
    assert(frame.count == 0);
    assert(frame.since_last_frame == 0);

    // Verificar parametros invalidos.
    assert(!frame_init(NULL, buffer, 8, 4));
    assert(!frame_init(&frame, NULL, 8, 4));
    assert(!frame_init(&frame, buffer, 0, 4));
    assert(!frame_init(&frame, buffer, 8, 0));
    assert(!frame_init(&frame, buffer, 8, 9));

    // Una inicializacion fallida no debe alterar el estado.
    assert(frame.frame_size == 8);
    assert(frame.hop_size == 4);

    printf("OK: frame_init() funciona correctamente.\n");

    test_frame_push();
    test_frame_no_overlap();
    test_frame_min_hop();
    test_frame_reinit();
    test_frame_invalid_state();

    return 0;
}

/* 
 * =============================================================================
 *  Prueba de segmentacion y traslape
 * =============================================================================
 */
static void test_frame_push(void)
{
    Frame frame = {0};

    int16_t buffer[4] = {0};
    int16_t output[4] = {0};

    const int16_t expected[3][4] = {
        {1, 2, 3, 4},
        {3, 4, 5, 6},
        {5, 6, 7, 8}
    };

    // Configurar ventanas de 4 muestras con avance de 2.
    assert(frame_init(&frame, buffer, 4, 2));

    size_t windows = 0;

    // Introducir las muestras del 1 al 8.
    for (int16_t sample = 1; sample <= 8; sample++)
    {
        FrameResult result = frame_push(&frame, sample, output);

        if (sample == 4 || sample == 6 || sample == 8)
        {
            assert(result == FRAME_READY);

            // Verificar el contenido de cada ventana.
            for (size_t i = 0; i < 4; i++)
            {
                assert(output[i] == expected[windows][i]);
            }

            windows++;
        }
        else
        {
            assert(result == FRAME_WAITING);
        }
    }

    // Verificar el numero total de ventanas.
    assert(windows == 3);

    // Verificar el manejo de punteros nulos.
    assert(frame_push(NULL, 0, output) == FRAME_ERROR);
    assert(frame_push(&frame, 0, NULL) == FRAME_ERROR);

    printf("OK: frame_push() funciona correctamente.\n");
}


/* 
 * =============================================================================
 *  Prueba de segmentacion sin traslape
 * =============================================================================
 */
static void test_frame_no_overlap(void)
{
    Frame frame = {0};

    int16_t buffer[4] = {0};
    int16_t output[4] = {0};

    const int16_t expected[2][4] = {
        {1, 2, 3, 4},
        {5, 6, 7, 8}
    };

    // Configurar ventanas sin traslape.
    assert(frame_init(&frame, buffer, 4, 4));

    size_t windows = 0;

    for (int16_t sample = 1; sample <= 8; sample++)
    {
        FrameResult result = frame_push(&frame, sample, output);

        if (sample == 4 || sample == 8)
        {
            assert(result == FRAME_READY);

            for (size_t i = 0; i < 4; i++)
            {
                assert(output[i] == expected[windows][i]);
            }

            windows++;
        }
        else
        {
            assert(result == FRAME_WAITING);
        }
    }

    assert(windows == 2);

    printf("OK: segmentacion sin traslape.\n");
}


/* 
 * =============================================================================
 *  Prueba de desplazamiento minimo
 * =============================================================================
 */
static void test_frame_min_hop(void)
{
    Frame frame = {0};

    int16_t buffer[4] = {0};
    int16_t output[4] = {0};

    const int16_t expected[5][4] = {
        {1, 2, 3, 4},
        {2, 3, 4, 5},
        {3, 4, 5, 6},
        {4, 5, 6, 7},
        {5, 6, 7, 8}
    };

    // Configurar el desplazamiento minimo.
    assert(frame_init(&frame, buffer, 4, 1));

    size_t windows = 0;

    // Introducir ocho muestras consecutivas.
    for (int16_t sample = 1; sample <= 8; sample++)
    {
        FrameResult result = frame_push(&frame, sample, output);

        if (sample >= 4)
        {
            assert(result == FRAME_READY);

            // Verificar el contenido de cada ventana.
            for (size_t i = 0; i < 4; i++)
            {
                assert(output[i] == expected[windows][i]);
            }

            windows++;
        }
        else
        {
            assert(result == FRAME_WAITING);
        }
    }

    assert(windows == 5);

    printf("OK: segmentacion con desplazamiento minimo.\n");
}


/* 
 * =============================================================================
 *  Prueba de reinicializacion del segmentador
 * =============================================================================
 */
static void test_frame_reinit(void)
{
    Frame frame = {0};

    int16_t buffer[4] = {0};
    int16_t output[4] = {0};

    const int16_t expected[3] = {10, 11, 12};

    // Configuracion inicial.
    assert(frame_init(&frame, buffer, 4, 2));

    // Recibir cinco muestras.
    for (int16_t sample = 1; sample <= 5; sample++)
    {
        FrameResult result = frame_push(&frame, sample, output);

        if (sample == 4)
        {
            assert(result == FRAME_READY);
        }
        else
        {
            assert(result == FRAME_WAITING);
        }
    }

    // Cambiar los parametros del segmentador.
    assert(frame_init(&frame, buffer, 3, 1));

    // Verificar la nueva configuracion.
    assert(frame.frame_size == 3);
    assert(frame.hop_size == 1);

    // Verificar que el estado se haya reiniciado.
    assert(frame.write_index == 0);
    assert(frame.count == 0);
    assert(frame.since_last_frame == 0);

    // Introducir nuevas muestras.
    for (int16_t sample = 10; sample <= 12; sample++)
    {
        FrameResult result = frame_push(&frame, sample, output);

        if (sample == 12)
        {
            assert(result == FRAME_READY);
        }
        else
        {
            assert(result == FRAME_WAITING);
        }
    }

    // Verificar que la ventana contenga solo muestras nuevas.
    for (size_t i = 0; i < 3; i++)
    {
        assert(output[i] == expected[i]);
    }

    printf("OK: reinicializacion del segmentador.\n");
}


/* 
 * =============================================================================
 *  Prueba de estados invalidos del segmentador
 * =============================================================================
 */
static void test_frame_invalid_state(void)
{
    Frame frame = {0};

    int16_t buffer[4] = {0};
    int16_t output[4] = {0};

    // Rechazar un segmentador sin inicializar.
    assert(frame_push(&frame, 1, output) == FRAME_ERROR);

    // Inicializar el segmentador correctamente.
    assert(frame_init(&frame, buffer, 4, 2));

    // Rechazar punteros nulos.
    assert(frame_push(NULL, 1, output) == FRAME_ERROR);
    assert(frame_push(&frame, 1, NULL) == FRAME_ERROR);

    // Rechazar un indice de escritura fuera del buffer.
    frame.write_index = 4;
    assert(frame_push(&frame, 1, output) == FRAME_ERROR);
    frame.write_index = 0;

    // Rechazar un contador superior al tamaño de ventana.
    frame.count = 5;
    assert(frame_push(&frame, 1, output) == FRAME_ERROR);
    frame.count = 0;

    // Rechazar un desplazamiento acumulado invalido.
    frame.since_last_frame = 2;
    assert(frame_push(&frame, 1, output) == FRAME_ERROR);
    frame.since_last_frame = 0;

    // Rechazar un hop_size mayor que frame_size.
    frame.hop_size = 5;
    assert(frame_push(&frame, 1, output) == FRAME_ERROR);
    frame.hop_size = 2;

    // Verificar que el segmentador pueda continuar.
    assert(frame_push(&frame, 1, output) == FRAME_WAITING);
    assert(frame.count == 1);

    printf("OK: validacion de estados invalidos.\n");
}

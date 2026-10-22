
/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/dct.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para la Transformada Discreta del Coseno tipo II.
 *  Utiliza normalizacion ortonormal para generar coeficientes MFCC.
 *
 * =============================================================================
 */

#ifndef DCT_H
#define DCT_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Estructura de configuracion de la DCT-II.
 */
typedef struct {
    size_t input_size;         // Numero de energias logaritmicas.
    size_t coefficient_count;  // Numero de coeficientes de salida.
    const float *basis;        // Coeficientes precalculados.
} DCT;

/**
 * @brief Inicializa la Transformada Discreta del Coseno.
 *
 * @param dct Puntero a la estructura de configuracion.
 * @param basis Arreglo para almacenar los coeficientes.
 * @param input_size Numero de elementos de entrada.
 * @param coefficient_count Numero de coeficientes de salida.
 *
 * @return true si la inicializacion fue correcta.
 * @return false si los argumentos son invalidos.
 *
 * @note coefficient_count debe estar entre 1 e input_size.
 * @note basis debe tener capacidad para
 *       input_size * coefficient_count elementos.
 * @note No se realiza asignacion dinamica de memoria.
 */
bool dct_init(DCT *dct, float *basis,
              size_t input_size, size_t coefficient_count);

/**
 * @brief Aplica la DCT-II sobre energias logaritmicas.
 *
 * @param dct Configuracion previamente inicializada.
 * @param input Arreglo de energias logaritmicas Mel.
 * @param output Arreglo de coeficientes MFCC.
 *
 * @return true si el procesamiento fue correcto.
 * @return false si los argumentos son invalidos.
 *
 * @note input debe contener input_size elementos.
 * @note output debe tener capacidad para coefficient_count elementos.
 * @note input y output no deben superponerse en memoria.
 */
bool dct_apply(const DCT *dct, const float *input, float *output);

#endif
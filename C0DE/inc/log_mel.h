
/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/log_mel.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para la transformacion logaritmica de energias Mel.
 *
 * =============================================================================
 */

#ifndef LOG_MEL_H
#define LOG_MEL_H

#include <stdbool.h>
#include <stddef.h>

/**
 * @brief Calcula el logaritmo natural de las energias Mel.
 *
 * @param input Arreglo de energias Mel.
 * @param output Arreglo de energias logaritmicas.
 * @param count Numero de energias Mel.
 * @param epsilon Valor minimo positivo antes del logaritmo.
 *
 * @return true si el procesamiento fue correcto.
 * @return false si los argumentos o las energias son invalidos.
 *
 * @note Los arreglos deben tener capacidad para count elementos.
 * @note Se permite utilizar el mismo arreglo como input y output.
 * @note No se permite superposicion parcial de arreglos distintos.
 * @note No se realiza asignacion dinamica de memoria.
 */
bool log_mel_apply(const float *input, float *output,
                   size_t count, float epsilon);

#endif

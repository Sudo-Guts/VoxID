/* 
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/inc/frame.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz para la segmentacion de muestras de audio en ventanas
 *  de longitud configurable, con soporte para traslape.
 *
 * =============================================================================
 */

#ifndef FRAME_H
#define FRAME_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Estados devueltos durante la segmentacion de audio.
 */
typedef enum {
    FRAME_ERROR = -1,
    FRAME_WAITING = 0,
    FRAME_READY = 1
} FrameResult;

/**
 * @brief Estructura de configuracion y estado del segmentador.
 */
typedef struct {
    size_t frame_size;        // Numero de muestras por ventana.
    size_t hop_size;          // Desplazamiento entre ventanas.
    size_t write_index;       // Posicion de escritura en el buffer.
    size_t count;             // Muestras almacenadas actualmente.
    size_t since_last_frame;  // Muestras recibidas desde la ultima ventana.
    int16_t *buffer;          // Almacenamiento de las muestras.
} Frame;

/**
 * @brief Inicializa el segmentador de audio.
 *
 * @param frame Puntero a la estructura del segmentador.
 * @param buffer Arreglo destinado al almacenamiento de muestras.
 * @param frame_size Numero de muestras por ventana.
 * @param hop_size Desplazamiento entre ventanas consecutivas.
 *
 * @return true si la inicializacion fue correcta.
 * @return false si los parametros son invalidos.
 *
 * @note El buffer debe tener capacidad para frame_size muestras.
 *       No se realiza asignacion dinamica de memoria.
 */
bool frame_init(Frame *frame, int16_t *buffer, size_t frame_size, size_t hop_size);

/**
 * @brief Agrega una muestra y genera una ventana cuando esta disponible.
 *
 * @param frame Puntero al segmentador previamente inicializado.
 * @param sample Muestra de audio PCM de 16 bits con signo.
 * @param output Arreglo donde se escribira la ventana completa.
 *
 * @return FRAME_READY si se genero una ventana.
 * @return FRAME_WAITING si aun no hay suficientes muestras.
 * @return FRAME_ERROR si se detecta un argumento o estado invalido.
 *
 * @note El arreglo output debe tener capacidad para frame_size
 *       muestras y no debe superponerse con el buffer interno.
 */
FrameResult frame_push(Frame *frame, int16_t sample, int16_t *output);

#endif
/* 
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/src/frame.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Implementacion del modulo de segmentacion de audio.
 *  Permite configurar ventanas de longitud y desplazamiento variables.
 *
 * =============================================================================
 */
#include "frame.h"

// =============================================================================
//  Inicializacion del segmentador
// =============================================================================
bool frame_init(Frame *frame, int16_t *buffer, size_t frame_size, size_t hop_size)
{
    // Validar punteros.
    if (frame == NULL || buffer == NULL) return false;

    // Validar dimensiones de la ventana y desplazamiento.
    if (frame_size == 0 || hop_size == 0 || hop_size > frame_size) return false;

    // Configurar almacenamiento y parametros.
    frame->buffer = buffer;
    frame->frame_size = frame_size;
    frame->hop_size = hop_size;

    // Reiniciar el estado del segmentador.
    frame->write_index = 0;
    frame->count = 0;
    frame->since_last_frame = 0;

    return true;
}

// =============================================================================
//  Recepcion y segmentacion de muestras
// =============================================================================
FrameResult frame_push(Frame *frame, int16_t sample, int16_t *output)
{
    // Validar punteros y configuracion.
    if (frame == NULL || output == NULL) return FRAME_ERROR;

    if (frame->buffer == NULL || frame->frame_size == 0 || frame->hop_size == 0 || frame->hop_size > frame->frame_size) return FRAME_ERROR;

    // Validar el estado interno.
    if (frame->write_index >= frame->frame_size || frame->count > frame->frame_size || frame->since_last_frame >= frame->hop_size) return FRAME_ERROR;

    // Almacenar la muestra en el buffer circular.
    frame->buffer[frame->write_index] = sample;
    frame->write_index = (frame->write_index >= frame->frame_size - 1) ? 0 : frame->write_index + 1;

    
    if (frame->count < frame->frame_size) {
        frame->count++;
        if (frame->count < frame->frame_size) return FRAME_WAITING;
    } else {    
        // Esperar el desplazamiento entre ventanas.
        frame->since_last_frame++;
        if (frame->since_last_frame < frame->hop_size) return FRAME_WAITING;
    }

    // Copiar las muestras en orden cronologico.
    size_t read_index = frame->write_index;

    for (size_t i = 0; i < frame->frame_size; i++) {
        output[i] = frame->buffer[read_index];
        read_index++;
        if (read_index == frame->frame_size) {
            read_index = 0;
        }
    }

    // Reiniciar el contador para la siguiente ventana.
    frame->since_last_frame = 0;

    return FRAME_READY;
}

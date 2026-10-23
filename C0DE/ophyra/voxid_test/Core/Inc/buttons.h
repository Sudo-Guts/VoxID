/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Inc/buttons.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Eventos de los cuatro pulsadores de Ophyra.
 *  El modulo no dibuja en pantalla ni bloquea interrupciones.
 *
 * =============================================================================
 */

#ifndef VOXID_BUTTONS_H
#define VOXID_BUTTONS_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    BUTTON_NONE = 0,
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_OK,
    BUTTON_BACK
} ButtonEvent;

/* Vaciar la cola y reiniciar el filtro de rebote. */
void buttons_init(void);

/*
 * Llamar desde HAL_GPIO_EXTI_Callback() con HAL_GetTick().
 * Solo registra pulsaciones validas; no realiza trabajo bloqueante.
 */
void buttons_on_exti(uint16_t gpio_pin, uint32_t tick_ms);

/* Extraer un evento desde el bucle principal; false si la cola esta vacia. */
bool buttons_next_event(ButtonEvent *event);

/* Numero de eventos descartados por cola llena (diagnostico). */
uint32_t buttons_dropped(void);

#endif /* VOXID_BUTTONS_H */

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid_test/Core/Inc/buttons.h
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

/**
 * @brief Vacia la cola y reinicia el antirrebote antes de habilitar los eventos.
 */
void buttons_init(void);

/**
 * @brief Registra una pulsacion desde el callback EXTI, sin dibujar ni esperar.
 * @param gpio_pin Pin del pulsador segun main.h.
 * @param tick_ms Tiempo actual en milisegundos.
 */
void buttons_on_exti(uint16_t gpio_pin, uint32_t tick_ms);

/**
 * @brief Extrae el siguiente evento desde el bucle principal.
 * @param event Destino del evento.
 * @return true si habia un evento; false si el puntero es nulo o la cola esta vacia.
 */
bool buttons_next_event(ButtonEvent *event);

/**
 * @brief Consulta la saturacion de la cola.
 * @return Cantidad de eventos descartados.
 */
uint32_t buttons_dropped(void);

#endif /* VOXID_BUTTONS_H */

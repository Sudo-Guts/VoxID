/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Src/buttons.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Antirrebote temporal y cola SPSC de eventos para EXTI.
 *  Productor: callbacks EXTI; consumidor: bucle principal.
 *
 * =============================================================================
 */

#include "buttons.h"
#include "main.h"

#include <stddef.h>

#define BUTTON_DEBOUNCE_MS  40U
#define BUTTON_QUEUE_SIZE   8U
#define BUTTON_QUEUE_MASK  (BUTTON_QUEUE_SIZE - 1U)

static volatile ButtonEvent event_queue[BUTTON_QUEUE_SIZE];
static volatile uint8_t queue_head;
static volatile uint8_t queue_tail;
static volatile uint32_t dropped_events;

/* Estas variables solamente se modifican desde las interrupciones EXTI. */
static uint32_t last_tick[4];
static uint8_t has_previous[4];

void buttons_init(void)
{
    queue_head = 0U;
    queue_tail = 0U;
    dropped_events = 0U;

    for (size_t i = 0U; i < 4U; ++i) {
        last_tick[i] = 0U;
        has_previous[i] = 0U;
    }
}

void buttons_on_exti(uint16_t gpio_pin, uint32_t tick_ms)
{
    ButtonEvent event;
    uint8_t index;

    /* Utilizar las etiquetas generadas en main.h por CubeIDE. */
    switch (gpio_pin) {
        case BTN_UP_Pin:
            event = BUTTON_UP;
            index = 0U;
            break;
        case BTN_DOWN_Pin:
            event = BUTTON_DOWN;
            index = 1U;
            break;
        case BTN_OK_Pin:
            event = BUTTON_OK;
            index = 2U;
            break;
        case BTN_BACK_Pin:
            event = BUTTON_BACK;
            index = 3U;
            break;
        default:
            return;
    }

    /* Resta sin signo: tambien funciona cuando HAL_GetTick() desborda. */
    if ((has_previous[index] != 0U) &&
        ((uint32_t)(tick_ms - last_tick[index]) < BUTTON_DEBOUNCE_MS)) {
        return;
    }

    has_previous[index] = 1U;
    last_tick[index] = tick_ms;

    uint8_t next = (uint8_t)((queue_head + 1U) & BUTTON_QUEUE_MASK);

    if (next == queue_tail) {
        ++dropped_events;
        return;
    }

    /* Publicar el indice solo despues de escribir el evento. */
    event_queue[queue_head] = event;
    queue_head = next;
}

bool buttons_next_event(ButtonEvent *event)
{
    if (event == NULL || queue_head == queue_tail) {
        return false;
    }

    *event = event_queue[queue_tail];
    queue_tail = (uint8_t)((queue_tail + 1U) & BUTTON_QUEUE_MASK);
    return true;
}

uint32_t buttons_dropped(void)
{
    return dropped_events;
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : ui.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz grafica de Ophyra, 160 x 128 pixeles.
 *  Navegacion por eventos; no dibujar desde interrupciones.
 *
 * =============================================================================
 */

#ifndef VOXID_UI_H
#define VOXID_UI_H

#include <stdbool.h>

// -----------------------------------------------------------------------------
//  Estados de interfaz
// -----------------------------------------------------------------------------

typedef enum {
    UI_STATE_MENU = 0,
    UI_STATE_IDENTIFY,
    UI_STATE_ENROLL,
    UI_STATE_SYSTEM,
    UI_STATE_NOTICE
} UIState;

// -----------------------------------------------------------------------------
//  Interfaz publica
// -----------------------------------------------------------------------------

/* Inicializar la UI tras st7735_init(); false si falla el dibujo. */
bool ui_init(void);

/* Procesar botones y actualizar unicamente las regiones necesarias. */
void ui_task(void);

/* Consultar la pantalla activa. */
UIState ui_state(void);

#endif /* VOXID_UI_H */

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/ui.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
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
    UI_STATE_CAPTURE,
    UI_STATE_SELFTEST
} UIState;

// -----------------------------------------------------------------------------
//  Interfaz publica
// -----------------------------------------------------------------------------

/**
 * @brief Inicializa la UI despues de st7735_init().
 * @return false si falla el dibujo.
 */
bool ui_init(void);

/**
 * @brief Procesa botones y actualiza la pantalla desde el bucle principal.
 */
void ui_task(void);

/**
 * @brief Consulta la pantalla activa.
 * @return Estado actual de la interfaz.
 */
UIState ui_state(void);

#endif /* VOXID_UI_H */

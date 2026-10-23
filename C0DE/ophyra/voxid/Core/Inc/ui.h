/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Inc/ui.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Interfaz de usuario horizontal (160 x 128) mediante maquina de estados.
 *  La UI se ejecuta fuera de las interrupciones.
 *
 * =============================================================================
 */

#ifndef VOXID_UI_H
#define VOXID_UI_H

typedef enum {
    UI_STATE_MENU = 0,
    UI_STATE_IDENTIFY,
    UI_STATE_ENROLL,
    UI_STATE_SYSTEM,
    UI_STATE_NOTICE
} UIState;

/* Inicializa la TFT en horizontal y muestra el menu principal. */
void ui_init(void);

/* Procesa eventos de botones y redibuja solamente cuando sea necesario. */
void ui_task(void);

/* Permite consultar el estado actual de la interfaz. */
UIState ui_state(void);

#endif /* VOXID_UI_H */

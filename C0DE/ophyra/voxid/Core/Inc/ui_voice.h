/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/ui_voice.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Indicador de tres circulos concentricos para el estado de captura
 *  y futuro procesamiento de voz, con paleta pastel sobre fondo claro.
 *
 * =============================================================================
 */

#ifndef VOXID_UI_VOICE_H
#define VOXID_UI_VOICE_H

#include <stdbool.h>
#include <stdint.h>

// =============================================================================
//  Parametros de animacion
// =============================================================================

#define UI_VOICE_PHASES 4U
#define UI_VOICE_TICK_MS 180U

// =============================================================================
//  Interfaz publica
// =============================================================================

/**
 * @brief Dibuja una fase de los tres circulos concentricos en la TFT.
 *
 * @param phase Indice de la fase, de 0 a UI_VOICE_PHASES - 1.
 * @param background Color RGB565 del fondo sobre el que se anima.
 *
 * @return true si el dibujo se completa correctamente.
 * @return false si la fase es invalida o falla un primitivo grafico.
 *
 * @note Redibuja solo un rectangulo de 64 x 56 pixeles.
 *       No contiene esperas, timers ni procesamiento de audio.
 *       Utilizar desde el bucle principal, nunca desde interrupciones.
 */
bool ui_voice_draw(uint8_t phase, uint16_t background);

#endif /* VOXID_UI_VOICE_H */

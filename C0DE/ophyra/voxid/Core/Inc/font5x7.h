/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Inc/font5x7.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *
 *  Fuente bitmap de 5x7 pixeles.
 *  Caracteres ASCII imprimibles.
 *
 * =============================================================================
 */

#ifndef FONT5X7_H
#define FONT5X7_H

#include <stdint.h>

// -----------------------------------------------------------------------------
//  Dimensiones de caracteres
// -----------------------------------------------------------------------------

#define FONT5X7_WIDTH         5U
#define FONT5X7_HEIGHT        7U
#define FONT5X7_SPACING       1U

// -----------------------------------------------------------------------------
//  Rango ASCII
// -----------------------------------------------------------------------------

#define FONT5X7_FIRST_CHAR    32U
#define FONT5X7_LAST_CHAR     126U
#define FONT5X7_CHAR_COUNT    (FONT5X7_LAST_CHAR - FONT5X7_FIRST_CHAR + 1U)

// -----------------------------------------------------------------------------
//  Mapa de caracteres
// -----------------------------------------------------------------------------

extern const uint8_t font5x7[FONT5X7_CHAR_COUNT][FONT5X7_WIDTH];

#endif /* FONT5X7_H */

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid_test/Core/Inc/ui.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  Monitor TFT exclusivo para el benchmark MFCC de voxid_test.
 * =============================================================================
 */
#ifndef VOXID_UI_H
#define VOXID_UI_H

#include "mfcc_benchmark.h"

/**
 * @brief Inicializa la pantalla exclusiva del benchmark.
 */
void ui_init(void);
/**
 * @brief Muestra el resumen del benchmark.
 * @param result Resultado medido.
 */
void ui_show_result(const MFCCBenchmarkResult *result);
/**
 * @brief Procesa la navegacion por paginas desde el bucle principal.
 */
void ui_task(void);

#endif /* VOXID_UI_H */

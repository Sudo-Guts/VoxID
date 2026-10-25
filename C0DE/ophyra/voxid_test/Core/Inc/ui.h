/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : Core/Inc/ui.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  Monitor TFT exclusivo para el benchmark MFCC de voxid_test.
 * =============================================================================
 */
#ifndef VOXID_UI_H
#define VOXID_UI_H

#include "mfcc_benchmark.h"

void ui_init(void);
void ui_show_result(const MFCCBenchmarkResult *result);
void ui_task(void);

#endif /* VOXID_UI_H */

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid_test/Core/Inc/mfcc_benchmark.h
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  Benchmark sintetico MFCC medido con DWT->CYCCNT.
 * =============================================================================
 */
#ifndef MFCC_BENCHMARK_H
#define MFCC_BENCHMARK_H

#include <stdint.h>
#include "voxid_dsp.h"

#define MFCC_BENCH_REPETITIONS  12U

typedef enum {
    MFCC_BENCH_NOT_RUN = 0,
    MFCC_BENCH_OK,
    MFCC_BENCH_DWT_ERROR,
    MFCC_BENCH_INIT_ERROR,
    MFCC_BENCH_PROCESS_ERROR,
    MFCC_BENCH_SEQUENCE_ERROR,
    MFCC_BENCH_VALUE_ERROR
} MFCCBenchmarkStatus;

typedef struct {
    MFCCBenchmarkStatus status;
    uint32_t cpu_hz;
    uint32_t repetitions;
    uint32_t frames;
    uint32_t hops;

    /* Latencia de la llamada mfcc_push() que devuelve MFCC_READY. */
    uint32_t frame_min_cycles;
    uint32_t frame_avg_cycles;
    uint32_t frame_max_cycles;

    /* Trabajo acumulado en las 160 llamadas entre dos frames. */
    uint32_t hop_min_cycles;
    uint32_t hop_avg_cycles;
    uint32_t hop_max_cycles;

    /* Ultimo vector (tercer frame de la ultima repeticion). */
    float mfcc[VOXID_MFCC_COUNT];
} MFCCBenchmarkResult;

/**
 * @brief Mide la cadena sintetica con DWT despues de configurar el reloj.
 * @param result Destino de tiempos, estado y ultimo vector MFCC.
 */
void mfcc_benchmark_run(MFCCBenchmarkResult *result);

#endif /* MFCC_BENCHMARK_H */

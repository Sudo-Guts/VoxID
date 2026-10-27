/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid_test/Core/Src/mfcc_benchmark.c
 *  Autor    : Gustavo Ruiz Luis
 * =============================================================================
 *  Entrada: 720 muestras sinteticas, senoide 440 Hz a 16 kHz.
 *  Salida: tres vectores MFCC por repeticion (400, 560 y 720).
 *
 *  Las muestras se generan ANTES de medir. No se mide TFT, sinf ni mfcc_init.
 *  "frame" incluye el procesamiento pesado de la llamada MFCC_READY.
 *  "hop" suma las 160 llamadas a mfcc_push entre frames consecutivos.
 *  No se habilitan ADC ni TIM2: prueba aislada, no prueba tiempo real.
 * =============================================================================
 */

#include "mfcc_benchmark.h"
#include "main.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define BENCH_SIGNAL_SIZE  720U
#define BENCH_FRAMES_PER_RUN 3U
#define BENCH_FREQUENCY_HZ 440.0f
#define BENCH_AMPLITUDE    12000.0f
#define BENCH_PI           3.14159265358979323846f

typedef struct {
    uint32_t min;
    uint32_t max;
    uint64_t sum;
    uint32_t count;
} CycleStats;

static int16_t test_signal[BENCH_SIGNAL_SIZE];

static void stats_add(CycleStats *stats, uint32_t cycles) {
    if (stats->count == 0U || cycles < stats->min) {
        stats->min = cycles;
    }
    if (stats->count == 0U || cycles > stats->max) {
        stats->max = cycles;
    }
    stats->sum += cycles;
    stats->count++;
}

static uint32_t stats_average(const CycleStats *stats) {
    return (stats->count != 0U)
    ? (uint32_t)(stats->sum / stats->count)
    : 0U;
}

static bool bench_enable_dwt(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0U;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0U;
}

static void generate_signal(void) {
    for (uint32_t n = 0U; n < BENCH_SIGNAL_SIZE; ++n) {
        const float angle = (2.0f * BENCH_PI * BENCH_FREQUENCY_HZ * (float)n) / (float)VOXID_SAMPLE_RATE;
        test_signal[n] = (int16_t)(BENCH_AMPLITUDE * sinf(angle));
    }
}

void mfcc_benchmark_run(MFCCBenchmarkResult *result) {
    if (result == NULL) {
        return;
    }

    (void)memset(result, 0, sizeof(*result));
    result->status = MFCC_BENCH_NOT_RUN;
    result->cpu_hz = SystemCoreClock;

    generate_signal();

    if (!bench_enable_dwt()) {
        result->status = MFCC_BENCH_DWT_ERROR;
        return;
    }

    /* Calentamiento: precarga de codigo y tablas, excluida de estadisticas. */
    if (!voxid_dsp_init()) {
        result->status = MFCC_BENCH_INIT_ERROR;
        return;
    }
    for (uint32_t n = 0U; n < BENCH_SIGNAL_SIZE; ++n) {
        const MFCCResult state = voxid_dsp_push(test_signal[n]);
        if (state == MFCC_ERROR) {
            result->status = MFCC_BENCH_PROCESS_ERROR;
            return;
        }
    }

    CycleStats frame_stats = {0};
    CycleStats hop_stats = {0};

    for (uint32_t run = 0U; run < MFCC_BENCH_REPETITIONS; ++run) {
        if (!voxid_dsp_init()) {
            result->status = MFCC_BENCH_INIT_ERROR;
            return;
        }

        uint32_t ready_count = 0U;
        uint64_t accumulated_hop_cycles = 0U;

        for (uint32_t n = 0U; n < BENCH_SIGNAL_SIZE; ++n) {
            const uint32_t start = DWT->CYCCNT;
            const MFCCResult state = voxid_dsp_push(test_signal[n]);
            const uint32_t cycles = (uint32_t)(DWT->CYCCNT - start);

            if (state == MFCC_ERROR) {
                result->status = MFCC_BENCH_PROCESS_ERROR;
                return;
            }

            accumulated_hop_cycles += cycles;

            const bool expected_ready = (n == 399U || n == 559U || n == 719U);
            if ((state == MFCC_READY) != expected_ready) {
                result->status = MFCC_BENCH_SEQUENCE_ERROR;
                return;
            }

            if (state == MFCC_READY) {
                ++ready_count;
                stats_add(&frame_stats, cycles);
                ++result->frames;

                /* Primer frame: 400 muestras; solo los siguientes son hops de 160. */
                if (ready_count > 1U) {
                    stats_add(&hop_stats, (uint32_t)accumulated_hop_cycles);
                    ++result->hops;
                }
                accumulated_hop_cycles = 0U;

                const float *coefficients = voxid_dsp_coefficients();
                for (uint32_t i = 0U; i < VOXID_MFCC_COUNT; ++i) {
                    if (!isfinite(coefficients[i])) {
                        result->status = MFCC_BENCH_VALUE_ERROR;
                        return;
                    }
                    if (n == 719U && run == MFCC_BENCH_REPETITIONS - 1U) {
                        result->mfcc[i] = coefficients[i];
                    }
                }
            }
        }

        if (ready_count != BENCH_FRAMES_PER_RUN) {
            result->status = MFCC_BENCH_SEQUENCE_ERROR;
            return;
        }
        result->repetitions++;
    }

    result->frame_min_cycles = frame_stats.min;
    result->frame_avg_cycles = stats_average(&frame_stats);
    result->frame_max_cycles = frame_stats.max;
    result->hop_min_cycles = hop_stats.min;
    result->hop_avg_cycles = stats_average(&hop_stats);
    result->hop_max_cycles = hop_stats.max;
    result->status = MFCC_BENCH_OK;
}

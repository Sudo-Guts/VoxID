/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/test_audio.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Regresiones de captura DMA y limpieza de errores con HAL simulada.
 *
 * =============================================================================
 */

#include "main.h"
#include "voxid_audio.h"
#include "voxid_voice.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

static void comprobar_captura(void) {
    VoxIDAudioStats adc;
    VoxIDVoiceStats voz;
    assert(voxid_audio_start());

    for (uint32_t b = 0U; b < VOXID_AUDIO_TOTAL_BLOCKS; ++b) {
        mock_dma_block((b % 2U) == 0U ? 2048U : 2058U);
        voxid_audio_task();
    }

    assert(voxid_audio_status() == VOXID_AUDIO_READY);
    assert(!mock_dma_running && !mock_timer_running);
    assert(voxid_audio_read_stats(&adc));
    assert(adc.count == 48000U && adc.minimum == 2048U && adc.maximum == 2058U && adc.average == 2053U);
    assert(voxid_voice_read_stats(&voz) && voz.frames == 298U);
}

static void comprobar_error(void) {
    VoxIDAudioStats adc;
    VoxIDVoiceStats voz;
    assert(voxid_audio_status() == VOXID_AUDIO_ERROR);
    assert(!mock_dma_running && !mock_timer_running);
    assert(!voxid_audio_read_stats(&adc));
    assert(!voxid_voice_read_stats(&voz));
}

int main(void) {
    mock_hal_reset();
    comprobar_captura();
    comprobar_captura();

    assert(voxid_audio_start());
    mock_dma_block(2048U);
    mock_dma_block(2048U);
    voxid_audio_task();
    comprobar_error();

    assert(voxid_audio_start());
    HAL_ADC_ErrorCallback(&hadc1);
    assert(!mock_timer_running && mock_dma_running);
    voxid_audio_task();
    comprobar_error();

    assert(voxid_audio_start());
    mock_dma_block(4096U);
    voxid_audio_task();
    comprobar_error();

    mock_fail_adc_start = true;
    assert(!voxid_audio_start());
    comprobar_error();
    mock_fail_adc_start = false;
    mock_fail_timer_start = true;
    assert(!voxid_audio_start());
    comprobar_error();
    mock_fail_timer_start = false;

    assert(voxid_audio_start());
    voxid_audio_stop();
    HAL_ADC_ConvHalfCpltCallback(&hadc1);
    assert(voxid_audio_status() == VOXID_AUDIO_IDLE && voxid_audio_sample_count() == 0U);
    comprobar_captura();

    voxid_audio_stop();
    hadc1.DMA_Handle = NULL;
    assert(!voxid_audio_start());
    voxid_audio_stop();
    puts("OK: DMA, estadisticas, repeticion, sobreescritura, IRQ, fallo HAL y cancelacion.");
    return 0;
}

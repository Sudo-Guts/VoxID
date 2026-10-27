/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/support/mock_hal.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Perifericos simulados para probar contratos de software sin una placa.
 *
 * =============================================================================
 */

#include "main.h"
#include "voxid_audio.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>

ADC_HandleTypeDef hadc1;
TIM_HandleTypeDef htim2;
I2C_HandleTypeDef hi2c1;
TestRCC test_rcc;
bool mock_dma_running;
bool mock_timer_running;
bool mock_fail_adc_start;
bool mock_fail_timer_start;
uint32_t mock_tick;

static DMA_HandleTypeDef dma;
static uint16_t *buffer_dma;
static uint32_t bloques_emitidos;

void mock_hal_reset(void) {
    dma = (DMA_HandleTypeDef){
        .Instance = DMA2_Stream0,
        .Init = {DMA_CHANNEL_0, DMA_CIRCULAR, DMA_MDATAALIGN_HALFWORD, DMA_PDATAALIGN_HALFWORD}
    };
    hadc1 = (ADC_HandleTypeDef){
        .Instance = ADC1,
        .Init = {ADC_RESOLUTION_12B, DISABLE, ADC_EXTERNALTRIGCONV_T2_TRGO, ADC_EXTERNALTRIGCONVEDGE_RISING, ENABLE},
        .DMA_Handle = &dma
    };
    htim2 = (TIM_HandleTypeDef){.Instance = TIM2, .Init = {0U, 5249U}};
    hi2c1 = (I2C_HandleTypeDef){.Instance = I2C1};
    test_rcc.CFGR = RCC_HCLK_DIV4;
    mock_dma_running = false;
    mock_timer_running = false;
    mock_fail_adc_start = false;
    mock_fail_timer_start = false;
    mock_tick = 0U;
    buffer_dma = NULL;
    bloques_emitidos = 0U;
}

uint32_t HAL_RCC_GetPCLK1Freq(void) {
    return 42000000U;
}

uint32_t HAL_GetTick(void) {
    return mock_tick;
}

void HAL_Delay(uint32_t demora) {
    mock_tick += demora;
}

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *timer) {
    (void)timer;
    mock_timer_running = !mock_fail_timer_start;
    return mock_fail_timer_start ? HAL_ERROR : HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *timer) {
    (void)timer;
    mock_timer_running = false;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *adc, uint32_t *data, uint32_t count) {
    (void)adc;
    assert(count == VOXID_AUDIO_DMA_SAMPLES);
    buffer_dma = (uint16_t *)data;
    bloques_emitidos = 0U;
    mock_dma_running = !mock_fail_adc_start;
    return mock_fail_adc_start ? HAL_ERROR : HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *adc) {
    assert(adc->DMA_Handle != NULL);
    mock_dma_running = false;
    return HAL_OK;
}

static uint16_t *mock_dma_next_half(void) {
    assert(mock_dma_running && mock_timer_running && buffer_dma != NULL);
    uint32_t offset = (bloques_emitidos % 2U) * VOXID_AUDIO_BLOCK_SAMPLES;
    return &buffer_dma[offset];
}

static void mock_dma_notify(void) {
    ++bloques_emitidos;
    mock_tick += 40U;

    if ((bloques_emitidos % 2U) != 0U) {
        HAL_ADC_ConvHalfCpltCallback(&hadc1);
    } else {
        HAL_ADC_ConvCpltCallback(&hadc1);
    }
}

void mock_dma_block(uint16_t value) {
    uint16_t *mitad = mock_dma_next_half();

    for (uint32_t i = 0U; i < VOXID_AUDIO_BLOCK_SAMPLES; ++i) {
        mitad[i] = value;
    }

    mock_dma_notify();
}

void mock_dma_tone_block(void) {
    uint16_t *mitad = mock_dma_next_half();

    for (uint32_t i = 0U; i < VOXID_AUDIO_BLOCK_SAMPLES; ++i) {
        uint32_t n = bloques_emitidos * VOXID_AUDIO_BLOCK_SAMPLES + i;
        float fase = 2.0f * 3.14159265358979323846f * 440.0f * (float)n / (float)VOXID_SAMPLE_RATE;
        mitad[i] = (uint16_t)(2048.0f + 900.0f * sinf(fase));
    }

    mock_dma_notify();
}

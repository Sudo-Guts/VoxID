/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_audio.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Captura finita de 3 segundos mediante ADC1, TIM2 TRGO y DMA circular.
 *  Las interrupciones solo notifican bloques completos. El resumen se
 *  calcula en el bucle principal, sin almacenar los 48000 samples en RAM.
 *
 * =============================================================================
 */

#include "voxid_audio.h"
#include "main.h"
#include "voxid_voice.h"

#include <stddef.h>

// =============================================================================
//  Perifericos generados por STM32CubeMX
// =============================================================================

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

// =============================================================================
//  Buffer DMA de dos mitades y estado sincronizado con interrupciones
// =============================================================================

static uint16_t adc_dma_buffer[VOXID_AUDIO_DMA_SAMPLES];
static volatile VoxIDAudioState audio_state = VOXID_AUDIO_IDLE;
static volatile uint32_t produced_blocks = 0U;
static volatile uint32_t consumed_blocks = 0U;

static uint32_t sample_sum = 0U;
static uint16_t sample_minimum = 4095U;
static uint16_t sample_maximum = 0U;
static uint32_t dropped_blocks = 0U;

// =============================================================================
//  Validacion de la frecuencia del temporizador y los perifericos
// =============================================================================

static bool voxid_audio_config_valid(void) {
    if (hadc1.Instance != ADC1 || htim2.Instance != TIM2 || hadc1.DMA_Handle == NULL) {
        return false;
    }

    if (hadc1.Init.Resolution != ADC_RESOLUTION_12B || hadc1.Init.ContinuousConvMode != DISABLE ||
        hadc1.Init.ExternalTrigConv != ADC_EXTERNALTRIGCONV_T2_TRGO ||
        hadc1.Init.ExternalTrigConvEdge != ADC_EXTERNALTRIGCONVEDGE_RISING ||
        hadc1.Init.DMAContinuousRequests != ENABLE) {
        return false;
    }

    if (htim2.Init.Prescaler != 0U || htim2.Init.Period != 5249U ||
        HAL_RCC_GetPCLK1Freq() != 42000000U ||
        (RCC->CFGR & RCC_CFGR_PPRE1) != RCC_HCLK_DIV4) {
        return false;
    }

    if (hadc1.DMA_Handle->Instance != DMA2_Stream0 ||
        hadc1.DMA_Handle->Init.Channel != DMA_CHANNEL_0 ||
        hadc1.DMA_Handle->Init.Mode != DMA_CIRCULAR ||
        hadc1.DMA_Handle->Init.MemDataAlignment != DMA_MDATAALIGN_HALFWORD ||
        hadc1.DMA_Handle->Init.PeriphDataAlignment != DMA_PDATAALIGN_HALFWORD) {
        return false;
    }

    return VOXID_AUDIO_TOTAL_SAMPLES % VOXID_AUDIO_BLOCK_SAMPLES == 0U;
}

// =============================================================================
//  Inicio y parada de adquisicion
// =============================================================================

bool voxid_audio_start(void) {
    if (audio_state == VOXID_AUDIO_CAPTURING) {
        return false;
    }

    if (!voxid_audio_config_valid()) {
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    (void)HAL_TIM_Base_Stop(&htim2);
    (void)HAL_ADC_Stop_DMA(&hadc1);

    if (!voxid_voice_begin()) {
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    produced_blocks = 0U;
    consumed_blocks = 0U;
    sample_sum = 0U;
    sample_minimum = 4095U;
    sample_maximum = 0U;
    dropped_blocks = 0U;
    __HAL_TIM_SET_COUNTER(&htim2, 0U);
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
    audio_state = VOXID_AUDIO_CAPTURING;

    // STM32F4: el puntero HAL es uint32_t*, pero el DMA usa halfword.
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, VOXID_AUDIO_DMA_SAMPLES) != HAL_OK) {
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        (void)HAL_ADC_Stop_DMA(&hadc1);
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    return true;
}

void voxid_audio_stop(void) {
    (void)HAL_TIM_Base_Stop(&htim2);

    if (hadc1.DMA_Handle != NULL) {
        (void)HAL_ADC_Stop_DMA(&hadc1);
    }

    audio_state = VOXID_AUDIO_IDLE;
    produced_blocks = 0U;
    consumed_blocks = 0U;
}

// =============================================================================
//  Consumir bloques DMA en el bucle principal
// =============================================================================

void voxid_audio_task(void) {
    if (audio_state != VOXID_AUDIO_CAPTURING) {
        return;
    }

    // Si existen dos bloques pendientes, la mitad antigua puede estar
    // siendo sobrescrita por DMA. No entregar estadisticas invalidas.
    if (produced_blocks - consumed_blocks > 1U) {
        ++dropped_blocks;
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_ERROR;
        (void)HAL_ADC_Stop_DMA(&hadc1);
        return;
    }

    if (consumed_blocks == produced_blocks) {
        return;
    }

    uint32_t block = consumed_blocks;
    uint32_t offset = (block % 2U) * VOXID_AUDIO_BLOCK_SAMPLES;

    for (uint32_t i = 0U; i < VOXID_AUDIO_BLOCK_SAMPLES; ++i) {
        uint16_t value = adc_dma_buffer[offset + i];

        if (value < sample_minimum) {
            sample_minimum = value;
        }

        if (value > sample_maximum) {
            sample_maximum = value;
        }

        sample_sum += value;
    }

    // El DSP consume la mitad del DMA fuera de IRQ.
    if (!voxid_voice_process_block(&adc_dma_buffer[offset], VOXID_AUDIO_BLOCK_SAMPLES)) {
        ++dropped_blocks;
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_ERROR;
        (void)HAL_ADC_Stop_DMA(&hadc1);
        return;
    }

    // Verificar otra vez: una operacion DSP lenta puede hacer que
    // el DMA vuelva a escribir la mitad que acabamos de leer.
    if (produced_blocks - block > 1U) {
        ++dropped_blocks;
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_ERROR;
        (void)HAL_ADC_Stop_DMA(&hadc1);
        return;
    }

    consumed_blocks = block + 1U;

    if (audio_state != VOXID_AUDIO_CAPTURING) {
        return;
    }

    if (consumed_blocks == VOXID_AUDIO_TOTAL_BLOCKS) {
        // El ultimo callback detuvo TIM2 al alcanzar 48000 muestras.
        if (HAL_TIM_Base_Stop(&htim2) != HAL_OK || HAL_ADC_Stop_DMA(&hadc1) != HAL_OK || !voxid_voice_finish()) {
            audio_state = VOXID_AUDIO_ERROR;
            return;
        }

        audio_state = VOXID_AUDIO_READY;
    }
}

// =============================================================================
//  Consultar avance y estadisticas
// =============================================================================

VoxIDAudioState voxid_audio_status(void) {
    return audio_state;
}

uint32_t voxid_audio_sample_count(void) {
    return consumed_blocks * VOXID_AUDIO_BLOCK_SAMPLES;
}

bool voxid_audio_read_stats(VoxIDAudioStats *stats) {
    if (stats == NULL || audio_state != VOXID_AUDIO_READY || consumed_blocks != VOXID_AUDIO_TOTAL_BLOCKS) {
        return false;
    }

    stats->minimum = sample_minimum;
    stats->maximum = sample_maximum;
    stats->average = (uint16_t)((sample_sum + VOXID_AUDIO_TOTAL_SAMPLES / 2U) / VOXID_AUDIO_TOTAL_SAMPLES);
    stats->count = VOXID_AUDIO_TOTAL_SAMPLES;
    stats->dropped_blocks = dropped_blocks;
    return true;
}

// =============================================================================
//  DMA: senalizar disponibilidad de bloques sin procesar dentro de IRQ
// =============================================================================

static void voxid_audio_block_ready(void) {
    if (audio_state != VOXID_AUDIO_CAPTURING) {
        return;
    }

    ++produced_blocks;

    if (produced_blocks == VOXID_AUDIO_TOTAL_BLOCKS) {
        // 75 eventos de 640 muestras: exactamente 3 segundos a 16 kHz.
        __HAL_TIM_DISABLE(&htim2);
    } else if (produced_blocks > VOXID_AUDIO_TOTAL_BLOCKS) {
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_ERROR;
    }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc != NULL && hadc->Instance == ADC1) {
        voxid_audio_block_ready();
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc != NULL && hadc->Instance == ADC1) {
        voxid_audio_block_ready();
    }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    if (hadc != NULL && hadc->Instance == ADC1 && audio_state == VOXID_AUDIO_CAPTURING) {
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_ERROR;
    }
}

/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/ophyra/voxid/Core/Src/voxid_audio.c
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Validacion inicial del ADC1 en PA2, sincronizado por TIM2 TRGO.
 *  Adquiere un bloque corto sin DMA y sin procesamiento en interrupciones.
 *
 * =============================================================================
 */

#include "voxid_audio.h"
#include "main.h"

#include <stddef.h>

// =============================================================================
//  Referencias a perifericos CubeMX
// =============================================================================

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;

// -----------------------------------------------------------------------------
//  Buffer de muestras sin asignacion dinamica
// -----------------------------------------------------------------------------

static volatile uint16_t adc_samples[VOXID_AUDIO_SAMPLES];
static volatile uint16_t adc_sample_count = 0U;
static volatile VoxIDAudioState audio_state = VOXID_AUDIO_IDLE;

// =============================================================================
//  Validacion de configuracion
// =============================================================================

static bool voxid_audio_config_valid(void) {
    if (hadc1.Instance != ADC1 || htim2.Instance != TIM2) {
        return false;
    }

    if (hadc1.Init.Resolution != ADC_RESOLUTION_12B || hadc1.Init.ContinuousConvMode != DISABLE) {
        return false;
    }

    if (hadc1.Init.ExternalTrigConv != ADC_EXTERNALTRIGCONV_T2_TRGO || hadc1.Init.ExternalTrigConvEdge != ADC_EXTERNALTRIGCONVEDGE_RISING) {
        return false;
    }

    if (htim2.Init.Prescaler != 0U || htim2.Init.Period != 5249U || HAL_RCC_GetPCLK1Freq() != 42000000U) {
        return false;
    }

    // El divisor APB1 es 4; por ello TIM2 recibe 84 MHz.
    return (RCC->CFGR & RCC_CFGR_PPRE1) == RCC_HCLK_DIV4;
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

    // Desarmar cualquier ejecucion anterior antes de vaciar el buffer.
    (void)HAL_TIM_Base_Stop(&htim2);
    (void)HAL_ADC_Stop_IT(&hadc1);

    adc_sample_count = 0U;
    __HAL_TIM_SET_COUNTER(&htim2, 0U);
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
    audio_state = VOXID_AUDIO_CAPTURING;

    // Armar el ADC antes de activar los disparos periodicos del timer.
    if (HAL_ADC_Start_IT(&hadc1) != HAL_OK) {
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        (void)HAL_ADC_Stop_IT(&hadc1);
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    return true;
}

void voxid_audio_stop(void) {
    (void)HAL_TIM_Base_Stop(&htim2);
    (void)HAL_ADC_Stop_IT(&hadc1);
    adc_sample_count = 0U;
    audio_state = VOXID_AUDIO_IDLE;
}

// =============================================================================
//  Estado y estadisticas
// =============================================================================

VoxIDAudioState voxid_audio_status(void) {
    return audio_state;
}

uint16_t voxid_audio_sample_count(void) {
    return adc_sample_count;
}

bool voxid_audio_read_stats(VoxIDAudioStats *stats) {
    if (stats == NULL || audio_state != VOXID_AUDIO_READY || adc_sample_count != VOXID_AUDIO_SAMPLES) {
        return false;
    }

    // Ya se detuvo TIM2 desde la interrupcion final; liberar ahora HAL ADC.
    if (HAL_TIM_Base_Stop(&htim2) != HAL_OK || HAL_ADC_Stop_IT(&hadc1) != HAL_OK) {
        audio_state = VOXID_AUDIO_ERROR;
        return false;
    }

    uint16_t minimum = 4095U;
    uint16_t maximum = 0U;
    uint32_t sum = 0U;

    for (uint16_t n = 0U; n < VOXID_AUDIO_SAMPLES; ++n) {
        uint16_t value = adc_samples[n];

        if (value < minimum) {
            minimum = value;
        }

        if (value > maximum) {
            maximum = value;
        }

        sum += value;
    }

    stats->minimum = minimum;
    stats->maximum = maximum;
    stats->average = (uint16_t)((sum + VOXID_AUDIO_SAMPLES / 2U) / VOXID_AUDIO_SAMPLES);
    stats->count = VOXID_AUDIO_SAMPLES;

    return true;
}

// =============================================================================
//  Callbacks ADC: trabajo minimo en interrupciones
// =============================================================================

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc == NULL || hadc->Instance != ADC1 || audio_state != VOXID_AUDIO_CAPTURING) {
        return;
    }

    uint16_t value = (uint16_t)HAL_ADC_GetValue(hadc);
    uint16_t index = adc_sample_count;

    if (index >= VOXID_AUDIO_SAMPLES) {
        return;
    }

    adc_samples[index] = value;
    adc_sample_count = index + 1U;

    if (adc_sample_count == VOXID_AUDIO_SAMPLES) {
        // Detener nuevos TRGO en la ultima muestra sin bloquear la ISR.
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_READY;
    }
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    if (hadc != NULL && hadc->Instance == ADC1 && audio_state == VOXID_AUDIO_CAPTURING) {
        __HAL_TIM_DISABLE(&htim2);
        audio_state = VOXID_AUDIO_ERROR;
    }
}

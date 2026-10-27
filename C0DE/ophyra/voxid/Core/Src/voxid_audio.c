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

static uint16_t buffer_adc[VOXID_AUDIO_DMA_SAMPLES];
static volatile VoxIDAudioState estado_audio = VOXID_AUDIO_IDLE;
static volatile uint32_t bloques_producidos = 0U;
static volatile uint32_t bloques_consumidos = 0U;
static volatile bool error_dma_pendiente = false;

static uint32_t suma_muestras = 0U;
static uint16_t minimo_adc = 4095U;
static uint16_t maximo_adc = 0U;
static uint32_t bloques_perdidos = 0U;

// =============================================================================
//  Validacion de la frecuencia del temporizador y los perifericos
// =============================================================================

static bool voxid_audio_config_valid(void) {
    if (hadc1.Instance != ADC1 || htim2.Instance != TIM2 || hadc1.DMA_Handle == NULL) {
        return false;
    }

    if (hadc1.Init.Resolution != ADC_RESOLUTION_12B || hadc1.Init.ContinuousConvMode != DISABLE || hadc1.Init.ExternalTrigConv != ADC_EXTERNALTRIGCONV_T2_TRGO || hadc1.Init.ExternalTrigConvEdge != ADC_EXTERNALTRIGCONVEDGE_RISING || hadc1.Init.DMAContinuousRequests != ENABLE) {
        return false;
    }

    if (htim2.Init.Prescaler != 0U || htim2.Init.Period != 5249U || HAL_RCC_GetPCLK1Freq() != 42000000U || (RCC->CFGR & RCC_CFGR_PPRE1) != RCC_HCLK_DIV4) {
        return false;
    }

    if (hadc1.DMA_Handle->Instance != DMA2_Stream0 || hadc1.DMA_Handle->Init.Channel != DMA_CHANNEL_0 || hadc1.DMA_Handle->Init.Mode != DMA_CIRCULAR || hadc1.DMA_Handle->Init.MemDataAlignment != DMA_MDATAALIGN_HALFWORD || hadc1.DMA_Handle->Init.PeriphDataAlignment != DMA_PDATAALIGN_HALFWORD) {
        return false;
    }

    return VOXID_AUDIO_SAMPLE_RATE == 16000U && VOXID_AUDIO_TOTAL_SAMPLES % VOXID_AUDIO_BLOCK_SAMPLES == 0U;
}

// =============================================================================
//  Inicio y parada de adquisicion
// =============================================================================

/** @brief Detiene los perifericos desde el bucle principal y descarta la toma. */
static void voxid_audio_fail(void) {
    (void)HAL_TIM_Base_Stop(&htim2);

    if (hadc1.DMA_Handle != NULL) {
        (void)HAL_ADC_Stop_DMA(&hadc1);
    }

    voxid_voice_cancel();
    error_dma_pendiente = false;
    estado_audio = VOXID_AUDIO_ERROR;
}

bool voxid_audio_start(void) {
    if (estado_audio == VOXID_AUDIO_CAPTURING) {
        return false;
    }

    if (!voxid_audio_config_valid()) {
        voxid_voice_cancel();
        estado_audio = VOXID_AUDIO_ERROR;
        return false;
    }

    (void)HAL_TIM_Base_Stop(&htim2);
    (void)HAL_ADC_Stop_DMA(&hadc1);

    if (!voxid_voice_begin()) {
        estado_audio = VOXID_AUDIO_ERROR;
        return false;
    }

    bloques_producidos = 0U;
    bloques_consumidos = 0U;
    error_dma_pendiente = false;
    suma_muestras = 0U;
    minimo_adc = 4095U;
    maximo_adc = 0U;
    bloques_perdidos = 0U;
    __HAL_TIM_SET_COUNTER(&htim2, 0U);
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_UPDATE);
    estado_audio = VOXID_AUDIO_CAPTURING;

    // STM32F4: el puntero HAL es uint32_t*, pero el DMA usa halfword.
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)buffer_adc, VOXID_AUDIO_DMA_SAMPLES) != HAL_OK) {
        voxid_audio_fail();
        return false;
    }

    if (HAL_TIM_Base_Start(&htim2) != HAL_OK) {
        voxid_audio_fail();
        return false;
    }

    return true;
}

void voxid_audio_stop(void) {
    (void)HAL_TIM_Base_Stop(&htim2);

    if (hadc1.DMA_Handle != NULL) {
        (void)HAL_ADC_Stop_DMA(&hadc1);
    }

    estado_audio = VOXID_AUDIO_IDLE;
    error_dma_pendiente = false;
    bloques_producidos = 0U;
    bloques_consumidos = 0U;
    voxid_voice_cancel();
}

// =============================================================================
//  Consumir bloques DMA en el bucle principal
// =============================================================================

void voxid_audio_task(void) {
    // El callback solo detiene TIM2 y notifica; la parada HAL queda fuera de IRQ.
    if (error_dma_pendiente) {
        voxid_audio_fail();
        return;
    }

    if (estado_audio != VOXID_AUDIO_CAPTURING) {
        return;
    }

    // Si existen dos bloques pendientes, la mitad antigua puede estar
    // siendo sobrescrita por DMA. No entregar estadisticas invalidas.
    if (bloques_producidos - bloques_consumidos > 1U) {
        ++bloques_perdidos;
        voxid_audio_fail();
        return;
    }

    if (bloques_consumidos == bloques_producidos) {
        return;
    }

    uint32_t bloque = bloques_consumidos;
    uint32_t inicio = (bloque % 2U) * VOXID_AUDIO_BLOCK_SAMPLES;

    for (uint32_t i = 0U; i < VOXID_AUDIO_BLOCK_SAMPLES; ++i) {
        uint16_t valor = buffer_adc[inicio + i];

        if (valor < minimo_adc) {
            minimo_adc = valor;
        }

        if (valor > maximo_adc) {
            maximo_adc = valor;
        }

        suma_muestras += valor;
    }

    // El DSP consume la mitad del DMA fuera de IRQ.
    if (!voxid_voice_process_block(&buffer_adc[inicio], VOXID_AUDIO_BLOCK_SAMPLES)) {
        voxid_audio_fail();
        return;
    }

    if (error_dma_pendiente) {
        voxid_audio_fail();
        return;
    }

    // Verificar otra vez: una operacion DSP lenta puede hacer que
    // el DMA vuelva a escribir la mitad que acabamos de leer.
    if (bloques_producidos - bloque > 1U) {
        ++bloques_perdidos;
        voxid_audio_fail();
        return;
    }

    bloques_consumidos = bloque + 1U;

    if (estado_audio != VOXID_AUDIO_CAPTURING) {
        return;
    }

    if (bloques_consumidos == VOXID_AUDIO_TOTAL_BLOCKS) {
        // El ultimo callback detuvo TIM2 al alcanzar 48000 muestras.
        if (HAL_TIM_Base_Stop(&htim2) != HAL_OK || HAL_ADC_Stop_DMA(&hadc1) != HAL_OK || !voxid_voice_finish()) {
            voxid_audio_fail();
            return;
        }

        estado_audio = VOXID_AUDIO_READY;
    }
}

// =============================================================================
//  Consultar avance y estadisticas
// =============================================================================

VoxIDAudioState voxid_audio_status(void) {
    return estado_audio;
}

uint32_t voxid_audio_sample_count(void) {
    return bloques_consumidos * VOXID_AUDIO_BLOCK_SAMPLES;
}

bool voxid_audio_read_stats(VoxIDAudioStats *stats) {
    if (stats == NULL || estado_audio != VOXID_AUDIO_READY || bloques_consumidos != VOXID_AUDIO_TOTAL_BLOCKS) {
        return false;
    }

    stats->minimum = minimo_adc;
    stats->maximum = maximo_adc;
    stats->average = (uint16_t)((suma_muestras + VOXID_AUDIO_TOTAL_SAMPLES / 2U) / VOXID_AUDIO_TOTAL_SAMPLES);
    stats->count = VOXID_AUDIO_TOTAL_SAMPLES;
    stats->dropped_blocks = bloques_perdidos;
    return true;
}

// =============================================================================
//  DMA: senalizar disponibilidad de bloques sin procesar dentro de IRQ
// =============================================================================

static void voxid_audio_block_ready(void) {
    if (estado_audio != VOXID_AUDIO_CAPTURING || error_dma_pendiente) {
        return;
    }

    ++bloques_producidos;

    if (bloques_producidos == VOXID_AUDIO_TOTAL_BLOCKS) {
        // 75 eventos de 640 muestras: exactamente 3 segundos a 16 kHz.
        __HAL_TIM_DISABLE(&htim2);
    } else if (bloques_producidos > VOXID_AUDIO_TOTAL_BLOCKS) {
        __HAL_TIM_DISABLE(&htim2);
        error_dma_pendiente = true;
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
    if (hadc != NULL && hadc->Instance == ADC1 && estado_audio == VOXID_AUDIO_CAPTURING) {
        __HAL_TIM_DISABLE(&htim2);
        error_dma_pendiente = true;
    }
}

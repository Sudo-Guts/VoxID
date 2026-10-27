/*
 * =============================================================================
 *  Proyecto : VoxID
 *  Archivo  : C0DE/tests/support/main.h
 *  Autor    : Gustavo Ruiz Luis
 * -----------------------------------------------------------------------------
 *
 *  Sustituto minimo de HAL para pruebas de adquisicion en PC.
 *  Este directorio no forma parte del proyecto STM32CubeIDE.
 *
 * =============================================================================
 */

#ifndef VOXID_TEST_MAIN_H
#define VOXID_TEST_MAIN_H

#include <stdbool.h>
#include <stdint.h>

#define DISABLE 0U
#define ENABLE 1U
#define ADC1 ((void *)1)
#define TIM2 ((void *)2)
#define DMA2_Stream0 ((void *)3)
#define ADC_RESOLUTION_12B 12U
#define ADC_EXTERNALTRIGCONV_T2_TRGO 1U
#define ADC_EXTERNALTRIGCONVEDGE_RISING 1U
#define DMA_CHANNEL_0 0U
#define DMA_CIRCULAR 1U
#define DMA_MDATAALIGN_HALFWORD 2U
#define DMA_PDATAALIGN_HALFWORD 2U
#define RCC_CFGR_PPRE1 0x1C00U
#define RCC_HCLK_DIV4 0x1400U
#define TIM_FLAG_UPDATE 1U
#define BTN_UP_Pin 4U
#define BTN_DOWN_Pin 32U
#define BTN_OK_Pin 16U
#define BTN_BACK_Pin 8U

typedef enum { HAL_OK = 0, HAL_ERROR = 1 } HAL_StatusTypeDef;

typedef struct {
    uint32_t Channel;
    uint32_t Mode;
    uint32_t MemDataAlignment;
    uint32_t PeriphDataAlignment;
} TestDMAInit;

typedef struct {
    void *Instance;
    TestDMAInit Init;
} DMA_HandleTypeDef;

typedef struct {
    uint32_t Resolution;
    uint32_t ContinuousConvMode;
    uint32_t ExternalTrigConv;
    uint32_t ExternalTrigConvEdge;
    uint32_t DMAContinuousRequests;
} TestADCInit;

typedef struct {
    void *Instance;
    TestADCInit Init;
    DMA_HandleTypeDef *DMA_Handle;
} ADC_HandleTypeDef;

typedef struct {
    uint32_t Prescaler;
    uint32_t Period;
} TestTIMInit;

typedef struct {
    void *Instance;
    TestTIMInit Init;
} TIM_HandleTypeDef;

typedef struct { uint32_t CFGR; } TestRCC;
extern TestRCC test_rcc;
#define RCC (&test_rcc)

extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern bool mock_dma_running;
extern bool mock_timer_running;
extern bool mock_fail_adc_start;
extern bool mock_fail_timer_start;
extern uint32_t mock_tick;

#define __HAL_TIM_DISABLE(timer) ((void)(timer), mock_timer_running = false)
#define __HAL_TIM_SET_COUNTER(timer, value) ((void)(timer), (void)(value))
#define __HAL_TIM_CLEAR_FLAG(timer, flag) ((void)(timer), (void)(flag))

/** @brief Reinicia perifericos simulados y fallos de prueba. */
void mock_hal_reset(void);

/**
 * @brief Llena una mitad DMA y ejecuta el callback correspondiente.
 * @param value Valor ADC constante del bloque.
 */
void mock_dma_block(uint16_t value);

/** @brief Reloj simulado. @return Frecuencia APB1 en Hz. */
uint32_t HAL_RCC_GetPCLK1Freq(void);
/** @brief Tiempo simulado. @return Milisegundos transcurridos. */
uint32_t HAL_GetTick(void);
/** @brief Inicia el temporizador simulado. @param timer Handle. @return Estado HAL. */
HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *timer);
/** @brief Detiene el temporizador simulado. @param timer Handle. @return Estado HAL. */
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *timer);
/** @brief Inicia DMA simulado. @param adc Handle. @param data Buffer. @param count Muestras. @return Estado HAL. */
HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *adc, uint32_t *data, uint32_t count);
/** @brief Detiene DMA simulado. @param adc Handle. @return Estado HAL. */
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *adc);
/** @brief Notifica media transferencia. @param adc Handle. */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *adc);
/** @brief Notifica transferencia completa. @param adc Handle. */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc);
/** @brief Notifica error ADC. @param adc Handle. */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *adc);

#endif /* VOXID_TEST_MAIN_H */

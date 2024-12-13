#include "drv_adc.h"
#include "stm32f1xx_hal.h"

static ADC_HandleTypeDef hadc1;

int Power_input_adc_init(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};

	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;		// ɨ��ģʽ: �Ƿ�ɨ����һ ͨ��channel
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		//Error_Handler();
		return -1;
	}
	
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;		// ͨ������, ���ǵ�һ��ͨ��
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	// ����ʱ��(Sampling time): 1.5 cycles, �� Cube MX �� cycle ��ʱ������ = 12MHz
	// ת��ʱ��(Tconv): 			Tconv = Sampling time + 12.5 cycles = 14 cycles = 7/6 us
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		//Error_Handler();
		return -1;
	}
	return 0;
}

static unsigned short Power_input_sample(void)	// ����ѹ
{
	if(HAL_ADC_Start(&hadc1) != HAL_OK)
		return 0xFFFF;	// �ɼ�λ��Ϊ12λ������16λ��Ϊ��
	if(HAL_ADC_PollForConversion(&hadc1, 1) != HAL_OK)
		return 0xFFFF;
	unsigned int value = HAL_ADC_GetValue(&hadc1);
	return (value&0xFFF);	// ʹ����λ���� 0xFFF���������������ǽ� value �����еĸ�16λ���㣬ֻ������12λ��
}

float Power_input_get(void)
{
	unsigned short data = Power_input_sample();
//	#define UINT_VOL (3.3 / 4096)			// ������ȷ��ֵ
//	float analogData = data * UINT_VOL;		// Ϊ�˼��ٸ���������
	float analogData = data * 3.3 / 4096;
	return (analogData * 5.99);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	if(adcHandle->Instance==ADC1)
	{
		/* ADC1 clock enable */
		__HAL_RCC_ADC1_CLK_ENABLE();
		
		__HAL_RCC_GPIOA_CLK_ENABLE();
		/*	 ADC1 GPIO Configuration   PA1------> ADC1_IN1  	*/
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{
	if(adcHandle->Instance==ADC1)
	{
		/* Peripheral clock disable */
		__HAL_RCC_ADC1_CLK_DISABLE();
		
		/*	ADC1 GPIO Configuration		PA1 ------> ADC1_IN1	*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
	}
}


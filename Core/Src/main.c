/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define Orange_LED_Pin GPIO_PIN_13
#define Orange_LED_GPIO_Port GPIOD
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
const uint8_t RTC_address = 0xD0;
uint8_t last_second, last_minute, last_hour;
uint8_t Time_reg[4];
uint8_t Time_settings[4];

/* Uart buffers */
char Ask_seconds[50] = "Enter seconds: \n";
char txData[50] = "Hello \n";
char rxData[50];

uint8_t alarm[2];
uint8_t alarm_reg[4];
uint8_t alarm_settings[4];
uint8_t alarm_int_byte = 0x05;

uint8_t control_reg 	= 0x0E;
uint8_t control_byte 	= 0x05; //enable alarm 1 (b0000 0101)

uint8_t status_reg		= 0x0F;	//status register
uint8_t status_byte;

bool b_is_alarm_triggered, b_button_pressed;
bool b_seconds_entered;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_DMA(&huart2, (uint8_t *)rxData, 1);

  HAL_UART_Transmit(&huart2,(uint8_t*) txData,strlen(txData), 50);

  //alarms
  alarm[0] = 0x01;
  alarm[1] = 0x02;

  //Time settings
  Time_reg[0] 			= 0x00;		//RTC register seconds
  Time_reg[1] 			= 0x01;		//RTC register minutes
  Time_reg[2] 			= 0x02;		//RTC register hours
  Time_reg[3] 			= 0x03;		//RTC register days

  Time_settings[0]    	= 0x00;		//seconds
  Time_settings[1] 		= 0x00;		//minutes
  Time_settings[2] 		= 0x65;		//hours (5 PM) (b01100101)
  Time_settings[3] 		= 0x01;		//day (Sunday)

  //Alarm settings
  alarm_reg[0] 			= 0x07;		//Alarm seconds reg
  alarm_reg[1] 			= 0x08;		//Alarm minutes reg
  alarm_reg[2] 			= 0x09;		//Alarm hours reg
  alarm_reg[3] 			= 0x0A;		//Alarm days reg

  alarm_settings[0]    	= 0x10;		//Alarm seconds
  alarm_settings[1] 	= 0x00;		//Alarm minutes
  alarm_settings[2]		= 0x65;		//Alarm hours (5 PM) (b0110 0101)
  alarm_settings[3]		= 0x41;		//Alarm day set (b0100 0001)

  //Set time
  Write_RTC(Time_reg[0], Time_settings[0]);
  Write_RTC(Time_reg[1], Time_settings[1]);
  Write_RTC(Time_reg[2], Time_settings[2]);
  Write_RTC(Time_reg[3], Time_settings[3]);

  //Set Alarm

  Write_RTC(alarm_reg[0], alarm_settings[0]);
  Write_RTC(alarm_reg[1], alarm_settings[1]);
  Write_RTC(alarm_reg[2], alarm_settings[2]);
  Write_RTC(alarm_reg[3], alarm_settings[3]);

  Clear_Alarm_IT(alarm[0]);
  Enable_Alarm_IT(alarm[0]);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	//Read time
	Display_Time();

	HAL_Delay(30);

	Read_RTC(&control_reg, &control_byte);

	Read_RTC(&status_reg, &status_byte);

	if(b_button_pressed)
	{
		HAL_UART_Transmit(&huart2,(uint8_t*) Ask_seconds,strlen(Ask_seconds), 50);
		HAL_Delay(1000);
		b_button_pressed = false;
		while(!b_seconds_entered);
		HAL_UART_Transmit(&huart2,(uint8_t*) rxData,strlen(rxData), 50);
		Write_RTC(Time_reg[0], rxData[0]);
		HAL_Delay(100);
		b_seconds_entered = false;
	}
	else
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	}

	if(b_seconds_entered)
	{
		HAL_UART_Transmit(&huart2,(uint8_t*) rxData,strlen(rxData), 50);
		Write_RTC(Time_reg[0], rxData[0]);
		HAL_Delay(100);
		b_seconds_entered = false;
	}

	if(b_is_alarm_triggered)
	{
		Display_Time();
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		Clear_Alarm_IT(alarm[0]);
		HAL_Delay(1000);
	}

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/* USER CODE BEGIN 4 */
void Write_RTC(uint8_t reg, uint8_t data)
{
	  static uint8_t payload[2];
	  payload[0] = reg;
	  payload[1] = data;
	  HAL_I2C_Master_Transmit(&hi2c1,RTC_address,payload,2,10);
	  HAL_Delay(30);
}

void Read_RTC(uint8_t* p_reg, uint8_t *p_data)
{
	//Ask slave to read data from register
	HAL_I2C_Master_Transmit(&hi2c1,RTC_address+1,p_reg,1,10);
	//Store data
	HAL_I2C_Master_Receive(&hi2c1,RTC_address,p_data,1,10);
	HAL_Delay(30);
}

void Enable_Alarm_IT(uint8_t alarm)
{
	Read_RTC(&control_reg, &control_byte);
	//Enable Alarm 1 interrupt
	if(alarm == 1)
	{
		control_byte =	 0x05;
	}
	//Enable Alarm 2 interrupt
	else
	{
		control_byte |=	 0x02;
	}

	Write_RTC(control_reg, control_byte);
}

void Clear_Alarm_IT(uint8_t alarm)
{
	Read_RTC(&status_reg, &status_byte);
	Read_RTC(&control_reg, &control_byte);

	//Clear Alarm 1 interrupt
	if(alarm == 1)
	{
		status_byte &=	 0xFE;
		control_byte &=	 0xFE;
	}
	//Clear Alarm 2 interrupt
	else
	{
		status_byte &=	 0xFD;
		control_byte &=	 0xFD;
	}

	Write_RTC(control_reg, control_byte);
	Write_RTC(status_reg, status_byte);
	b_is_alarm_triggered = false;
	HAL_Delay(500);
}

void Display_Time(void)
{
	Read_RTC(&Time_reg[0], &Time_settings[0]);
	Read_RTC(&Time_reg[1], &Time_settings[1]);
	Read_RTC(&Time_reg[2], &Time_settings[2]);
	Read_RTC(&Time_reg[3], &Time_settings[3]);

	if(last_second != Time_settings[0] || last_minute != Time_settings[1] || last_hour != Time_settings[2])
	{
		sprintf(txData, "%x:%x:%x\n",Time_settings[2], Time_settings[1], Time_settings[0]);
		HAL_UART_Transmit(&huart2,(uint8_t*) txData,strlen(txData), 10);
	}
	last_second = Time_settings[0];
	last_minute = Time_settings[1];
	last_hour	= Time_settings[2];
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1) == GPIO_PIN_RESET)
  {
	  b_is_alarm_triggered = true;
  }
  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
  {
	  b_button_pressed = true;
  }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

  HAL_UART_Transmit(&huart2,(uint8_t*) rxData,strlen(rxData), 50);

  b_seconds_entered = true;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

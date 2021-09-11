/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  * 
  * Description    : This project will start a binary 4-bit counter using external LEDs connected
  *                   on GPIO pins PD_7 - PD_11.
  *                   Serial communication is established via UART to monitor the count status.
  *                   Counter start / stop status is toggled if keyword "Toggle" is received via UART.
  * 
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

#include <string.h>
#include <iostream>
using namespace std;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* 
External LEDs -> GPIO Port_D  8, 9, 10, 11
*/

#define LED0 11
#define LED1 10
#define LED2 8
#define LED3 9

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */

char rxData[50];
int STOP_COUNT = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

void setPortDOutMode(int pinNum);
void sendPortDOutData(int pinNum, bool highSig);
int getPinNum(int LEDNum);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

class Counter
{
public:
  int tot_bits = 4;

private:
  int counterVal = 16;
  string order = "Up";
  int startVal = 0;
  int endVal = counterVal - 1;

  void setStartEndVals()
  {
    if (order == "Down")
    {
      startVal = -counterVal + 1;
      endVal = 0;
    }
    else
    {
      startVal = 0;
      endVal = counterVal - 1;
    }
  }

public:
  Counter() {}

  Counter(int value, string countOrder)
  {
    setCountValue(value);
    setCountOrder(countOrder);
  }

  Counter(int value)
  {
    setCountValue(value);
  }

  void setCountValue(int value)
  {
    counterVal = value;
    setStartEndVals();
  }

  void setCountOrder(string countOrder)
  {
    order = countOrder;
    setStartEndVals();
  }

  void startCounter()
  {
    for (int countVal = startVal; countVal <= endVal; countVal++)
    {
      if (STOP_COUNT)
      {
        break;
      }

      int num = countVal;
      if (order == "Down")
      {
        num *= -1;
      }

      char number[10];
      sprintf(number, "Current Number: %d\r\n", num);
      HAL_UART_Transmit(&huart2, (__uint8_t *)number, strlen(number), 10);

      writeLED(num);
      HAL_Delay(1000);
    }
  }

private:
  void writeLED(int num)
  {
    int bitVal, index = 0;
    for (int bitShift = tot_bits - 1; bitShift >= 0; bitShift--, index++)
    {
      bitVal = (num & (1 << bitShift)) >> bitShift;
      sendPortDOutData(getPinNum(bitShift), bitVal);
    }
  }
};

int getPinNum(int LEDNum)
{
  switch (LEDNum)
  {
  case 0:
    return LED0;

  case 1:
    return LED1;

  case 2:
    return LED2;

  default:
    return LED3;
  }
}

void setPortDOutMode(int pinNum)
{
  __uint32_t *pPortDMode = (__uint32_t *)0x40020C00;

  *pPortDMode &= ~(3 << (2 * pinNum));
  *pPortDMode |= (1 << (2 * pinNum));
}

void sendPortDOutData(int pinNum, bool highSig)
{
  __uint32_t *pPortDOut = (__uint32_t *)0x40020C14;

  if (highSig)
  {
    *pPortDOut |= (1 << pinNum);
  }
  else
  {
    *pPortDOut &= ~(1 << pinNum);
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // Connection Success Message
  char txdata[50] = "Connection Successful!\r\n";
  HAL_UART_Transmit(&huart2, (uint8_t *)txdata, strlen(txdata), 10);
  HAL_UART_Receive_DMA(&huart2, (uint8_t *)rxData, 6);

  // Enable clock for bus of GPIOD, i.e, AHB1
  // RCC base -> 40023800 + offset 30
  __uint32_t *pClockControl = (__uint32_t *)0x40023830;
  *pClockControl |= (1 << 3);

  //Port_D Pins Enable
  setPortDOutMode(LED0);
  setPortDOutMode(LED1);
  setPortDOutMode(LED2);
  setPortDOutMode(LED3);
  Counter c1;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    if (!STOP_COUNT)
    {
      c1.startCounter();
    }

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  huart2.Init.BaudRate = 115200;
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

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */

  char msg[50] = "Message received!\r\n";
  HAL_UART_Transmit(&huart2, (__uint8_t *)msg, strlen(msg), 10);

  if (strcmp(rxData, "Toggle") == 0)
  {
    STOP_COUNT ^= 1;
  }
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

#ifdef USE_FULL_ASSERT
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

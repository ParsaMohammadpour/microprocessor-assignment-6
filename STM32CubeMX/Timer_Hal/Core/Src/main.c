/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// LCD pins number in port A
#define d0 0 //PA0
#define d1 1 //PA1
#define d2 2 //PA2
#define d3 3 //PA3
#define d4 4 //PA4
#define d5 5 //PA5
#define d6 6 //PA6
#define d7 7 //PA07

#define E 8 //PA8
#define RW 9 //PA9
#define RS 10 //PA10

// push buttons in port B
#define button1 0 // PB0 button one
#define button2 1 // PB1 button two
#define button3 2 // PB2 button three

//MASKS:
#define SET1(x) (1ul << (x))
#define SET0(x) (~(SET1(x)))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
 TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
static char array [9] ;
static int status = -2; // 0 => nothing(counting)       1 => turn off was shown          -1 => turn off wasn't shown         2 => stop (2nd push button)   
//    -2 => start
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void MyInitialize(void);
void LCD_put_char(char data);
void HAL_Delay(uint32_t Delay);
void LCD_init(void);
void LCD_command(unsigned char command);
void LCD_resetCommand(void);
void LCD_setCommand(void);
int getNumber(char c);
char getChar(int digit);
void resetArray(void);
void firstButtonHandler(void);
void incMiliSecond(int index, int value);
void secondButtonHandler(void);
void thirdButtonClick(void);
void thirdButtonPressed(void);
void print_turnOff(void);
void clearTurnOff(void);
void tim3_Handler(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 ****************** Editing HAL_Delay Function Because It Added The Delay Time **************
 */
 void HAL_Delay(uint32_t Delay){ // It is same as the function in stm32f4xx_hal.c File, Line 390, But we commented the part which 
//was adding delay time
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  ///* Add a freq to guarantee minimum wait */
  //if (wait < HAL_MAX_DELAY)
  //{
  //  wait += (uint32_t)(uwTickFreq);
  //}

  while((HAL_GetTick() - tickstart) < wait)
  {
  }
}
/*
 ******************************************* My Functions ******************************************
 */
void MyInitialize(void){
	MX_TIM2_Init();
	MX_TIM3_Init();
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	LCD_init();
	LCD_setCommand();
	// setting all array indexes to " ". (we consider this as a null index in array)
	resetArray();
}

void LCD_put_char(char data) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, SET1(0) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET1(1) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET1(2) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET1(3) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET1(4) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET1(5) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET1(6) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET1(7) & data);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 1); // RS
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0); // RW
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1); // E
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0); // E
	// making the least possible delay for writing characters correctly in the LCD
	for(int i = 0; i < 17 ; i++)
		HAL_Delay(0);
}

void LCD_init(void){
	LCD_command(0x38);
	LCD_command(0x06);
	LCD_command(0x08);
	LCD_command(0x0C);
}

void LCD_command(unsigned char command){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, SET1(0) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, SET1(1) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, SET1(2) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, SET1(3) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, SET1(4) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, SET1(5) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, SET1(6) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, SET1(7) & command);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, 0); // RS
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, 0); // RW
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1); // E
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0); // E
	
  if (command < 4)
      HAL_Delay(2);           /* command 1 and 2 needs up to 1.64ms */
  else
      HAL_Delay(1);           /* all others 40 us */
}
void LCD_setCommand(void){ 
	LCD_command(0x01);
	LCD_command(0x38);
	//Welcome
	LCD_put_char('W');
	LCD_put_char('e');
	LCD_put_char('l');
	LCD_put_char('c');
	LCD_put_char('o');
	LCD_put_char('m');
	LCD_put_char('e');
	LCD_command(0xC0);
}

void LCD_resetCommand(void){
	LCD_command(0xC0);
	int i = 0;
	while(i < 9){
		LCD_put_char(array[i++]);
	}
}

int getNumber(char c){
	switch(c){
		case '0' : return 0;
		case '1' : return 1;
		case '2' : return 2;
		case '3' : return 3;
		case '4' : return 4;
		case '5' : return 5;
		case '6' : return 6;
		case '7' : return 7;
		case '8' : return 8;
		case '9' : return 9;
	}
	return -1;
}
char getChar(int digit){
	switch(digit){
		case 0 : return '0';
		case 1 : return '1';
		case 2 : return '2';
		case 3 : return '3';
		case 4 : return '4';
		case 5 : return '5';
		case 6 : return '6';
		case 7 : return '7';
		case 8 : return '8';
		case 9 : return '9';
	}
	return '&';
}
void resetArray(void){
	array[0] = '0';
	array[1] = '0';
	array[2] = ':';
	array[3] = '0';
	array[4] = '0';
	array[5] = ':';
	array[6] = '0';
	array[7] = '0';
	array[8] = '0';
	
}
void incMiliSecond(int index, int value){
	LCD_command(0x10);
	if(index == 2 || index == 5){
		incMiliSecond(index - 1, value);
		LCD_put_char(':');
		return;
	}
	int a = getNumber(array[index]);
	a += value;
	if(index == 3 && a == 6){
		incMiliSecond(index - 1, 1);
		a-= 6;
		array[index] = getChar(a);
		LCD_put_char(getChar(a));
	}else if((a >= 10)){
		incMiliSecond(index - 1, a / 10);
		a %= 10;
		array[index] = getChar(a);
		LCD_put_char(getChar(a));
	}else{
		array[index] = getChar(a);
		LCD_put_char(array[index]);
	}
}
void firstButtonHandler(void){
	HAL_NVIC_DisableIRQ(TIM3_IRQn);
	if(status != 0){
		status = 0;
		LCD_resetCommand();
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
}
void secondButtonHandler(void){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	if(status == 0)
		status = 2;
}
void thirdButtonHandler(void){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	
	
	HAL_NVIC_SetPendingIRQ(EXTI2_IRQn);
	
	int a = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)		== 0 ? 0 : 1;
	int isPressed = 1;
	
	
	// the folowiong is the exact body of the HAL_Delay
	uint32_t tickstart = HAL_GetTick();
  uint32_t wait = 200; // NUMBER OF THE TIME THAT WE ARE IN LOOP

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

	
	// if we just checked the GPIOB -> IDR at the end of the 3rd minuet, we would waste a lot of time
  while((HAL_GetTick() - tickstart) < wait)
  {
		if((a == 1) && ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0))){
			isPressed = 0;
			break;
		}
		if((a == 0) && ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 1))){
			isPressed = 0;
			break;
		}
  }
	if(isPressed == 1){
		thirdButtonPressed();
	}else{
		thirdButtonClick();
	}
	
	HAL_NVIC_ClearPendingIRQ(EXTI2_IRQn);
}
void thirdButtonClick(void){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	status = -2;
	resetArray();
	LCD_resetCommand();
}
void thirdButtonPressed(void){
	print_turnOff();
	resetArray();
	status = 1;
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
}
void print_turnOff(void){
	LCD_command(0xC0);
	LCD_put_char('T');
	LCD_put_char('u');
	LCD_put_char('r');
	LCD_put_char('n');
	LCD_put_char(' ');
	LCD_put_char('O');
	LCD_put_char('f');
	LCD_put_char('f');
	LCD_put_char(' ');
}
void clearTurnOff(void){ // for better looking, we set delays 30 mili seconds
	// but even 1 mili second was enough
	LCD_command(0xC0);
	LCD_put_char(' ');
	LCD_put_char(' ');
	LCD_put_char(' ');
	LCD_put_char(' ');
	LCD_put_char(' ');
	LCD_put_char(' ');
	LCD_put_char(' ');
	LCD_put_char(' ');
}
void tim3_Handler(void){
	HAL_NVIC_DisableIRQ(TIM2_IRQn);
	if(status == 1){
		clearTurnOff();
		status = -1;
	}else if (status == -1){
		print_turnOff();
		status = 1;
	}
}
/*
 ********************************** Intrrups ***********************************
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin & SET1(0)){ // First Button is pushed
		firstButtonHandler();
	} else if(GPIO_Pin & SET1(1)){ // second button is pushed
		secondButtonHandler();
	} else if(GPIO_Pin & SET1(2)){ // third button is pushed
		thirdButtonHandler();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim -> Instance == TIM2)
		incMiliSecond(8, 37);
	else if(htim -> Instance == TIM3)
		tim3_Handler();
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

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
	MyInitialize();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_TIM;
  PeriphClkInitStruct.TIMPresSelection = RCC_TIMPRES_ACTIVATED;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 15999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
	//TIM2 -> CR1 = 1; 
	htim2.Instance->CR1 = 1; // active
	htim2.Instance->DIER |= 1;
	//TIM2 -> DIER |= 1;
  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1599;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 500;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
	//TIM3 -> CR1 = 1; 
	htim3.Instance->CR1 = 1; // active
	htim3.Instance->DIER |= 1;
	//TIM3 -> DIER |= 1;
  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 PA7
                           PA8 PA9 PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

}

/* USER CODE BEGIN 4 */

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

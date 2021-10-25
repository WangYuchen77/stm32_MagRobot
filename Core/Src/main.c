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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "control.h"

 #define RXBUFFERSIZE  256     
 char info_buffer[RXBUFFERSIZE];   
 uint8_t aRxBuffer;            
 uint8_t Uart1_Rx_Cnt = 0;   
 
#define MAGNETIC_BUFFER_LEN 8
uint8_t magnetic_buffer[MAGNETIC_BUFFER_LEN];
uint8_t Uart3_Rx_Cnt = 0;

#define CARD_BUFFER_LEN 16
uint8_t card_buffer[CARD_BUFFER_LEN];
uint8_t Uart6_Rx_Cnt = 0;
 
enum Motor_state g_motor;

struct Robot g_robot;
struct Command g_command;


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int get_key_status(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET)
	{
			while(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET);
			return 1;
	}
	else
	{
			return 0;
	}
}
int robot_init()
{
	g_robot.m_speed = MOTOR_SPEED_INITIAL;
	g_robot.m_motor_left_value = MOTOR_SPEED_INITIAL;
	g_robot.m_motor_right_value = MOTOR_SPEED_INITIAL;
	g_robot.m_target = -1;
	g_robot.m_robot_status = READY;
	g_robot.m_direction = 1;
	
	g_motor = MOTOR_DISABLE;
	return 0;
}

int robot_enable()
{
	if (g_motor == MOTOR_DISABLE)
	{
		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2); 
		HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_2); 
		g_motor = MOTOR_ENABLE;
		printf("< robot > robot enable!\n");
	}
	else
	{
		//printf("< robot > already enable!\n");
	}
	return 0;
}

int robot_disable()
{
	if (g_motor == MOTOR_ENABLE)
	{
		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim8,TIM_CHANNEL_2); 
		g_motor = MOTOR_DISABLE;
		printf("< robot > robot disable!\n");
	}
	else
	{
		//printf("< robot > already disable!\n");
	}
	
	return 0;
}

int robot_run(int motor_left_value, int motor_right_value)
{
	robot_enable();
	
	if (g_robot.m_direction == 1)
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, motor_left_value);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, motor_right_value);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, 0);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, abs(motor_left_value));
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
		__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_2, abs(motor_right_value));
	}
	g_robot.m_robot_status = RUNNING;
	
	//printf("< robot > running! left speed: %d, right speed: %d, direction: %d\n", 
	//g_robot.m_motor_left_value, g_robot.m_motor_right_value, g_robot.m_direction);
	
	return 0;
}

int robot_speed_up(int value)
{
	g_robot.m_speed = g_robot.m_speed + value * g_robot.m_direction;
	if (abs(g_robot.m_speed) >= abs(MOTOR_SPEED_MAX))
	{
		g_robot.m_speed = MOTOR_SPEED_MAX * g_robot.m_direction;
	}
	if (abs(g_robot.m_speed) <= abs(MOTOR_SPEED_MIN))
	{
		g_robot.m_speed = MOTOR_SPEED_MIN * g_robot.m_direction;
	}
	return 0;
}

int robot_speed_down(int value)
{
	g_robot.m_speed = g_robot.m_speed - value * g_robot.m_direction;
	if (abs(g_robot.m_speed) >= abs(MOTOR_SPEED_MAX))
	{
		g_robot.m_speed = MOTOR_SPEED_MAX * g_robot.m_direction;
	}
	if (abs(g_robot.m_speed) <= abs(MOTOR_SPEED_MIN))
	{
		g_robot.m_speed = MOTOR_SPEED_MIN * g_robot.m_direction;
	}
	return 0;
}

int robot_direction_reverse()
{
	g_robot.m_direction = - g_robot.m_direction;
	g_robot.m_speed = -g_robot.m_speed;
	return 0;
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
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
	HAL_UART_Receive_IT(&huart3, magnetic_buffer, MAGNETIC_BUFFER_LEN);
	HAL_UART_Receive_IT(&huart6, card_buffer, CARD_BUFFER_LEN);\

	robot_init();
	printf("hello FUXI ROBOT!\n");
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_UART_Receive_IT(&huart3, magnetic_buffer, MAGNETIC_BUFFER_LEN);
		if (get_key_status(GPIOA, GPIO_PIN_0)) // enable motor
    {
			robot_enable();
			robot_run(g_robot.m_speed, g_robot.m_speed);
	
			printf("< key > motor 1 on\n");
			printf("< key > motor 1 speed up, current speed: %d, direction: %d\n", g_robot.m_speed, g_robot.m_direction);
    }
    if (get_key_status(GPIOG, GPIO_PIN_2)) // disable motor
    {
			robot_disable();
			g_robot.m_robot_status = READY;
			
			printf("< key > motor 1 off\n");
    }
    if (get_key_status(GPIOC, GPIO_PIN_13)) // motor speed up
    {
			robot_speed_up(MOTOR_SPEED_DELTA);
			robot_run(g_robot.m_speed, g_robot.m_speed);
			printf("< key > motor 1 speed up, current speed: %d, direction: %d\n", g_robot.m_speed, g_robot.m_direction);
    }
    if (get_key_status(GPIOG, GPIO_PIN_3)) // motor speed down 
    {
			robot_speed_down(MOTOR_SPEED_DELTA);
			robot_run(g_robot.m_speed, g_robot.m_speed);
			printf("< key > motor 1 speed down, current speed: %d, direction: %d\n", g_robot.m_speed, g_robot.m_direction);
    }
    if (get_key_status(GPIOG, GPIO_PIN_4)) // motor direction reverse
    {
			robot_direction_reverse();
			robot_run(g_robot.m_speed, g_robot.m_speed);
			printf("< key > motor 1 direction reverse, current speed: %d, direction: %d\n", g_robot.m_speed, g_robot.m_direction);
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */




int robot_walk_forward(){return 0;};
int robot_walk_rear(){return 0;};
int robot_walk_left(){return 0;};
int robot_walk_right(){return 0;};

int process_magnetic(uint8_t magnetic_buffer[MAGNETIC_BUFFER_LEN])
{
	uint8_t magnetic_value = magnetic_buffer[5];
	
	struct Robot r;
	r = translate_magnetic_2_motor(g_robot, magnetic_value);

	g_robot.m_motor_left_value = r.m_motor_left_value;
	g_robot.m_motor_right_value = r.m_motor_right_value;
	g_robot.m_magnetic_value = r.m_magnetic_value;
	for (int i = 0 ; i < MAGNETIC_BUFFER_LEN; i++)
	{
		g_robot.m_magnetic_status_0_1[i] = r.m_magnetic_status_0_1[i];
	}
	if (g_robot.m_robot_status == RUNNING)
	{
		robot_run(r.m_motor_left_value, r.m_motor_right_value);
	}

	return 0;
}

int process_card(int card_id)
{
	if (card_id == g_robot.m_target)
	{
		printf("< robot > arrived target %d!\n", card_id);
		g_robot.m_target = -1;
		g_robot.m_robot_status = READY;
		robot_disable();
	}
	else
	{
		printf("< robot > arrived at %d!\n", card_id);
	}
	printf("< robot > state: %d, speed: %d, target: %d, left: %d, right: %d, direction:%d\n",  
		g_robot.m_robot_status, 
		g_robot.m_speed, 
		g_robot.m_target,
		g_robot.m_motor_left_value,
		g_robot.m_motor_right_value,
		g_robot.m_direction
	);
	return 0;
}


int process_command(char info_buffer[256])
{
	g_command = translate_infor_2_command(info_buffer);
	
	switch (g_command.m_command_kind)
	{
		case RUN:
			printf("< answer >< OK >[ start ]\n");
			robot_run(g_robot.m_motor_left_value, g_robot.m_motor_right_value);
			g_robot.m_robot_status = RUNNING;
			break;
		case STOP:
			printf("< answer >< OK >[ stop ]\n");
			robot_disable();
			g_robot.m_robot_status = READY;
			break;
		case SET_SPEED:
			g_robot.m_speed = g_command.m_command_value; // speed can +/-
			printf("< answer >< OK >[ set ][ speed ]: %d\n", g_robot.m_speed );
			g_robot.m_motor_left_value = g_robot.m_speed;
			g_robot.m_motor_right_value = g_robot.m_speed;
			if (g_robot.m_robot_status == RUNNING) // + no collision + no obstacle
			{
				if (g_robot.m_speed < 0)
				{
					g_robot.m_direction = -1;
				}
				else
				{
					g_robot.m_direction = 1;
				}
				robot_run(g_robot.m_motor_left_value, g_robot.m_motor_right_value);
			}
			break;
		case SET_TARGET:
			g_robot.m_target = g_command.m_command_value;
			printf("< answer >< OK >[ set ][ target ]: %d\n", g_robot.m_target);	
			if (g_robot.m_robot_status != DERAILED) // + no collision + no obstacle
			{
				robot_run(g_robot.m_motor_left_value, g_robot.m_motor_right_value);
			}
			break;
		case CHARGE:
			printf("< answer >< OK >[ charge ]\n");
			g_robot.m_target = g_command.m_command_value;
			if (g_robot.m_robot_status != DERAILED) // + no collision + no obstacle
			{
				robot_run(g_robot.m_motor_left_value, g_robot.m_motor_right_value);
			}
			break;
		case WALK_FORWARD:
			printf("< answer >< OK >[ walk ][ forward ]\n");
			robot_walk_forward();
			robot_disable();
			g_robot.m_robot_status = READY;
			break;
		case WALK_REAR:
			printf("< answer >< OK >[ walk ][ rear ]\n");
			robot_walk_rear();
			robot_disable();
			g_robot.m_robot_status = READY;
			break;
		case WALK_LEFT:
			printf("< answer >< OK >[ walk ][ left ]\n");
			robot_walk_left();
			robot_disable();
			g_robot.m_robot_status = READY;
			break;
		case WALK_RIGHT:
			printf("< answer >< OK >[ walk ][ right ]\n");
			robot_walk_right();
			robot_disable();
			g_robot.m_robot_status = READY;
			break;
		case QUERY_ALL:
			query(g_robot, g_command, g_motor, 0);
			break;
		case QUERY_STATUS:
			query(g_robot, g_command, g_motor, 1);
			break;
		case QUERY_SPEED:
			query(g_robot, g_command, g_motor, 2);
			break;
		case QUERY_TARGET:
			query(g_robot, g_command, g_motor, 3);
			break;
		case COMMAND_ERROR:
			break;
	}
	printf("< robot > state: %d, speed: %d, target: %d, left: %d, right: %d, direction:%d\n",  
		g_robot.m_robot_status, 
		g_robot.m_speed, 
		g_robot.m_target,
		g_robot.m_motor_left_value,
		g_robot.m_motor_right_value,
		g_robot.m_direction
	);
	return 0;
}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
	if (huart == &huart1)
	{
		if(Uart1_Rx_Cnt >= 255)
		{
			Uart1_Rx_Cnt = 0;
			memset(info_buffer,0x00,sizeof(info_buffer));
			HAL_UART_Transmit(&huart1, (uint8_t *)"Data overflow!", 10,0xFFFF);     
		}
		else
		{
			info_buffer[Uart1_Rx_Cnt++] = aRxBuffer;
			if(info_buffer[Uart1_Rx_Cnt-3] == ';' && info_buffer[Uart1_Rx_Cnt-2] == 0x0D && info_buffer[Uart1_Rx_Cnt-1] == 0x0A)
			{
				process_command(info_buffer);

				Uart1_Rx_Cnt = 0;
				memset(info_buffer,0x00,sizeof(info_buffer)); 
			}
		}
		HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1); 
		
	}
	if (huart == &huart3)
	{
		//printf("s");
		process_magnetic(magnetic_buffer);
		HAL_UART_Receive_IT(&huart3, magnetic_buffer, MAGNETIC_BUFFER_LEN);
//		if (huart->RxState == HAL_UART_STATE_READY)
//		 {
//			 printf("0");
//		 }
//		 else
//		 {
//			 printf("1");
//		 }
	}
	if (huart == &huart6)
	{
		uint8_t card_id = card_buffer[15];
		process_card(card_id);
		HAL_UART_Receive_IT(&huart6, card_buffer, CARD_BUFFER_LEN);
//		HAL_UART_Receive_IT(&huart3, magnetic_buffer, MAGNETIC_BUFFER_LEN);
//		 if (huart->RxState == HAL_UART_STATE_READY)
//		 {
//			 printf("000000000");
//		 }
//		 else
//		 {
//			 printf("1111111");
//		 }
			 
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

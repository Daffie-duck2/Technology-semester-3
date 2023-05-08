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
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_ALT 2
#define LOW 0
#define HIGH 1
#define TOGGLE 2

#define ENCODER_DATA 0   
#define ENCODER_CLOCK 1 
#define CAL_LED 15        
#define INDICATOR_LED 12
#define SERVO_OUT 6
#define SERVO_IN 11 
//SERVO_IN=PA

volatile int encoderLastDir=0;
volatile bool encoderTicked;

volatile int servoTarget=100;
volatile int speed=0;

osMutexId_t led_delay;
osMutexId_t PIDA_value_mutex;

uint16_t leddelay =250;

float Protected_Kp =15.8364600623519;
float Protected_Ki =19.8024420766591;
float Protected_Kd =1.03008409800077;
volatile bool PIDValueUpdateFlag = false;

const osMutexAttr_t Thread_Mutex_attr = {
  "ledDelay",
  osMutexPrioInherit,
  NULL,
  0U
};

const osMutexAttr_t MsgQueueMutexAttr = {
  "PIDA_value_mutex",
  osMutexPrioInherit,
  NULL,
  0U
};

const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .attr_bits = osThreadDetached,
    .cb_mem = NULL,
    .cb_size = 0,
    .stack_mem = NULL,
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
    .tz_module = 0,
    .reserved = 0};


void SystemClock_Config(void);
void MX_FREERTOS_Init(void);

GPIO_TypeDef *resolvePort(char portname){ 
  switch(portname){
    case 'a': return GPIOA; 
    case 'b': return GPIOB; 
    case 'c': return GPIOC; 
    case 'd': return GPIOD; 
  }
  return nullptr;
}
TIM_TypeDef *resolveTimer(uint timerNumber){
  switch(timerNumber){
    case 3: return TIM3; break; 
    case 4: return TIM4; break;
  }
  return nullptr;
}

void timerInit(uint timerNumber, uint prescaler, uint period, uint initialPulseWidth){
  TIM_TypeDef *timer = resolveTimer(timerNumber);
  uint RCCOffset;
  switch(timerNumber){
    case 3: RCCOffset=RCC_APB1ENR_TIM3EN_Pos; break;
    case 4: RCCOffset=RCC_APB1ENR_TIM4EN_Pos; break;
  }
  RCC->APB1ENR &= ~(0b1 <<RCCOffset);
  RCC->APB1ENR |= 0b1 <<RCCOffset;
  timer->PSC = prescaler;
  if(timerNumber==3){
    timer->ARR = period;
    timer->CCMR1 &= ~0b11;
    timer->CCMR1 &= ~0b1111 <<TIM_CCMR1_OC1M_Pos;
    timer->CCMR1 |= 0b0110 <<TIM_CCMR1_OC1M_Pos;
    timer->CCR1 = initialPulseWidth;
    timer->CCER |= 0b1;
  }
  if(timerNumber==4){
    timer->CCMR1 &= ~(0b11 << TIM_CCMR1_CC1S_Pos);
    timer->CCMR1 |=  0b01 << TIM_CCMR1_CC1S_Pos;
    timer->CCER  &= ~(0b1 << TIM_CCER_CC1P_Pos);
    timer->CCER  &= ~(0b1 << TIM_CCER_CC1NP_Pos);
    timer->CCER |= 0b1 <<TIM_CCER_CC1E_Pos;
    timer->CCMR1 &= ~(0b11 << TIM_CCMR1_CC2S_Pos);
    timer->CCMR1 |=  0b10 << TIM_CCMR1_CC2S_Pos;
    timer->CCER &= ~(0b1 << TIM_CCER_CC2NP_Pos);
    timer->CCER &= ~(0b1 << TIM_CCER_CC2P_Pos);
    timer->CCER |= 0b0 << TIM_CCER_CC2NP_Pos;
    timer->CCER |= 0b1 << TIM_CCER_CC2P_Pos;
    timer->CCER |= 0b1 <<TIM_CCER_CC2E_Pos;
    timer->SMCR &= ~(0b111 <<TIM_SMCR_TS_Pos);
    timer->SMCR |= 0b101 <<TIM_SMCR_TS_Pos;
    timer->SMCR &= ~(0b111 <<TIM_SMCR_SMS_Pos);
    timer->SMCR |= 0b100 <<TIM_SMCR_SMS_Pos;
  }
  timer->CR1 |= 0b1;
  if(timerNumber==3){
    timer->EGR |= TIM_EGR_UG;
    timer->SR = ~TIM_SR_UIF;
  }
}

void setTimerPulsewidth(uint timerNumber, int widthInUS){
  TIM_TypeDef *timer = resolveTimer(timerNumber);
  timer->CCR1 = widthInUS;
}

void setTimerPeriod(uint timerNumber, int periodInUS, bool overridePulseWidth=true){ //zet hier een interrupt aan, testen met osc
  TIM_TypeDef *timer = resolveTimer(timerNumber);
  timer->ARR = periodInUS;
  if(overridePulseWidth)timer->CCR1 = periodInUS/2;
}

int pinMode(int pin, uint mode, char port='a')//0=in, 1=out, 2=alt
{
  GPIO_TypeDef *PinPort = resolvePort(port);
  PinPort->MODER &= ~(0b11<<2*pin); 

  if(mode==GPIO_OUTPUT){
    PinPort->MODER |=  0b01<<2*pin;
    
  } else if(mode==GPIO_ALT){
    PinPort->MODER |= 0b10 << pin*2;
  } else {
    return -1;
  }
  return 0;
}

void pinPullMode(int pin, uint pullmode, char port='a') //1=up, 2=down
{
  GPIO_TypeDef *PinPort = resolvePort(port);
  PinPort->PUPDR &= ~(0b00<<2*pin); 
  switch(pullmode)
  {
    case GPIO_PULLUP: PinPort->PUPDR |=  0b01<<2*pin;
            break;
    case GPIO_PULLDOWN: PinPort->PUPDR |=  0b10<<2*pin;
            break;
  }
}

void setPin(int pin, uint mode, char port='a'){ //0=low 1=high 2=toggle
  GPIO_TypeDef *PinPort = resolvePort(port);
  switch(mode){
    case HIGH: PinPort->ODR |= 0b1<<pin;
            break;
    case TOGGLE: PinPort->ODR ^= (0b1 << pin);
            break;
    case LOW: 
    default: PinPort->ODR &= ~(0b1<<pin);
            break;
  }
}

void UARTInputThread(void *argument){
  char receivedMsg[80];
  int msgPointer=0;
  uint8_t rxChar;
  HAL_StatusTypeDef status;

  //program reception of msges here.
    
  while(1){
    
    USART2->ICR |= USART_ICR_ORECF;
    status = HAL_UART_Receive(&huart2, &rxChar, 1, 0);
    if (status == HAL_OK) // No HAL_TIMEOUT, so a character is read.
    {
      if(rxChar=='\n'||rxChar=='\r'){
        receivedMsg[msgPointer++]='\0';

        int value=0;
        float fValue=0;
        bool floatFlag=false;
        char valueText[80];
        int count=0;

        for(int i=1; receivedMsg[i]!=NULL; i++){
          if(receivedMsg[i]=='.'||receivedMsg[i]==','){
            floatFlag=true;
         }
          valueText[i-1]=receivedMsg[i];
          count++;
        }

        valueText[count]='\0';
        if(floatFlag){
          fValue=atof(valueText);
        } else {
          value=atoi(valueText);
        }

        osMutexAcquire(PIDA_value_mutex,osWaitForever);
          switch(receivedMsg[0]){
            case 'P': Protected_Kp = fValue; break;
            case 'I': Protected_Ki = fValue; break;
            case 'D': Protected_Kd = fValue; break;
            case 'A': servoTarget = value; break;
          default: break;
          }
        osMutexRelease(PIDA_value_mutex);
        PIDValueUpdateFlag=true;
        msgPointer=0;
        
      } else {
        receivedMsg[msgPointer++]=rxChar;
      }
    }
    osDelay(1);
  }
}

void PIDThread(void *argument){
   

  float Kp =15.8364600623519;
  float Ki =19.8024420766591;
  float Kd =1.03008409800077;
  int32_t integral=0;
  int32_t derivative=0;
  int32_t error=0;
  int32_t previous_error=0;

  while(1){
    error = (TIM4->CCR2) - (servoTarget);
    integral = integral + error;
    if (error == 0)
    { 
      integral = 0;
    }
    if ( abs(error) > 40)
    { 
      integral = 0;  
    }
    derivative = error - previous_error;
    previous_error = error;
    speed=(Kp*error + Ki*integral + Kd*derivative)/20;
    if(speed>220)speed=120; //was 220
    if(speed<-220)speed=-120;
    setTimerPulsewidth(3,1500+speed);

    //Daphne's solution)
    if(PIDValueUpdateFlag){
      
      if(osMutexAcquire(PIDA_value_mutex,0)== osOk){ // hier kan hij blijven hangen door de osWaitForever je wilt liever opvragen of de mutex vrij is met de null en dan pas de mutex claimen
      
        Kp = Protected_Kp;
        Ki = Protected_Ki;
        Kd = Protected_Kd;
      osMutexRelease(PIDA_value_mutex);}
      
      PIDValueUpdateFlag=false;
    }

    osDelay(20);//had 20 ms moeten zijn net als in de simulatie ivm met de integral/ afgeleiden!
  }
}

void outputThread(void *argument){
  char txBuf[80];
  int cycles=0;

  snprintf(txBuf,80,"System booted!\n");
  HAL_UART_Transmit(&huart2,(uint8_t *)txBuf, strlen(txBuf), HAL_MAX_DELAY);
  while(1){
    if(encoderTicked){
      snprintf(txBuf,80,"Ticked!");
      HAL_UART_Transmit(&huart2,(uint8_t *)txBuf, strlen(txBuf), HAL_MAX_DELAY);
    }
      uint32_t debugCCR1 = TIM4->CCR1;
      uint32_t debugCCR2 = TIM4->CCR2;
      uint32_t debugCNT = TIM4->CNT;
      if(cycles>=200){
        snprintf(txBuf,80,"pos: %lu, TGT: %lu, SPD: %li\n",TIM4->CCR2, servoTarget, speed);
        HAL_UART_Transmit(&huart2,(uint8_t *)txBuf, strlen(txBuf), HAL_MAX_DELAY);
        cycles=0;
      } else {
        cycles = cycles+1;
      }
    osDelay(1);
  }
  
}

void inputThread(void *argument){
  while(1){
    if(encoderTicked)
    {
      GPIOA->ODR ^= (1 << 5);
      encoderTicked=false;
      osMutexAcquire(led_delay,osWaitForever);
        leddelay += encoderLastDir*10;
        //osDelay(1000);
      osMutexRelease(led_delay);
      servoTarget = (servoTarget + encoderLastDir*10) %1000;
      encoderLastDir=0;
    }
    osDelay(1);
    IWDG->KR=0xAAAA;
  }
}
/*
void secondThread(void *argument){
  int internalDelay=250;
  while(1){
    setPin(INDICATOR_LED,TOGGLE);
    osMutexAcquire(led_delay,osWaitForever);
      internalDelay=leddelay;
    osMutexRelease(led_delay);
    setTimerPulsewidth(3,1000+internalDelay);
    osDelay(internalDelay);
  }
}
*/

void mainThread(void *argument){
  led_delay = osMutexNew(&Thread_Mutex_attr);
  while(1){
    setPin(CAL_LED,TOGGLE);
    osDelay(500u);
  }
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void){

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  osKernelInitialize(); /* Call init function for freertos objects (in freertos.c) */
  //MX_FREERTOS_Init();
  PIDA_value_mutex = osMutexNew(&MsgQueueMutexAttr);
  osThreadNew(mainThread,  NULL, &defaultTask_attributes);
  osThreadNew(outputThread,NULL, &defaultTask_attributes);
  osThreadNew(inputThread, NULL, &defaultTask_attributes);
  //osThreadNew(secondThread,NULL, &defaultTask_attributes);
  osThreadNew(PIDThread, NULL, &defaultTask_attributes);
  osThreadNew(UARTInputThread, NULL, &defaultTask_attributes);

    // prepare Interrupts
  SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI1) | (0b0000 << SYSCFG_EXTICR1_EXTI1_Pos); // pin PA0 to interrupt EXTI*
  EXTI->RTSR |= EXTI_RTSR_TR1;   // Set interrupt EXTI* trigger to rising edge
  EXTI->IMR |= EXTI_IMR_MR1;     // Unmask EXTI* line
  NVIC_EnableIRQ(EXTI1_IRQn);   // Enable EXTI* interrupt

  timerInit(3,72,20000,1000);
  timerInit(4,72,0,0); //feedback channel

  pinMode(CAL_LED,GPIO_OUTPUT);
  pinMode(INDICATOR_LED,GPIO_OUTPUT);
  pinMode(ENCODER_DATA,GPIO_INPUT);
  pinMode(ENCODER_CLOCK,GPIO_INPUT);
  pinPullMode(ENCODER_DATA,GPIO_PULLDOWN);
  pinPullMode(ENCODER_CLOCK,GPIO_PULLDOWN);

  pinMode(SERVO_OUT,GPIO_ALT);
  pinMode(SERVO_IN,GPIO_ALT);
  GPIOA->AFR[0] &= ~(0b1111 << GPIO_AFRL_AFRL6_Pos);
  GPIOA->AFR[0] |= 0b0010 << GPIO_AFRL_AFRL6_Pos;

  GPIOA->AFR[1] &= ~(0b1111 << GPIO_AFRH_AFRH3_Pos);
  GPIOA->AFR[1] |= 0b1010 << GPIO_AFRH_AFRH3_Pos;
  
  /*
  IWDG->KR= 0x5555;
  IWDG->PR = 0b101;
  IWDG->RLR = 3;
  IWDG->KR = 0x0000;
  IWDG->KR = 0xCCCC;
  */
  osKernelStart(); /* Start scheduler */

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */

}

extern "C" void EXTI1_IRQHandler(void)  // Encoder rotation tick
{
    EXTI->PR = EXTI_PR_PR1;    // Reset interrupt flag of EXTI*. This is needed otherwise the interrupt handler will be called again.
    if((GPIOA->IDR & 0b1<<ENCODER_DATA) >0 ){
      encoderLastDir=-1;
    } else{
      encoderLastDir=1;
    }
    encoderTicked=1;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

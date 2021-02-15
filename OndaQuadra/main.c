#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

#define Tck 1.388888889*pow(10,-8)

int cont=0;
int cnt;
float periodo;
int flag=0;
int flag2=0;
int main()
{RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
RCC->AHBENR |=RCC_AHBENR_GPIOFEN;
GPIOA->MODER &= ~GPIO_MODER_MODER2;
GPIOF->MODER |= GPIO_MODER_MODER4_0;

RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
TIM2->ARR =136404000;//272808000;
TIM2->CNT=0;
TIM2->DIER |=TIM_DIER_UIE;
RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
TIM6->ARR =65536-1;
TIM6->PSC =65536-1;
TIM6->CNT=0;

RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN;
SYSCFG->EXTICR[0] |=SYSCFG_EXTICR1_EXTI2_PA;
NVIC->ISER[0] |= (1<<8);
NVIC->ISER[0] |= (1<<28);
EXTI->IMR |= EXTI_IMR_MR2;
EXTI->RTSR |= EXTI_RTSR_TR2;
TIM2->CR1 |=TIM_CR1_CEN;
  while(1);
}


void EXTI2_TS_IRQHandler(){
  EXTI->PR |=EXTI_PR_PR2;
  if(flag==0){
  TIM6->CNT=0;
  TIM6->CR1 |=TIM_CR1_CEN;
  flag=1;
  }else if(flag==1){
    cnt=TIM6->CNT;
    TIM6->CR1 &= ~TIM_CR1_CEN;
    periodo=(float)(cnt*Tck*65536);
    printf("Periodo onda quadra %d: %f s \n",cont++,periodo);
    flag=0;
  }
}


void TIM2_IRQHandler(){
  TIM2->SR &= ~TIM_SR_UIF;
  if(flag2==0){
    GPIOF->ODR |=GPIO_ODR_4;
    flag2=1;
  }else if(flag2==1){
    GPIOF->ODR &= ~GPIO_ODR_4;
    flag2=0;
  }
}
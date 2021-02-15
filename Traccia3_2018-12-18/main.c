#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

#define Tck 1.388888889*pow(10,-8)

int flag=0;
int flag2=0;
float cnt;
int cont=0;
float risultato;

int main()
{ RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
RCC->AHBENR |= RCC_AHBENR_GPIOFEN;

GPIOA->MODER |=GPIO_MODER_MODER2_0;
GPIOF->MODER &= ~GPIO_MODER_MODER4; //LO SAREBBE ANCHE DI DEFAULT

RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;  
TIM2->ARR =136404000;
TIM6->ARR =65536-1;
TIM6->PSC =65536-1;
TIM2->DIER |=TIM_DIER_UIE;
NVIC->ISER[0] |= (1<<28);

RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN;
SYSCFG->EXTICR[1] |=SYSCFG_EXTIRCR_EXTI4_PF;
EXTI->IMR |=EXTI_IMR_MR4;
EXTI->RTSR |=EXTI_RTSR_TR4;
NVIC->ISER[0] |= (1<<10);
TIM2->CR1 |=TIM_CR1_CEN;
 while(1);
}
void EXTI4_IRQHandler(){
EXTI->PR |= EXTI_PR_PR4;
if(flag2==0){
  //printf("inizio a contare\n");
   TIM6->CNT=0;
  TIM6->CR1 |=TIM_CR1_CEN;
  flag2=1;
}else if(flag2==1){
  
   cnt=(float)TIM6->CNT;
   
    risultato=(float)(cnt*Tck*65536);
    printf("Il periodo dell'onda quadra %d è: %f s\n",cont++,risultato);
    TIM6->CR1 &= ~TIM_CR1_CEN;
      flag2=0;
}

}
void TIM2_IRQHandler(){
  TIM2->SR &= ~TIM_SR_UIF;
  if(flag==0){
    GPIOA->ODR |=GPIO_ODR_2;
    flag=1;
  }else if(flag==1){
    GPIOA->ODR &= ~GPIO_ODR_2;
    flag=0;
  }
}
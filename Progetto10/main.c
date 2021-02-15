#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>
#define Tck 1388888889*pow(10,-17)*65535

unsigned int stato=0;
unsigned int flag=0;
unsigned int n=0;
int Ncnt=0;
float t;
int main()
{RCC->AHBENR |=RCC_AHBENR_GPIOBEN;//PER I PIN
RCC->APB2ENR |=RCC_APB2ENR_SYSCFGEN;//PER SYSCFG PER EXTI
RCC->APB1ENR |=RCC_APB1ENR_TIM6EN;//PER I TIMER
RCC->APB1ENR |=RCC_APB1ENR_TIM7EN;
GPIOB->MODER |=GPIO_MODER_MODER0_0;// POSTO PB0 COME USCITA, MENTRE PB1 è COME INGRESSO DI DEFAULT
GPIOB->MODER &= ~GPIO_MODER_MODER1;
GPIOB->ODR &= ~GPIO_ODR_0;
TIM6->PSC = 7199;
TIM6->ARR = 50000;
TIM7->ARR = 65535;
TIM7->PSC = 65535;
TIM6->CNT=0;
TIM7->CNT=0;
NVIC->ISER[1] |= (1<<22);
TIM6->DIER |= TIM_DIER_UIE;
SYSCFG->EXTICR[0] |=SYSCFG_EXTICR1_EXTI1_PB;
EXTI->IMR |= EXTI_IMR_MR1;
EXTI->FTSR |= EXTI_FTSR_TR1;
NVIC->ISER[0] |= (1<<7);
TIM6->CR1 |= TIM_CR1_CEN;
  while(1);
}
void TIM6_DAC_IRQHandler(){
  //printf("Entrato in interrupt TIM6 \n");
  TIM6->SR &= ~TIM_SR_UIF;
  if(flag==0){
    GPIOB->ODR |= GPIO_ODR_0;
    flag=1;
  }else{
    GPIOB->ODR &= ~GPIO_ODR_0;
    flag=0;
  }
}


void EXTI1_IRQHandler(){
  //printf("Entrato in interrupt EXTI \n");
  EXTI->PR |=EXTI_PR_PR1;
  if(stato==0){
    TIM7->CR1 |=TIM_CR1_CEN;
    stato=1;
  }else{
    TIM7->CR1 &=~TIM_CR1_CEN;
    Ncnt=TIM7->CNT;
    TIM7->CNT =0;
      t=(float)Ncnt*Tck;
      printf("PERIODO MISURATO %d: %f SECONDI\n",n,t);
      n++;
      stato=0;
  }
  
}
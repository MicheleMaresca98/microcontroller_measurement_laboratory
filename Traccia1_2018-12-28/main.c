#include <stm32f30x.h>
#include <math.h>
#define PI 3.141592654
#define N 100
unsigned short int LUT[N];
unsigned short int LUT2_d[N];
float LUT2_a[N];
int cont=0;
int cont2=0;
float ampiezza=1.46484375;
float offset=1.5;
float Vsin;
int main()
{
  for(int i=0;i<N;i++){
    Vsin=offset+ ampiezza*sin(2*PI*((float)i/N));
    LUT[i]=(unsigned short int)(Vsin*(4096.0/3.0));
  }
  
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
GPIOA->MODER |=GPIO_MODER_MODER4;
GPIOA->MODER &= ~GPIO_MODER_MODER2;


RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
SYSCFG->CFGR1 |=SYSCFG_CFGR1_TIM6DAC1_DMA_RMP;
/*
SYSCFG->EXTICR[0] |=SYSCFG_EXTICR1_EXTI2_PA;
NVIC->ISER[0] |= (1<<8);
EXTI->IMR |= EXTI_IMR_MR2;
EXTI->RTSR |= EXTI_RTSR_TR2;
*/


RCC->APB1ENR |=RCC_APB1ENR_TIM2EN;
TIM2->ARR =190;
TIM2->CNT =0;
TIM2->CR2 |=TIM_CR2_MMS_1;
RCC->APB1ENR |=RCC_APB1ENR_DACEN;
   
   DAC->CR |=DAC_CR_TEN1; 
   DAC->CR |=DAC_CR_TSEL1_2;
   DAC->CR |=DAC_CR_DMAEN1;
   DAC->CR |=DAC_CR_EN1 ;
   for(int i=0;i<10000;i++);
   
RCC->AHBENR |= RCC_AHBENR_DMA1EN;

DMA1_Channel3->CCR |=DMA_CCR_DIR;
DMA1_Channel3->CCR |=DMA_CCR_MINC;
DMA1_Channel3->CCR |=DMA_CCR_MSIZE_0;
DMA1_Channel3->CCR |=DMA_CCR_PSIZE_0;
DMA1_Channel3->CNDTR =N;
DMA1_Channel3->CPAR =(uint32_t)&(DAC->DHR12R1);
DMA1_Channel3->CMAR =(uint32_t)LUT;
DMA1_Channel3->CCR |=DMA_CCR_EN; 

RCC->AHBENR |= RCC_AHBENR_DMA2EN;                  
DMA2_Channel3->CCR &= ~DMA_CCR_DIR;
DMA2_Channel3->CCR |=DMA_CCR_MINC;
DMA2_Channel3->CCR |=DMA_CCR_MSIZE_0;
DMA2_Channel3->CCR |=DMA_CCR_PSIZE_0;
DMA2_Channel3->CNDTR =N;
DMA2_Channel3->CPAR =(uint32_t)&(DAC->DOR1);
DMA2_Channel3->CMAR =(uint32_t)LUT2_d;
DMA2_Channel3->CCR |=DMA_CCR_EN; 
TIM2->CR1 |=TIM_CR1_CEN;
while(1);
/*for(int i=0;i<N;i++){
  LUT2_d[i]=(unsigned short int)(LUT2_a[i]*(4096.0/3.0));
}
  return 0;*/
}
/*
void EXTI2_TS_IRQHandler(){
EXTI->PR |= EXTI_PR_PR2;
LUT2[cont2++]=DAC->DOR1;
}
*/
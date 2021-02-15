#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>
#define N 100
#define Avg_Slope 4.3*pow(10,-3)
#define V25 1.43
unsigned short int LUT[N];
float val;
float sum=0;
int fine=0;
int main()
{
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  RCC->AHBENR |= RCC_AHBENR_ADC12EN;                 
  RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
  
  EXTI->IMR |=EXTI_IMR_MR0;
  EXTI->RTSR |=EXTI_RTSR_TR0;
  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  TIM6->ARR =37890;
  TIM6->PSC =7200-1;
  TIM6->CR2 |=TIM_CR2_MMS_1;
  
  
  ADC1_2->CCR |=ADC12_CCR_CKMODE_0;
  ADC1_2->CCR |=ADC12_CCR_TSEN;
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i=0;i<10000;i++)
  ADC1->CR &= ~ADC_CR_ADCALDIF;
  ADC1->CR |=ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CFGR |=ADC_CFGR_DMAEN;
  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->CFGR |=ADC_CFGR_EXTEN_0;
  ADC1->CFGR |=ADC_CFGR_EXTSEL;
  ADC1->CFGR &= ~ADC_CFGR_EXTSEL_1;
    
  ADC1->SMPR2 |=ADC_SMPR2_SMP16;
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SQR1 |= ADC_SQR1_SQ1_4;
  
  RCC->AHBENR |=RCC_AHBENR_DMA1EN;
  DMA1_Channel1->CCR |=DMA_CCR_MINC;
  DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
  DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
  DMA1_Channel1->CNDTR=N;
  DMA1_Channel1->CPAR=(uint32_t)&(ADC1->DR);
  DMA1_Channel1->CMAR=(uint32_t)LUT;
  
  DMA1_Channel1->CCR |= DMA_CCR_TCIE;
  NVIC->ISER[0] |= (1<<6);
  NVIC->ISER[0] |= (1<<11);
  
  DMA1_Channel1->CCR |=DMA_CCR_EN;
  
  ADC1->CR |=ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
  ADC1->CR |=ADC_CR_ADSTART;
    
  while(fine==0);
  for(int i=0;i<N;i++){
    val=(float)((V25 + (float)LUT[i]*(3.0/4095.0))/Avg_Slope +25);
    
    sum += (float)(val/N);
  }
  printf("La temperatura media vale: %f \n",sum);
  return 0;
}

void EXTI0_IRQHandler(){ 
 // printf("premo\n");
EXTI->PR |=EXTI_PR_PR0;
TIM6->CNT=0;
TIM6->CR1 |=TIM_CR1_CEN;

}

void DMA1_Channel1_IRQHandler(){
  DMA1->IFCR |= DMA_IFCR_CTCIF1;
  fine=1;
}
#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

#define N 100
#define PI 3.141592654

int cont=0;
int cont2=0;

unsigned short int LUT[N];
unsigned short int LUT2[N];

float Vsin;
float ampiezza=1;
float offset=1.5;

int main()
{
  for(int i=0;i<N;i++){
  Vsin=offset+ ampiezza*sin(2*PI*(float)i/N);
  LUT[i]=(unsigned short int)(Vsin*(4096.0/3.0));
  }
  
  RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
  GPIOA->MODER |= GPIO_MODER_MODER2;
  GPIOA->MODER |=GPIO_MODER_MODER4;
  
  RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
  
  TIM7->ARR =7200;
  TIM7->PSC =10-1;
  TIM7->CR2 |= TIM_CR2_MMS_1;
  
  TIM6->ARR =36360;
  TIM6->PSC =2-1;
  TIM6->CR2 |= TIM_CR2_MMS_1;
  
  RCC->AHBENR |= RCC_AHBENR_ADC12EN;
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i=0;i<1000;i++);
  
  ADC1->CR &= ~ADC_CR_ADCALDIF;
  ADC1->CR |= ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);
  
  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;
  ADC1->CFGR |=ADC_CFGR_EXTSEL;
  ADC1->CFGR &= ~ADC_CFGR_EXTSEL_1;
ADC1->CFGR |=ADC_CFGR_DMAEN;
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SQR1 |= ADC_SQR1_SQ1_1|ADC_SQR1_SQ1_0; 
    ADC1->SMPR1 |=ADC_SMPR1_SMP3;
  
  
  
  
  
 

ADC1->CR |= ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
  ADC1->CR |= ADC_CR_ADSTART;
  
  
  
  RCC->APB1ENR |=RCC_APB1ENR_DACEN;
  DAC->CR |= DAC_CR_TEN1;
  DAC->CR |=DAC_CR_TSEL1_1;
  DAC->DHR12R1=LUT[0];
  
  DAC->CR |=DAC_CR_DMAEN1;
   DAC->CR |=DAC_CR_EN1;
  for(int i=0;i<1000;i++);
  RCC->AHBENR |=RCC_AHBENR_DMA1EN; 
  RCC->AHBENR |=RCC_AHBENR_DMA2EN;
  
  DMA1_Channel1->CCR |=DMA_CCR_MINC;  
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;  
  DMA1_Channel1->CCR |=DMA_CCR_MSIZE_0;
  DMA1_Channel1->CCR |=DMA_CCR_MINC;
  DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
  DMA1_Channel1->CNDTR =N;
    DMA1_Channel1->CPAR =(uint32_t) &(ADC1->DR);
    DMA1_Channel1->CMAR =(uint32_t) LUT2;
    
  DMA1_Channel1->CCR |=DMA_CCR_EN;
  
  DMA2_Channel3->CCR |=DMA_CCR_MINC;  
  DMA2_Channel3->CCR |= DMA_CCR_PSIZE_0;  
  DMA2_Channel3->CCR |=DMA_CCR_MSIZE_0;
  DMA2_Channel3->CCR |=DMA_CCR_MINC;
  DMA2_Channel3->CCR |= DMA_CCR_DIR;
   
  DMA2_Channel3->CNDTR =N;
  DMA2_Channel3->CPAR =(uint32_t) &(DAC->DHR12R1);
  DMA2_Channel3->CMAR =(uint32_t) LUT;
    
  DMA2_Channel3->CCR |=DMA_CCR_EN;
  
  TIM6->CR1 |= TIM_CR1_CEN;
  TIM7->CR1 |= TIM_CR1_CEN;
  
  
  while(1);
}

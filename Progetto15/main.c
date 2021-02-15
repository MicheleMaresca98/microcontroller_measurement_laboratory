#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>
#define PI 3.141592654
#define N 100

float Vsin;
float offset=1.5;
float ampiezza=1;

unsigned short int LUT[N];
unsigned short int LUT2[N];

int cont=0;
int cont2=0;

int main()
{ 
    //GENERARE SINUSOIDE
  for(int i=0;i<N;i++){
  Vsin=(float)offset + ampiezza*sin(2*PI*((float)i/N));
  
  LUT[i]=(unsigned short int)(Vsin*(4096.0/3.0));
  
}

  //abilitare clock GPIOA
  RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
  //configurare i pin PA2 e PA4 come analogico poichè PA2 è l'ingresso del canale 3 dell'ADC1 e PA4 è il canale di uscita 1 del DAC1
  GPIOA->MODER |=GPIO_MODER_MODER2;
  GPIOA->MODER |=GPIO_MODER_MODER4;
  
  
  //ABILITARE CLOCK TIMER6 E 7
   RCC->APB1ENR |=RCC_APB1ENR_TIM6EN;
  RCC->APB1ENR |=RCC_APB1ENR_TIM7EN;
  //CONFIGURARE TIMER 7 1ms
  TIM7->ARR =7200;
  TIM7->PSC =10-1;
  TIM7->DIER |=TIM_DIER_UIE;
  NVIC->ISER[1] |= 1<<23;
  
 
  //CONFIGURARE TIMER 6 10 us
  TIM6->ARR =720;
  TIM6->CR2 |=TIM_CR2_MMS_1;
  TIM6->CR1 |=TIM_CR1_OPM;
  
  
  
  //abilitare clock adc1: AHB
  //CONFIGURARE ADC1
  RCC->AHBENR |=RCC_AHBENR_ADC12EN;
  ADC1_2->CCR |=ADC12_CCR_CKMODE_0;
  
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i=0;i<10000;i++);
  ADC1->CR &= ~ADC_CR_ADCALDIF;
  ADC1->CR |=ADC_CR_ADCAL;
  while((ADC1->CR & ADC_CR_ADCAL)==ADC_CR_ADCAL);
  
  ADC1->CFGR &= ~ADC_CFGR_CONT;
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;
  ADC1->CFGR |= ADC_CFGR_EXTSEL;
  ADC1->CFGR &= ~ADC_CFGR_EXTSEL_1;
 
  ADC1->SQR1 &= ~ADC_SQR1_L;
  ADC1->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1;
  ADC1->SMPR1 |=ADC_SMPR1_SMP3;
 

  ADC1->IER |=ADC_IER_EOC;
  NVIC->ISER[0] |= 1<<18;
  
  
  //abilitare clock DAC1  : APB1
  //CONFIGURARE DAC1
  
  RCC->APB1ENR |=RCC_APB1ENR_DACEN;
  DAC->CR |=DAC_CR_TEN1;
  DAC->CR |= DAC_CR_TSEL1;
 
     
  
    ADC1->CR |=ADC_CR_ADEN;
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD); 
  ADC1->CR |= ADC_CR_ADSTART;
  
  DAC->CR |=DAC_CR_EN1;
  for(int i=0;i<1000;i++);

  TIM7->CNT=0;
  TIM7->CR1 |=TIM_CR1_CEN;
  while(cont2<N);
  return 0;
 // while(1);
}

void ADC1_2_IRQHandler(){
  LUT2[cont2++]=ADC1->DR;
}
void TIM7_IRQHandler(){
  TIM7->SR &= ~TIM_SR_UIF;
  TIM6->CNT=0;
  DAC->DHR12R1 =LUT[cont++];
  DAC->SWTRIGR |=DAC_SWTRIGR_SWTRIG1;
  TIM6->CR1 |=TIM_CR1_CEN;
}
  
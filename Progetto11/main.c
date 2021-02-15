#include <stm32f30x.h>
#include <stdio.h>
#define q 3.0/4096.0
float dr;
int main()
{float risultato;
  RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
  GPIOA->MODER |=  GPIO_MODER_MODER0;
  RCC->AHBENR |=RCC_AHBENR_ADC12EN;
  ADC1_2->CCR |=ADC12_CCR_CKMODE_0;
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;
  ADC1->CR |= ADC_CR_ADVREGEN_0;
  for(int i=0;i<1000;i++); //DEVO ASPETTARE 10us
  ADC1->CR |=ADC_CR_ADCAL;
  while((ADC1->CR&ADC_CR_ADCAL)==1);
  
  ADC1->CR |=ADC_CR_ADEN;
  while((ADC1->ISR&ADC_ISR_ADRD)==0);
  ADC1->CFGR &=~ADC_CFGR_CONT;//singola conversione e non continua
  ADC1->SQR1 &=~ADC_SQR1_L;
//i primi 4 bit ovvero quelli di L pari a zero per convertire solo il primo canale
  ADC1->SQR1 =ADC_SQR1_SQ1_0;//nel primo canale metto PA0
  ADC1->SMPR1 |=ADC_SMPR1_SMP1;
  ADC1->CR |=ADC_CR_ADSTART; 
 while(ADC1->ISR&ADC_ISR_EOC==0);
  while(1){ 
   
  dr=ADC1->DR;
  risultato=(float)q*dr;
  printf("Il valore di tensione convertito vale: %f V \n",risultato);
   ADC1->CR |=ADC_CR_ADSTART; 
  }
}

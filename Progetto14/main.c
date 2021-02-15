#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

int codice_in=500;
float tensione_out,tensione_in;
int codice_out;

int main()
{ //ABILITAZIONE PERIFERICHE
  RCC->AHBENR |=RCC_AHBENR_ADC12EN;  
  RCC->APB1ENR |=RCC_APB1ENR_DACEN;
  RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
  
  //PA4 E PA2 IN MODALITA ANALOGICA
  GPIOA->MODER |=GPIO_MODER_MODER4;//CANALE DI USCITA 1 DAC
  GPIOA->MODER |=GPIO_MODER_MODER2;//CANALE DI INGRESSO 3 ADC
  
  DAC->CR |= DAC_CR_EN1;
  DAC->DHR12R1 =codice_in;
  for(int i=0;i<1000;i++);//attesa generazione di tensione
  
  
  ADC1_2->CCR |=ADC12_CCR_CKMODE_0;
  ADC1->CR &=~ADC_CR_ADVREGEN_1;
  ADC1->CR |=ADC_CR_ADVREGEN_0;
  for(int i=0;i<1000;i++);
  ADC1->CR |=ADC_CR_ADCAL;
  while(ADC1->CR&ADC_CR_ADCAL==1);
  ADC1->CR |=ADC_CR_ADEN;
  while(ADC1->ISR&ADC_ISR_ADRD != ADC_ISR_ADRD);
  ADC1->CFGR &=~ADC_CFGR_CONT;
  ADC1->SQR1 &=~ADC_SQR1_L;
  ADC1->SQR1 |=ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_1;
  ADC1->SMPR1 |=ADC_SMPR1_SMP1;  
  
  ADC1->CR |=ADC_CR_ADSTART;
  while(ADC1->ISR&ADC_ISR_EOC != ADC_ISR_EOC);
      
  tensione_out=codice_in*(3.0/4096.0);
  
  printf("DAC\n");
  printf("Ingresso: %d\n",codice_in);
  printf("Uscita: %f V\n\n",tensione_out);
  
  codice_out =ADC1->DR;
  tensione_in=codice_out*(3.0/4096.0);
  
    printf("ADC\n");
  printf("Ingresso: %f\n",tensione_in);
  printf("Uscita: %d V\n\n",codice_out);
  
  //disabilitazione periferiche
  ADC1->CR |=ADC_CR_ADDIS;
  DAC->CR &=~DAC_CR_EN1;
      while(1);
}

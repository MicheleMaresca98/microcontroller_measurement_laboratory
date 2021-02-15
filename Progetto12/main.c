#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

#define Avg_Slope 4.3*pow(10,-3)
#define V25 1.43

float vts,risultato;
int main()
{
  RCC->APB1ENR |=RCC_APB1ENR_TIM3EN;
  
  TIM3->PSC |=14400-1;
  TIM3->ARR |=50000;
  TIM3->CNT =0;
  TIM3->CR2 |=TIM_CR2_MMS_1;//Per impostare trigger on update
  
  RCC->AHBENR |=RCC_AHBENR_ADC12EN;
  ADC1_2->CCR |=ADC12_CCR_CKMODE_0;
  ADC1_2->CCR |=ADC12_CCR_TSEN;
 // ADC1->SMPR1 |=ADC_SMPR1_SMP1;
   ADC1->SMPR2 |=ADC_SMPR2_SMP16 ;
  ADC1->CFGR &=~ADC_CFGR_CONT;//MODALITA SINGOLA PERCHè NON PUOI METTERE MODALITà CONTINUA CON TRIGGER HARDWARE
  ADC1->SQR1 &=~ADC_SQR1_L;
  ADC1->SQR1 |=ADC_SQR1_SQ1_4;//BIT 4 ALTO PER METTERE 16 PERCHè IL SENSORE DI TEMPERATURA è COLLEGATO AL CANALE 16 DELL'ADC
  
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;//FRONTE SALITA
  ADC1->CFGR |=ADC_CFGR_EXTSEL_2;//EXT4 TIM3_TRGO_EVENT
  
  ADC1->CR &=~ADC_CR_ADVREGEN_1;
  ADC1->CR |=ADC_CR_ADVREGEN_0;
  for(int i=0;i<1000;i++);
  ADC1->CR |=ADC_CR_ADCAL;
  while(ADC1->CR&ADC_CR_ADCAL==1);
  
  ADC1->CR |=ADC_CR_ADEN;
  while(ADC1->ISR&ADC_ISR_ADRD==0);
 
  
  TIM3->CR1 |=TIM_CR1_CEN; 
  ADC1->CR |=ADC_CR_ADSTART;//ANCHE SE è HARDWARE TRIGGER L'ADSTART LA PRIMA VOLTA GLIELO DOBBIAMO SEMPRE DARE NOI
  while(1){
while((ADC1->ISR & ADC_ISR_EOC) != ADC_ISR_EOC);//attendo EOC pari a 1
  vts=ADC1->DR*(3.0/4096.0);/*risultato * VDD/2^n
  3.0/4096.0 ci da la discretizzazione, ci da il quanto e moltiplicato per la parola
  codice ci da la lettura di tensione*/
risultato=((V25-vts)/Avg_Slope)+25;
printf("La temperatura vale: %f° \n",risultato);
}
}

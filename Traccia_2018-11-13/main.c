#include <stm32f30x.h>
#include <math.h>
#define N 100

#define V25 1.43
#define AVG_SLOPE 4.3*pow(10,-3)

float VTS;
float media=0;
float deviazione;
float sum=0;
float val;
float temperatura[N];
int cont=0;
int fine=0;
int main()
{RCC->AHBENR |=RCC_AHBENR_ADC12EN;
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
TIM2->ARR =272808000;
//TIM2->DIER |=TIM_DIER_UIE;
//NVIC->ISER[0] |= (1<<28);
TIM2->CR2 |=TIM_CR2_MMS_1;

ADC1_2->CCR |=ADC12_CCR_TSEN;
ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
ADC1->CR &= ~ADC_CR_ADVREGEN_1;
ADC1->CR |=ADC_CR_ADVREGEN_0;
for(int i=0;i<1000;i++);
ADC1->CR &= ~ADC_CR_ADCALDIF;
ADC1->CR |= ADC_CR_ADCAL;
while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL);

ADC1->CR |= ADC_CR_ADEN;
while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);
ADC1->CFGR &= ~ADC_CFGR_CONT;
ADC1->CFGR |=ADC_CFGR_EXTEN_0;
ADC1->CFGR |=ADC_CFGR_EXTSEL;
ADC1->CFGR &= ~ADC_CFGR_EXTSEL_2;
ADC1->SQR1 &= ~ ADC_SQR1_L;
ADC1->SQR1 |=ADC_SQR1_SQ1_4;
ADC1->SMPR2 |=ADC_SMPR2_SMP16;

ADC1->IER |=ADC_IER_EOC;
NVIC->ISER[0] |= (1<<18);
ADC1->CR |=ADC_CR_ADSTART;
TIM2->CR1 |= TIM_CR1_CEN;

  while(fine==0);
  TIM2->CR1 &= ~TIM_CR1_CEN;
  ADC1->CR |= ADC_CR_ADDIS;
  for(int i=0;i<N;i++){
    val=temperatura[i];
    media += val/N;
  }
  for(int i=0;i<N;i++){
    val=temperatura[i];
    sum += (pow((val-media),2))/N;
  }
  deviazione=sqrt(sum);
  return 0;
}
void ADC1_2_IRQHandler(){
  VTS=(float)(ADC1->DR * (3.0/4096.0));
 temperatura[cont++]=(float)(((V25-VTS)/AVG_SLOPE) +25);
 if(cont==N)
   fine=1;
}

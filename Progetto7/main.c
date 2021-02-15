#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>
#define Tck 125*pow(10,-9)

unsigned int i=0;
float t=0;
int n=1;
int cnt=0;

int main()
{RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
//NON HO SETTATO MODER 0 COME INPUT PERCHè LO è DI DEFAULT
//ARR è MASSIMO NON LO SETTO
while(1){
  if((GPIOA->IDR&GPIO_IDR_0)==1){
    while((GPIOA->IDR&GPIO_IDR_0)==1);
    if(i==0){
      i=1;
      TIM2->CR1 |=TIM_CR1_CEN;
      TIM2->CNT=0;
    }else{
      TIM2->CR1 &= ~TIM_CR1_CEN;
      cnt=TIM2->CNT;
      t=(float)cnt*Tck;
      printf("TEMPO MISURATO %d: %f SECONDI\n",n,t);
      n++;
      i=0;
    }
  }
  }
}

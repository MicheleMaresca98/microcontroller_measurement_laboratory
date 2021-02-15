#include <stm32f30x.h>
#include <stdio.h>
float tensione_a=0.789;
unsigned short int tensione_d=(unsigned short int)(0.789*(4096.0/3.0));
float risultato_a;
unsigned short int risultato_d;
int main()
{RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
GPIOA->MODER &= ~GPIO_MODER_MODER0;//ANCHE DI DEFAULT
EXTI->IMR |=EXTI_IMR_MR0;
EXTI->RTSR |=EXTI_RTSR_TR0;
NVIC->ISER[0] |= (1<<6);

RCC->APB1ENR |=RCC_APB1ENR_DACEN;
DAC->CR |=DAC_CR_TEN1;
DAC->CR |=DAC_CR_TSEL1;
DAC->CR |=DAC_CR_EN1;
for(int i=0;i<1000;i++);
  while(1);
}

void EXTI0_IRQHandler(){
EXTI->PR |= EXTI_PR_PR0;
DAC->DHR12R1 = tensione_d;
DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
risultato_d=(unsigned short int)DAC->DOR1;
risultato_a=(float)(risultato_d*(3.0/4096.0));
printf("TENSIONE ANALOGICA: %f V \n",tensione_a);
printf("TENSIONE DIGITALE: %d V \n",tensione_d);
printf("RISULTATO ANALOGICO: %f V \n",risultato_a);
printf("RISULTATO DIGITALE: %d V \n",risultato_d);
}
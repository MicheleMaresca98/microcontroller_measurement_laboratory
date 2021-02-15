#include <stm32f30x.h>
unsigned int i=0;
int main()
{RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
GPIOE->MODER |=GPIO_MODER_MODER8_0|GPIO_MODER_MODER9_0|GPIO_MODER_MODER10_0|GPIO_MODER_MODER11_0|GPIO_MODER_MODER12_0|GPIO_MODER_MODER13_0|GPIO_MODER_MODER14_0|GPIO_MODER_MODER15_0 ;
GPIOA->MODER &=0xFFFFFFFC;
EXTI->IMR |= EXTI_IMR_MR0;
EXTI->RTSR |=EXTI_RTSR_TR0;


NVIC->ISER[0] |= (1<<6);
while(1);
}


void  EXTI0_IRQHandler(){
  EXTI->PR |= EXTI_PR_PR0;
 if(i==0){
GPIOE->ODR |= GPIO_ODR_8 | GPIO_ODR_9| GPIO_ODR_10| GPIO_ODR_11| GPIO_ODR_12| GPIO_ODR_13| GPIO_ODR_14| GPIO_ODR_15;
i=1;
 }else{
   GPIOE->ODR &= ~GPIO_ODR_8;
GPIOE->ODR &= ~GPIO_ODR_9;
GPIOE->ODR &= ~GPIO_ODR_10;
GPIOE->ODR &= ~GPIO_ODR_11;
GPIOE->ODR &= ~GPIO_ODR_12;
GPIOE->ODR &= ~GPIO_ODR_13;
GPIOE->ODR &= ~GPIO_ODR_14;
GPIOE->ODR &= ~GPIO_ODR_15;
i=0;
 }
}


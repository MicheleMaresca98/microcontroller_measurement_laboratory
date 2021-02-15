#include <stm32f30x.h>
int flag=0;
int flag2=0;
int main()
{RCC->AHBENR |=RCC_AHBENR_GPIOEEN;
RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
GPIOA->MODER &= ~GPIO_MODER_MODER0;
GPIOE->MODER |= GPIO_MODER_MODER8_0;

RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
TIM2->ARR =272808000;
TIM2->DIER |= TIM_DIER_UIE;
NVIC->ISER[0] |= (1<<28);

EXTI->IMR |=EXTI_IMR_MR0;
EXTI->RTSR |=EXTI_RTSR_TR0;
NVIC->ISER[0] |= (1<<6);
while(1);
}
void EXTI0_IRQHandler(){
  EXTI->PR |= EXTI_PR_PR0;
  if(flag2==0){
  TIM2->CR1 |= TIM_CR1_CEN;
  flag2=1;
  }else if(flag2==1){
     TIM2->CR1 &= ~TIM_CR1_CEN;
    flag2=0;
  }
}
void TIM2_IRQHandler(){
 TIM2->SR &= ~TIM_SR_UIF;
 if(flag==0){
   GPIOE->ODR |= GPIO_ODR_8;
   flag=1;
 }else if(flag==1){
   GPIOE->ODR &= ~GPIO_ODR_8;
   flag=0;
 }
}
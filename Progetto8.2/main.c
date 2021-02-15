#include <stm32f30x.h>
int main()
{RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
GPIOE->MODER |=GPIO_MODER_MODER8_0|GPIO_MODER_MODER13_0 ;
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
TIM7->ARR =100000;
TIM7->PSC =7199;
TIM7->CNT=0;
TIM7->DIER |=TIM_DIER_UIE;
NVIC->ISER[1] |=(1<<23);

TIM6->ARR =50000;
TIM6->PSC =7199;
TIM6->CNT=0;
TIM6->DIER |=TIM_DIER_UIE;
NVIC->ISER[1] |=(1<<22);

TIM6->CR1 |=TIM_CR1_CEN;
TIM7->CR1 |=TIM_CR1_CEN;
while(1);
}

void TIM7_IRQHandler(){
    TIM7->SR &= ~TIM_SR_UIF;
    if((GPIOE->ODR&GPIO_ODR_8)==0){
    GPIOE->ODR |= GPIO_ODR_8;
    }else{
       GPIOE->ODR &= ~GPIO_ODR_8;
    }
}

void TIM6_DAC_IRQHandler(){
    TIM6->SR &= ~TIM_SR_UIF;
    if((GPIOE->ODR&GPIO_ODR_13)==0){
    GPIOE->ODR |= GPIO_ODR_13;
    }else{
       GPIOE->ODR &= ~GPIO_ODR_13;
    }
}

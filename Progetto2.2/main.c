#include <stm32f30x.h>
int main()
{RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
GPIOE->MODER |=GPIO_MODER_MODER9_0 | GPIO_MODER_MODER8_0;
GPIOE->ODR |= GPIO_ODR_9 | GPIO_ODR_8;
GPIOE->ODR &= ~GPIO_ODR_8;
while(1);
}
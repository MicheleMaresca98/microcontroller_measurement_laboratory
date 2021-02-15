#include <stm32f30x.h>
#include <stdio.h>
#include <math.h>

// valore di tensione da voler generare
#define PI 3.1415926  // pi greco
#define VDDA 3        // massima tensione generata dal DAC è 3V
#define N 100         // dimensione del vettore contenente le parole 
                      // codice da convertire

int cont_cod = 0;



// vettore contenente le parole codice da convertire
unsigned short int LUT[N];  // parole codice da generare
unsigned short int LUT2[N];  // parole codice da conversione
int cont_mis=0;        // contatore dei campioni acquisiti

/*
 * Programma: Si genera una sinusoide numerica a frequenza 10Hz con 100 campioni,
 *              la si acquisisce con un ADC. Il vettore delle 100 parole codice
 *              da convertire in analogico è LUT, il vettore delle 100 parole
 *              codice risultanti dalla conversione in serie DAC-ADC è LUT2.
*/


int main() {
  // preparo l'array
   float offset = 1.5;
  float amp = 1;
  float Vmis;
  for(int i=0; i<N; i++){
    Vmis = offset + amp * sin( 2 * PI * ((float) i / N));
    LUT[i] = (unsigned short int) (Vmis * (4095.0/3.0)); 
  }
  // configuro i pin
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;    // abilito il port A
  GPIOA->MODER |= GPIO_MODER_MODER2;    // abilito il port PA2 in mod. analogica
  GPIOA->MODER |= GPIO_MODER_MODER4;    // abilito il port PA4 in mod. analogica
    
  // configuro Tim7 -> lancia un interrupt ogni mezzo secondo
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;   // abilito il clock per il TIM7
  //TIM7->CR1 &= ~TIM_CR1_CEN;            // disabilito il TIM7
 // TIM7->PSC = 9-1;                      // Tpsc = 125ns <- tempo tra due conteggi
  //TIM7->ARR = 8000;                     // TIM7 si resetta ogni 1ms
  //TIM7->CNT = 0;                        // azzero il contatore
 TIM7->ARR =7200;
  TIM7->PSC =10-1;
    TIM7->DIER |= TIM_DIER_UIE;           // genero interrupt all'update event
  NVIC->ISER[1] = (1<<23);              // abilito NVIC per gestire l'interruzione
  
  // configuro Tim6 -> lancia un interrupt dopo 10us
   RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;   // abilito il clock per il TIM6
 // TIM6->CR1 &= ~TIM_CR1_CEN;            // disabilito il TIM6
 
 // TIM6->PSC = 9-1;                      // Tpsc = 125ns <- tempo tra due conteggi
 // TIM6->ARR = 80;                       // TIM6 si resetta ogni 10us
 TIM6->ARR =720;
  // TIM6->CNT = 0;                        // azzero il contatore
  TIM6->CR2 |= TIM_CR2_MMS_1;           // genero TRGO all'update event
   TIM6->CR1 |= TIM_CR1_OPM;             // modalità non continua
  // configuro l'ADC
   // abilitazioni
  RCC->AHBENR |= RCC_AHBENR_ADC12EN;    // abilito il clock per gli ADC12
  //ADC1->CR &= ~ADC_CR_ADEN;             // disabilito l'ADC1
  
  // setto il clock per l'ADC a 72MHz
  ADC1_2->CCR |= ADC12_CCR_CKMODE_0;
    
  // avvio il regolatore della tensione di riferimento
  ADC1->CR &= ~ADC_CR_ADVREGEN_1;         // abilitazione del regolatore della 
  ADC1->CR |= ADC_CR_ADVREGEN_0;        // tensione di riferimento Vref
  for(int i=0; i<10000; i++);           // aspetto 10us

  // inizio la calibrazione
  ADC1->CR &= ~ADC_CR_ADCALDIF;         // calibrazione in mod single ended
  ADC1->CR |= ADC_CR_ADCAL;             // abilito la calibrazione
  while((ADC1->CR & ADC_CR_ADCAL) == ADC_CR_ADCAL); // aspetto la fine della cal
  
  // configuro il convertitore ADC1
  ADC1->CFGR &= ~ADC_CFGR_CONT;         // setto la modalità single ended
 // ADC1->CFGR &= ~ADC_CFGR_RES;          // risoluzione pari a 12
//  ADC1->CFGR &= ~ADC_CFGR_ALIGN;        // abilito l'allineamento a destra
  ADC1->CFGR |= ADC_CFGR_EXTEN_0;       // ADC si abilita sul f. di salita
  //ADC1->CFGR |= 13<<6;                  // ADC abilitato sul f. di salita di TIM6
  ADC1->CFGR |= ADC_CFGR_EXTSEL;
  ADC1->CFGR &= ~ADC_CFGR_EXTSEL_1;
  ADC1->SQR1 &= ~ADC_SQR1_L;            // setto la lunghezza pari a 1
  //ADC1->SQR1 |= 3<<6;                   // PA2 è connesso ad ADC1_3
 ADC1->SQR1 |= ADC_SQR1_SQ1_0;
 ADC1->SQR1 |= ADC_SQR1_SQ1_1;
  ADC1->SMPR1 |= ADC_SMPR1_SMP3;        // Ts = 601.5 Ck cycles => 
                                        // Tadc = (601.5+12.5)/72MHz = 8.5us
  
  // configuro l'interruzione sul flag EOC
  ADC1->IER |= ADC_IER_EOC;             // segn. dell'interruzione sull'EOC
  NVIC->ISER[0] = 1<<18;                // abilito l'NVIC per l'interrupt su ADC12
  
  // configuro il DAC
  RCC->APB1ENR |= RCC_APB1ENR_DACEN;    // abilitazione del DAC
  DAC->CR |= DAC_CR_TEN1;               // DAC converte quando è triggerato
  DAC->CR |= DAC_CR_TSEL1;              // Il trigger che abilita è via software
    
  // abilito l'ADC
  ADC1->CR |= ADC_CR_ADEN;                              // abilito l'ADC
  while((ADC1->ISR & ADC_ISR_ADRD) != ADC_ISR_ADRD);    // aspetto il ready
  ADC1->CR |= ADC_CR_ADSTART;                           // avvio la conversione
  
  // abilito il DAC
  DAC->CR |= DAC_CR_EN1;        // abilito il DAC1 -> genera segnale su PA4
  for(int i=0; i<1000; i++);    // attendo il Twakeup, max 10us
  
  // abilito il timer 7
  TIM7->CR1 |= TIM_CR1_CEN;
  
  // attendo l'acquisizione di N campioni
  while(cont_mis < N);
  /*
  // disabilito le periferiche
  TIM7->CR1 &= ~TIM_CR1_CEN;
  ADC1->CR |= ADC_CR_ADDIS;
  DAC->CR &= ~DAC_CR_EN1;
  TIM6->CR1 &= ~TIM_CR1_CEN;
    */
  // uscita dal programma
  return 0;
}

void ADC1_2_IRQHandler(){
  // si è verificato la fine di una conversione
  LUT2[cont_mis++] = ADC1->DR;
}

/* -----------------------------------------------------------------------------
 * - Definisco la isr relativa al timer 7 -> è eseguita ogni 500ms
 * -----------------------------------------------------------------------------
*/
void TIM7_IRQHandler(){
  TIM7->SR &= ~TIM_SR_UIF;      // disabilito il bit relativo all'interruzione
  TIM6->CNT = 0;                // azzero il contatore
  DAC->DHR12R1 = LUT[cont_cod++];      // parola codice, ris. a 12 bit
  DAC->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;     // simulo evento di trigger via SW
  TIM6->CR1 |= TIM_CR1_CEN;     // abilito TIM7 -> ADC converte dopo Tsettling
}
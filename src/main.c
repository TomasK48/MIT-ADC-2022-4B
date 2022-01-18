#include "stm8s.h"
#include "milis.h"
#include "spse_stm8.h"
#include "stm8_hd44780.h"

/*#include "delay.h"*/
#include <stdio.h>*/
#include "../lib/uart.c"*/

#define _ISOC99_SOURCE
#define _GNU_SOURCE

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_5
#define LED_HIGH   GPIO_WriteHigh(LED_PORT, LED_PIN)
#define LED_LOW  GPIO_WriteLow(LED_PORT, LED_PIN)
#define LED_TOGG GPIO_WriteReverse(LED_PORT, LED_PIN)

#define BTN_PORT GPIOE
#define BTN_PIN  GPIO_PIN_4
#define BTN_PUSH (GPIO_ReadInputPin(BTN_PORT, BTN_PIN)==RESET) 


void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz

    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    init_uart();
    init_milis();    //spustit časovač milis
    lcd_init();

    // na pinech/vstupech ADC_IN2 (PB2) a ADC_IN3 (PB3) vypneme vstupní buffer
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL4, DISABLE);
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5, DISABLE);
    // při inicializaci volíme frekvenci AD převodníku mezi 1-4MHz při 3.3V
    // mezi 1-6MHz při 5V napájení
    // nastavíme clock pro ADC (16MHz / 4 = 4MHz)
    ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);
    // volíme zarovnání výsledku (typicky vpravo, jen vyjmečně je výhodné vlevo)
    ADC2_AlignConfig(ADC2_ALIGN_RIGHT);
    // nasatvíme multiplexer na některý ze vstupních kanálů
    ADC2_Select_Channel(ADC2_CHANNEL_4);
    // rozběhneme AD převodník
    ADC2_Cmd(ENABLE);
    // počkáme než se AD převodník rozběhne (~7us)
    ADC2_Startup_Wait();
}

void main(void)
{
    char text[32];
    uint32_t voltage = 0;
    uint32_t temperature = 0;
    uint32_t time = 0;
    uint16_t ADCx= 0;
    setup();
    while (1) {

        if (milis() - time > 333 && BTN_PUSH) {
            LED_TOGG; 
            time = milis();
            
            ADCx = ADC_get(ADC2_CHANNEL_4);
            voltage = (uint32_t)3300 * ADCx / 1024;
            //temperature = ((uint32_t)33000 * ADCx - (uint32_t)4000 * 1024) / 1024 /195;
            temperature = ((uint32_t)33000 * ADCx - (uint32_t)4096000 +19968/2) / 19968;
            printf("value %ld \n \r", voltage);
            printf("teplota %ld.%ld °C \n \r", temperature/10,temperature%10);

            lcd_clear();
            lcd_gotoxy(0,1);
            //sprintf(text,"Teplota: ",temperature/10);
            lcd_puts("32");
        }

    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"

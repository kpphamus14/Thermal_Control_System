#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/SysTick.h"
#include "../inc/PLL.h"
#include "../inc/ST7735.h"
#include "../inc/ST7735_PortD.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
#include "TM4C123GH6PM.h"

#define LEDS (*((volatile uint32_t *)0x4000703C))
#define DELAY 0x08
// access PD3-PD0


void Switch_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x00000020;
	while((SYSCTL_PRGPIO_R&0x20) == 0) {};
		GPIO_PORTF_LOCK_R = 0x4C4F434B;
		GPIO_PORTF_CR_R = 0x1F;
		GPIO_PORTF_DIR_R |= 0x00;
		GPIO_PORTF_PUR_R = 0x11;
		GPIO_PORTF_DEN_R |= 0x11;
		GPIO_PORTF_DATA_R = 0;
}
void LED_Init(void) {
	SYSCTL_RCGCGPIO_R |= 0x08;
	while((SYSCTL_PRGPIO_R&0x08)==0){};
	
	GPIO_PORTD_AMSEL_R &= ~0x0F;
	GPIO_PORTD_PCTL_R &= ~0x0000FFFF;
	GPIO_PORTD_DIR_R |= 0x03;
	GPIO_PORTD_AFSEL_R &= ~0x03;
	GPIO_PORTD_DEN_R |= 0x03;
	GPIO_PORTD_DATA_R |= 0x00;
}

void SPI1_Write(unsigned char data)
{
    GPIOF->DATA &= ~(1<<2);       /* Make PF2 Selection line (SS) low */
    while((SSI1->SR & 2) == 0); /* wait untill Tx FIFO is not full */
    SSI1->DR = data;            /* transmit byte over SSI1Tx line */
    while(SSI1->SR & 0x10);     /* wait until transmit complete */
    GPIOF->DATA |= 0x04;        /* keep selection line (PF2) high in idle condition */
}

void SPI1_init(void)
{
    /* Enable clock to SPI1, GPIOD and GPIOF */
	
   	SYSCTL->RCGCSSI |= (1<<1);   /*set clock enabling bit for SPI1 */
    SYSCTL->RCGCGPIO |= (1<<3); /* enable clock to GPIOD for SPI1 */
    SYSCTL->RCGCGPIO |= (1<<5); /* enable clock to GPIOF for slave select */

    /*Initialize PD3 and PD0 for SPI1 alternate function*/
	
    GPIOD->AMSEL &= ~0x09;      /* disable analog functionality RD0 and RD3 */
    GPIOD->DEN |= 0x09;         /* Set RD0 and RD3 as digital pin */
    GPIOD->AFSEL |= 0x09;       /* enable alternate function of RD0 and RD3*/
    GPIOD->PCTL &= ~0x0000F00F; /* assign RD0 and RD3 pins to SPI1 */
    GPIOD->PCTL |= 0x00002002;  /* assign RD0 and RD3 pins to SPI1  */
    
    /* Initialize PF2 as a digital output as a slave select pin */
	
    GPIOF->DEN |= (1<<2);         /* set PF2 pin digital */
    GPIOF->DIR |= (1<<2);         /* set PF2 pin output */
    GPIOF->DATA |= (1<<2);        /* keep SS idle high */

    /* Select SPI1 as a Master, POL = 0, PHA = 0, clock = 4 MHz, 8 bit data */
		
    SSI1->CR1 = 0;          /* disable SPI1 and configure it as a Master */
    SSI1->CC = 0;           /* Enable System clock Option */
    SSI1->CPSR = 4;         /* Select prescaler value of 4 .i.e 16MHz/4 = 4MHz */
    SSI1->CR0 = 0x00007;     /* 4MHz SPI1 clock, SPI mode, 8 bit data */
    SSI1->CR1 |= 2;         /* enable SPI1 */
}

/* This function generates delay in ms */
/* calculations are based on 16MHz system clock frequency */

void Delay_ms(int time_ms)
{
    int i, j;
    for(i = 0 ; i < time_ms; i++)
        for(j = 0; j < 3180; j++)
            {}  /* excute NOP for 1ms */
}


int main(void) {
	Switch_Init();
	LED_Init();

	unsigned char val1 = 'A';
	unsigned char val2 = 'B';
	
	SPI1_init();
	
	while(1){
		if(GPIO_PORTF_DATA_R == (0x10|GPIO_PORTF_DATA_R)){
			GPIO_PORTD_DATA_R |= 0x00;//
		}
		else{
			SPI1_Write(val1); /* write a character */
      Delay_ms(1000);
		}
		
		if(GPIO_PORTF_DATA_R == (0x01|GPIO_PORTF_DATA_R)){
			GPIO_PORTD_DATA_R |= 0x00;
			}
			else{
			SPI1_Write(val2); /* write a character */
      Delay_ms(1000);	

		}
	}			
	}
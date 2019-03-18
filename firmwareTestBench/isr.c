#include <generated/csr.h>
#include <irq.h>
#include <uart.h>
#include "globalvar.h"

extern void periodic_isr(void);

void isr(void);
void button_isr(void);
void isr(void)
{
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & (1 << UART_INTERRUPT))
    {
        uart_isr();
    }
    if(irqs & (1 << 4))
    {
        button_isr();
    }
}
void button_isr(void){
    unsigned int pen = buttons_ev_pending_read();
    switch(pen){
		case 2:
            if(dir==2){dir=2;}else{dir=0;}
			break;
		case 4:
            if(dir==0){dir=0;}else{dir=2;}
			break;
		case 8:
            if(dir==1){dir=1;}else{dir=3;}
			break;
		case 1:
            if(dir==3){dir=3;}else{dir=1;}
			break;
		case 16:
            switch(gameState){
                case GAME:
                    gameState=PAUSE;
                break;
                case PAUSE:
                    gameState=GAME;
                break;
                case LOST:
                    gameState=GAME;
                break;
            }
		break;
		default:
            dir=dir;
		break;
	}
	buttons_ev_pending_write(0xff);	
	buttons_ev_enable_write(0xff);
}

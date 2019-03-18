#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include "./logic_game.c"

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();
	buttons_ev_enable_write(0xff);
	buttons_ev_pending_write(0xff);
	irq_setmask(irq_getmask() | (1<< 4));
    
    lcd_initialize();
    initConfig();

	while(1) {
		game();
	}
	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include "./logic_game.c"

typedef struct {
	volatile uint32_t write;
	volatile uint32_t write2;

} led_t;


led_t  *ledf  = (led_t *)   0xe0005800;


static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = readchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					putsnonl("\x08 \x08");
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				putsnonl("\n");
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				putsnonl(c);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}

	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("RUNTIME>");
}

static void help(void)
{
	puts("Available commands:");
	puts("help        - this command");
	puts("1           - reboot CPU");
    puts("2           - Game");
    puts("3           - Fill Screen");
    puts("4           - Test");
    puts("5           - idle");
    puts("6           - Initialize");
}

static void reboot(void)
{
	asm("call r0");
}

static void console_service(void)
{
	char *str;
	char *token;

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "1") == 0)
		reboot();
    else if(strcmp(token, "2") == 0)
        {
            game();
        }
    else if(strcmp(token, "3") == 0)
        {
            fillScreen();
        }
    else if(strcmp(token, "4") == 0)
        {
            test();
        }
    else if(strcmp(token, "5") == 0)
        {
            idle();
        }
    else if(strcmp(token, "6") == 0)
        {
            lcd_initialize();
        }
prompt();
}


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

	puts("\nGameSnake - CPU testing software built "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}



#include "uart/uart.h"

#define BAUD 115200 // 9600 14400 19200 38400 57600 115200

void hook_early_init(void)
{
	uart_init(UART_BAUD_SELECT(BAUD, F_CPU));
}

void hook_late_init(void)
{

}

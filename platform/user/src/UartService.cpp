#include <zephyr/drivers/gpio.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <UartService.h>
#include "libuser.h"
#include "mcu_api.h"
#include <platform/CHIPDeviceLayer.h>
#include <platform/PlatformManager.h>
#include <platform/internal/DeviceNetworkInfo.h>
#include <string.h>

// #include "types.h"
// #include "parking_message.h"
// #include "handle_message.h"


using namespace ::chip;
using namespace ::chip::DeviceLayer;


/* change this to any other UART peripheral if desired */


// create queue buffer to store data from uart



/* queue to store data_buffer_t */
// extern k_msgq queue_data_rx;

// K_MSGQ_DEFINE(data_buffer_t, MSG_SIZE, 10, 4);

const struct device *uart1 = DEVICE_DT_GET(DT_NODELABEL(uart1));


/* receive buffer used in UART ISR callback */
unsigned char rx_buf[MSG_SIZE];
uint8_t rx_buf_pos;

// res_flag_t res[8];

// int T_BDT_chechsum = 0;
/*
 * Print a null-terminated string character by character to the UART interface
 */
void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart1, buf[i]);
	}
}

void debug_uart(char *buf, int len)
{

	for (int i = 0; i < len; i++) {
		uart_poll_out(uart1, buf[i]);
	}
}

void printUartHexToHexString(const uint8_t * buf, uint16_t bufLen)
{
	char hexBuf[3];
	for (int i = 0; i < bufLen; i++)
	{
		snprintf(hexBuf, sizeof(hexBuf), "%02X", buf[i]);
		print_uart(hexBuf);
	}
}


/*
 * Read characters from UART until FIFO empty. Afterwards push the
 * data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(uart1)) {
		return;
	}

	if (!uart_irq_rx_ready(uart1)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uart1, &c, 1) == 1) {
		uart_receive_input(c);
		/* else: characters beyond buffer size are dropped */
	}

}

int uart_init()
{
	/* initialize the device */
	if (!device_is_ready(uart1)) {
		printk("UART device %s is not ready\n", uart1->name);
		return -1;
	}
    /* register callback */
    uint8_t ret = uart_irq_callback_set(uart1, serial_cb);
	if(ret != 0){
		printk("UART device %s callback set failed\n", uart1->name);
		return -1;
	}
	// /* configure the device */
	// ret = uart_configure(uart1, &uart_cfg);

	// print_uart("UART device ready\n");
    /* Enable rx interrupts */
    uart_irq_rx_enable(uart1);


    return 0;
}

void uart_send(uint8_t *buf, uint16_t len)
{
	for (int i = 0; i < len; i++)
	{
		uart_poll_out(uart1, buf[i]);
	}
}
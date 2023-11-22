#include <stdio.h>

#include "sen44_uart.h"
#include "sensirion_common.h"
#include "sensirion_uart_hal.h"

int init(void);
int reset(void);
int start(void);
void sleep(int);
int stop(void);

int init(void)
{
	return sensirion_uart_hal_init();
}

int reset()
{
	return sen44_device_reset();
}

int start()
{
	return sen44_start_measurement();
}

void sleep(int ms)
{
	sensirion_uart_hal_sleep_usec((uint32_t)ms * (uint32_t)1000);
}

int measure(int* masscon_pm1p0, int* masscon_pm2p5, int* masscon_pm4p0, int* masscon_pm10p0, float* voc_index, float* ambient_humidity, float* ambient_temp)
{
	int16_t error = sen44_read_measured_mass_concentration_and_ambient_values(
		(uint16_t*)masscon_pm1p0, (uint16_t*)masscon_pm2p5, (uint16_t*)masscon_pm4p0, (uint16_t*)masscon_pm10p0,
		voc_index, ambient_humidity, ambient_temp
	);

	return (int)error;
}

int get_serial_number(char serial_number[32])
{
	int16_t error = sen44_get_serial_number((unsigned char)serial_number, (uint8_t)32);
	return (int)error;
}

int get_version(int* firm_maj, int* firm_min, bool* debug, int* hard_maj, int* hard_min, int* pro_maj, int* pro_min)
{
	int16_t error = sen44_get_version(firm_maj, firm_min, debug, hard_maj, hard_min, pro_maj, pro_min);
	return (int)error;
}

int stop()
{
	return sen44_stop_measurement();
}

#include <stdio.h>

#include "sen44_uart.h"
#include "sensirion_common.h"
#include "sensirion_uart_hal.h"

int init(void);
int reset(void);
int start(void);
void sleep(int);
int stop(void);

int main(void)
{
	printf("Hello, World!\n");
	init();
	reset();
	start();
	sleep(5000);
	stop();
	printf("Done.\n");
}

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

// TODO: direct?
int measure(int* masscon_pm1p0, int* masscon_pm2p5, int* masscon_pm4p0, int* masscon_pm10p0, float* voc_index, float* ambient_humidity, float* ambient_temp)
{
	uint16_t _masscon_pm1p0;
	uint16_t _masscon_pm2p5;
	uint16_t _masscon_pm4p0;
	uint16_t _masscon_pm10p0;
	float _voc_index;
	float _ambient_humidity;
	float _ambient_temp;
	int16_t error = sen44_read_measured_mass_concentration_and_ambient_values(
		&_masscon_pm1p0, &_masscon_pm2p5, &_masscon_pm4p0, &_masscon_pm10p0,
		&_voc_index, &_ambient_humidity, &_ambient_temp
	);

	*masscon_pm1p0 = _masscon_pm1p0;
	*masscon_pm2p5 = _masscon_pm2p5;
	*masscon_pm4p0 = _masscon_pm4p0;
	*masscon_pm10p0 = _masscon_pm10p0;
	*voc_index = _voc_index;
	*ambient_humidity = _ambient_humidity;
	*ambient_temp = _ambient_temp;

	return (int)error;
}

int stop()
{
	return sen44_stop_measurement();
}
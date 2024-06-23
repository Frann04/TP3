#ifndef SENSOR_H_
#define SENSOR_H_

#define F_CPU 16000000UL
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define DHT11_PIN PINC0


void sensor_init();
uint8_t sensor_medir();
uint8_t sensor_leer (uint8_t *tempE,uint8_t *tempD, uint8_t *humE, uint8_t *humD);




#endif /* SENSOR_H_ */
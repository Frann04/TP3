/*
 * IncFile1.h
 *
 * Created: 8/6/2024 21:07:10
 *  Author: Usuario
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#define F_CPU 16000000UL
#define Prescaler 1024
#define F_int 0.5
#include <avr/io.h>
#include <avr/interrupt.h>

void timer_init();



#endif /* TIMER_H_ */
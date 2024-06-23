/*
 * TIMER.c
 *
 * Created: 8/6/2024 21:06:01
 *  Author: Usuario
 */ 

#include "timer.h"

void timer_init(){
	
	// Configurar el Timer1 en modo CTC (WGM12 = 1)
	TCCR1B |= (1 << WGM12);

	// Configurar el prescaler a 1024 (CS12 = 1, CS10 = 1)
	TCCR1B |= (1 << CS12) | (1 << CS10);
	
	
	OCR1A = (F_CPU/(Prescaler*F_int)) - 1; //Calcula el valor de comparacion dependiendo de los valores predefinidos
	
	// Habilitar la interrupción de comparación A del Timer1
	TIMSK1 |= (1 << OCIE1A);

	// Habilitar interrupciones globales
	sei();
}

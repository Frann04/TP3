/*
* CFile1.c
*
* Created: 07/6/24 11:02:18 PM
*  Author: Frank
*/
#include "sensor.h"

uint8_t paridad;	// Varible para guardar la paridad
uint8_t temp[2];	// Variable para guardar el valor de la temperatura
uint8_t hum[2];		// Variable para guardar el valor de la humedad

void sensor_init()
{
	DDRC |= (1<<PORTC0);	// Configuramos como salida el pin C0
	PORTC &= ~(1<<PORTC0);	// Forzamos un estado bajo en la linea de datos
	_delay_ms(25);			// Esperamos 25ms > 18ms
	PORTC |= (1<<PORTC0);	// Volvemos al estado alto en la linea de datos.
	DDRC &= ~(1<<PORTC0);	// Configuramos como entrada para que el sensor nos envie datos

	// Esperamos respuesta 
	
	while(PINC&(1<<PINC0));			//Espera que el PINC0 este en alto
	while((PINC&(1<<PINC0))==0);	//Espera que el PINC0 este en bajo
	while(PINC&(1<<PINC0));			//Espera que el PINC0 este en alto
}

uint8_t sensor_medir()
{
	uint8_t i, cant, data = 0;
	for(i=0;i<8;i++){				// itera por cada uno de los 8 bits del byte
		cant=0;
		while((PINC&(1<<PINC0))==0);
		while(PINC&(1<<PINC0)){		// Cuenta el tiempo que está en alto
			_delay_us(1);
			cant++;
		}
		if(cant>40) data = ((data<<1) | 1);	// Si es mayor a 40us es un bit 1 y lo agrega a la derecha
		else data = (data<<1);				// Si es menor de 40us es un bit 0 y lo agrega a la derecha
	}
	return data;
}

uint8_t sensor_leer (uint8_t *tempE,uint8_t *tempD, uint8_t *humE, uint8_t *humD)
{
	cli();
	uint8_t paridadComp = 0;
	sensor_init();				// Enviamos señal de inicio y esperamos respuesta
	hum[0] = sensor_medir();    //Humedad entero
	hum[1] = sensor_medir();    //Humedad decimal
	temp[0] = sensor_medir();    //Temp entero
	temp[1] = sensor_medir();    //Temp decimal
	paridad = sensor_medir();    //Paridad
	paridadComp = temp[0] + temp[1] + hum[0] + hum[1];	// Realizamos la suma para chequear
	sei();
	if (paridadComp == paridad){	// Chequeamos paridad
		*tempE = temp[0];
		*tempD = temp[1];
		*humE = hum [0];
		*humD = hum [1];
		return 1;			// Devolverá 1 si no hubo error de paridad
	}
	else{
		return 0;			// Devolverá 0 si  hubo error de paridad
	}
}
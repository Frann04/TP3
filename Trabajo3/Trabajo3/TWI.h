/*
 * TWI.h
 *
 * Created: 21/09/2017 04:32:03 p.m.
 *  Author: Marlon
 */ 
 

#ifndef TWI_H_
#define TWI_H_
		
	#include <avr/io.h>
	#include <stdbool.h>
	
	///// TIW //////
	#define	TWI_W			0x00 //escritura en el bus
	#define	TWI_R			0x01 //operacion de lectura
	#define TWI_ACK			0x01 //indica que se debe enviar un ACK
	#define TWI_NACK		0x00 //indica que se debe encviar un NACK
	 
	#define TWI_START		0x08 //codigo de envio correcto
	#define TWI_RESTART		0x10 //codigo de reinicio correcto
	#define TWI_WT_SLA_ACK	0x18 //codigo de estado que indica que se ha enviado la direccion del esclavo con bit de escritura y se recibio un ack
	#define TWI_MT_DATA_ACK	0x28 //se envio un byte de datos en modo maestro-transmisor y se recibio un ack
	#define TWI_RD_SLA_ACK	0x40 //se envio la direccion del escalbo con bit de escritura y ack

	#define RTC_slaveAddr	0b01101000
	#define RTC_segAddr		0x00


	void TWI_Init();
	bool TWI_startCond();
	bool TWI_restrtCond();
	void TWI_stopCond();
	bool TWI_write(uint8_t);
	uint8_t TWI_read(uint8_t);
	bool TWI_sendAdrr(uint8_t, uint8_t);
	uint8_t decToBcd(uint8_t );
	uint8_t bcdToDec(uint8_t);
	void RTC_setTime(uint8_t , uint8_t , uint8_t , uint8_t, uint8_t , uint8_t);
	void RTC_getTime(uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
	

#endif /* TWI_H_ */
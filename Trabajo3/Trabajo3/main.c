/*
 * Trabajo3.c
 *
 * Created: 07/6/24 11:01:59 PM
 * Author : Frank
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "serialPort.h"
#include "sensor.h"
#include "timer.h"
#include "TWI.h"

#define BR9600 (0x67)	// 0x67=103 configura BAUDRATE=9600@16MHz
char msg1[] = "REGISTRADOR DE TEMPERATURA Y HUMEDAD\n\r";
char msg2[] = "Si desea detener o reanudar el registro presione la tecla 'S' \n\r";
char msgPausar[] = "Registro pausado. Para reanudar presione 'S'. \n\r";
char msgReanudar [] = "Registro reanudado. Para pausar presione 'S'. \n\r\r";
char msg [150];

//comunicación con la ISR
volatile char RX_Buffer=0;

volatile uint8_t flag_UART = 1;
volatile uint8_t flag_TIMER = 0;
uint8_t imprimir = 0;
uint8_t humE, humD;
uint8_t tempE, tempD;
uint8_t paridad;
uint8_t year, month, date, hour, minute, second;

void parseDate(const char* date, uint8_t* day, uint8_t* month, uint8_t* year);
void parseTime(const char* time, uint8_t* hour, uint8_t* minute, uint8_t* second);

int main(void){
	timer_init();
    SerialPort_Init(BR9600); 			// Inicializo formato 8N1 y BAUDRATE = 9600bps
    SerialPort_TX_Enable();				// Activo el Transmisor del Puerto Serie
    SerialPort_RX_Enable();				// Activo el Receptor del Puerto Serie
	SerialPort_RX_Interrupt_Enable();	// Activo Interrupción de recepcion.
	TWI_Init();							// Inicializmos el RTC


	
	 uint8_t year, month, day, hour, minute, second;
	 
	 //Obtiene la fecha y hora del sistema y la convierte.
	 parseDate(__DATE__, &day, &month, &year);
	 parseTime(__TIME__, &hour, &minute, &second);

	 SerialPort_Send_String(msg1);		 // Envío el mensaje principal
	 SerialPort_Send_String(msg2);		 // Envío el mensaje de Menu
	 
	 
	 // Le enviamos la fech y hora actual al RTC
	 RTC_setTime(decToBcd(year), decToBcd(month), decToBcd(day), decToBcd(hour), decToBcd(minute), decToBcd(second));	
	 sei();								// Activo la mascara global de interrupciones (Bit I del SREG en 1)
   
    while(1)
    {
		
			if (flag_TIMER){							
				
				paridad = sensor_leer(&tempE, &tempD, &humE, &humD);	
				
				RTC_getTime(&year, &month, &date, &hour, &minute, &second);
				
				flag_TIMER=0;
				imprimir = 1;
			}
			if (flag_UART && imprimir){
				
				if(paridad == 1){
					sprintf(msg, "TEMP:%02d.%02d °C HUM:%02d.%02d %%RH \rFECHA: %02d/%02d/%2d HORA: %02d:%02d:%02d \r\r",tempE, tempD, humE, humD, date, month, year, hour, minute, second);

					SerialPort_Send_String(msg);
				}
				
				else{
					sprintf(msg, "Error de transmision. \r\r");

					SerialPort_Send_String(msg);
					}
				imprimir=0;
			}
			
	    }
	    
}

// Rutina de Servicio de Interrupcion por Timer1
ISR(TIMER1_COMPA_vect){

	flag_TIMER=1;

}


// Rutina de Servicio de Interrupción de Byte Recibido
ISR(USART_RX_vect) {
    RX_Buffer = UDR0; // La lectura de UDR0 borra el flag RXC

    if (RX_Buffer == 's' || RX_Buffer == 'S') { // Comprobar si el carácter recibido es 's' o 'S'
        if (!(flag_UART)) {
            flag_UART = 1;
			SerialPort_Send_String(msgReanudar); // Comenzar la transmisión si aún no se había iniciado
        } else {
            flag_UART = 0;
			SerialPort_Send_String(msgPausar); // Pausar la transmisión si ya se había iniciado
        }
    }
}

// Función para convertir una cadena "MMM DD YYYY" a valores numéricos
void parseDate(const char* date, uint8_t* day, uint8_t* month, uint8_t* year) {
	const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
	char monthStr[4];
	strncpy(monthStr, date, 3);
	monthStr[3] = '\0';
	const char* pos = strstr(months, monthStr);
	*month = (pos - months) / 3 + 1;
	*day = atoi(date + 4);
	*year = atoi(date + 7) - 2000; // Para obtener el año en formato de 2 dígitos
}

// Función para convertir una cadena "HH:MM:SS" a valores numéricos
void parseTime(const char* time, uint8_t* hour, uint8_t* minute, uint8_t* second) {
	*hour = atoi(time);
	*minute = atoi(time + 3);
	*second = atoi(time + 6);
}
/*
 * TWI.c
 *
 * Created: 21/09/2017 04:31:53 p.m.
 *  Author: Marlon
 */

 #include "TWI.h"
 
 void TWI_Init(){

	/////// Setting frequency ///////
	/*
		F = 	CPUclk/(16 + (2*TWBR*Prescaler))
		F =		20000/(16 + 2*42*1)
		F =		200kHz
	*/

	//Prescaler
	TWSR &=~ (1<<TWPS0);
	TWSR &=~ (1<<TWPS1);

	//factor divisor
	TWBR = 17;

	/////// TWI POWER ON ///////
	PRR	 &=~ (1<<PRTWI);

 }

 bool TWI_startCond(){
	TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN)); //Enviar condición de INICIO



	while(!(TWCR & (1<<TWINT))); //Espere a que se establezca el indicador TWINT. Esto indica que la condición de INICIO ha sido transmitida  


	if ((TWSR & 0xF8) == TWI_START) //Verifique el valor del registro de estado TWI. Enmascarar bits de preescalador. Si el estado es diferente de INICIO, vaya a ERROR
		return false;

     return true;
 }
 
 

 bool TWI_restrtCond(){ //lo mismo que el anterior modulo solo que se envia al registro Reset
   TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));

   while(!(TWCR & (1<<TWINT)));

   if ((TWSR & 0xF8) == TWI_RESTART)
	  return false;

   return true;
 }



 void TWI_stopCond(){
	TWCR |= ((1<<TWINT) | (1<<TWSTO) | (1<<TWEN)); //Transmitir condición de PARADA
 }

 bool TWI_sendAdrr(uint8_t adrr, uint8_t action){
	
	uint8_t cmp = 0;  //variable de comparacion.
	adrr = (adrr << 1 ); //direccion del esclavo, se desplaza 1 a la izquierda para el bit R/W

	if (action == TWI_W){  //si la accion es de escritura
		adrr &=~ 1;        //se asegura que el bit menos significativo sea 0
		cmp = TWI_WT_SLA_ACK;  //establece el codigo esperado para que el esclavo reconocio la direccion
	}
	else{                         //si la accion es de lectura
		adrr |= 1;
		cmp = TWI_RD_SLA_ACK;
	}

	TWDR = adrr;  //carga la direccion en el registro
	TWCR = ((1<<TWINT) | (1<<TWEN)); //inicia la transmision

	while(!(TWCR & (1<<TWINT))); //espera que se establezca la direccion

	if ((TWSR & 0xF8) == cmp)
		return false;
	 
	return true;
 }

 bool TWI_write(uint8_t data2write){
	
	bool ret = true;

  

	TWDR = data2write; // Cargar el dato a enviar en el registro TWDR
	TWCR = ((1<<TWINT) | (1<<TWEN));
	
	while(!(TWCR & (1<<TWINT)));
	
	if ((TWSR & 0xF8) == TWI_MT_DATA_ACK)
		ret = false;
	
	return ret;
 }

 uint8_t TWI_read(uint8_t ACK_NACK){
	
	TWCR = ((1 << TWINT) | (1 << TWEN) | (ACK_NACK << TWEA));

	while(!(TWCR & (1<<TWINT)));
	return TWDR;
 }
 
 
 uint8_t decToBcd(uint8_t val) {
	 return ((val / 10 * 16) + (val % 10));
 }

void RTC_setTime(uint8_t year, uint8_t month, uint8_t date, uint8_t hour, uint8_t minute, uint8_t second) {
	while(TWI_startCond());  // Send start condition
	while(TWI_sendAdrr(RTC_slaveAddr, TWI_W)); // Send slave address with write

	while(TWI_write(RTC_segAddr));  // Write starting register address

	while(TWI_write(second));  // Write seconds
	while(TWI_write(minute));  // Write minutes
	while(TWI_write(hour));    // Write hours
	while(TWI_write(1));       // Write day (1: Monday, 2: Tuesday, etc.)
	while(TWI_write(date));    // Write date
	while(TWI_write(month));   // Write month
	while(TWI_write(year));    // Write year

	TWI_stopCond();  // Send stop condition
}
 
 uint8_t bcdToDec(uint8_t val) {
	 return ((val / 16 * 10) + (val % 16));
 }

 
void RTC_getTime(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *hour, uint8_t *minute, uint8_t *second){
	
	while(TWI_startCond());  // Send start condition
	while(TWI_sendAdrr(RTC_slaveAddr, TWI_W)); // Send RTC slave address with write
	while(TWI_write(RTC_segAddr)); // Write starting register address
	
	while(TWI_restrtCond()); // Send restart condition
	
	while(TWI_sendAdrr(RTC_slaveAddr, TWI_R)); // Send RTC slave address with read
	
	*second = bcdToDec(TWI_read(TWI_ACK)); // Read seconds and send ACK
	*minute = bcdToDec(TWI_read(TWI_ACK)); // Read minutes and send ACK
	*hour = bcdToDec(TWI_read(TWI_ACK));   // Read hours and send ACK
	TWI_read(TWI_ACK);                    // Read day and send ACK (not used)
	*date = bcdToDec(TWI_read(TWI_ACK));   // Read date and send ACK
	*month = bcdToDec(TWI_read(TWI_ACK));  // Read month and send ACK
	*year = bcdToDec(TWI_read(TWI_NACK));  // Read year and send NACK
	
	TWI_stopCond(); // Send stop condition
}

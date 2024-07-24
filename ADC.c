#include "ADC.h"

void initADC(){
	
	ADMUX = 0;
	DIDR0 |= (1 <<ADC0D);	//desabilitar el A0 como digital
	DIDR0 |= (1<<ADC1D);     //desabilitar el A1 como digital


	//JUSTIFICACION IZQUIERDA
	ADMUX |= (1<<ADLAR);
	//REFERENCIA INTERNA
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1 << REFS1);
	// HABILITAR INTERRUPCION
	ADCSRA |= (1<<ADIE);
	//PRESCALER 128
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);  //Habilitando prescaler de 16M/128 frecuencia = 125Khz
	//HABILITAR ADC
	ADCSRA |= (1<<ADEN);
}

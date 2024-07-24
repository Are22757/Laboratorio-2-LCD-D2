#include "LCD8bits.h"


void Lcd_Init8bits()
{
	PORTB &= ~(1<<PORTB0);   //RS = 0, se le indica que es modo comando
	PORTB &= ~(1<<PORTB1);   //E = 0
	_delay_ms(20);     //Pequeños delay que indica el fabricante del LCD
	inicio(0x30);     //Comando que se repite 3 veces, que indica el fabricante de la LCD
	_delay_ms(5);
	inicio(0x30);
	_delay_ms(5);
	inicio(0x30);
	_delay_ms(10);

	inicio(0x38);  //Comando que indica el fabricante del LCD, usando la matriz de 5X8
	inicio(0x0C);  //Comando que indica el fabricante del LCD, display encendido
	inicio(0x01);  //Comando que indica el fabricante del LCD, Limpiar LCD
	inicio(0x06);  //Comando que indica el fabricante del LCD, comenzar a almacenar en DDRAM
}

void inicio(char a)
{
	PORTB &= ~(1<<PORTB0);  //RS = 0, se le indica que es modo comando
	dato_a_mostrar(a);
	PORTB |= (1<<PORTB1);    // E = 1, se le indica que envie los datos
	_delay_ms(1);
	PORTB &= ~(1<<PORTB1);   // E = 0,  se le indica que se cierra el envio de datos
	_delay_ms(1);
}


void dato_a_mostrar(char a)
{
	if(a & 1)				//0000 0001, el  a&1 es una operación bitwise, si el primer bit menos signfiativo es 1, la condición es verdadera
	PORTB |= (1<<3);
	else
	PORTB &= ~(1<<3);
	
	if(a & 2)			   //0000 0010, ahora verifica el segundo bit menos significativo, y si es 1, la condición es verdadera
	PORTB |= (1<<2);
	else
	PORTB &= ~(1<<2);
	
	if(a & 4)				//0000 0100
	PORTD |= (1<<PORTD2);
	else
	PORTD &= ~(1<<PORTD2);
	
	if(a & 8)				//0000 1000
	PORTD |= (1<<PORTD3);
	else
	PORTD &= ~(1<<PORTD3);
	
	if(a & 16)			    //0001 0000
	PORTD |= (1<<PORTD4);
	else
	PORTD &= ~(1<<PORTD4);

	if(a & 32)			    //0010 0000
	PORTD |= (1<<PORTD5);
	else
	PORTD &= ~(1<<PORTD5);
	
	if(a & 64)			   //0100 0000
	PORTD |= (1<<PORTD6);
	else
	PORTD &= ~(1<<PORTD6);
	
	if(a & 128)			   //1000 0000
	PORTD |= (1<<PORTD7);
	else
	PORTD &= ~(1<<PORTD7);
}





void Lcd_Clear()    //Limpia la pantalla LCD
{
	inicio(1);  //Comando dado por el fabricante de la LCD
}



void Lcd_Set_Cursor(char a, char b)
{
	if(a == 0)
	inicio(0x80 + b);  //Posicionarse en la linea 1 y se suma la columna
	
	else if(a == 1)
	inicio(0xC0 + b);  //Posicionarse en la linea 2  y se suma la columna
}


void Lcd_Write_Char(char data)
{
	PORTB |= (1<<PORTB0);        // RS = 1, se le indica que esta en modo caracter
	dato_a_mostrar(data);             //Escribir el caracter
	PORTB |= (1<<PORTB1);       // E = 1, enviar el dato
	_delay_ms(1);
	PORTB &= ~(1<<PORTB1);    // E = 0
	_delay_ms(1);
}


void Lcd_Write_String(char *a) //Usando punteros, ya que, estos almacenan por ejemplo *dato = hola\0, al final siempre llevan un caracter nulo
{
	int i;
	for(i=0; a[i]!='\0'; i++)   //Se recorre todo el puntero, hasta que el valor sea nulo
	Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right() //Activa el corrimiento hacia la derecha
{
	inicio(0x1C);    //Comando brindado por el fabricante de la LCD
}

void Lcd_Shift_Left()    //Activa el corrimiento hacia la izquierda
{
	inicio(0x18);  //Comando brindado por el fabricante de la LCD
}

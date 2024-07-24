//******************************************************************************
// Universidad Del Valle De Guatemala
// Curso: Electrónica Digital 2
// Autor: Lisbeth Ivelisse Arévalo Girón
// Carné: 22757
// Proyecto: Laboratorio 2 LCD
// Hardware: Atmega238p
//******************************************************************************

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "LCD8bits/LCD8bits.h"
#include "ADC/ADC.h"

// Configuración UART
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

// Variables globales
uint8_t adcValue1 = 0, adcValue2 = 0, adcChannel = 0;
char voltageStr1[8], voltageStr2[8], counterStr[4] = {'0', '0', '0', '0'};
volatile uint8_t counter = 0;
volatile uint8_t lcdUpdateFlag = 0;  // Indicador para actualizar la LCD
volatile uint8_t adcUpdateFlag = 0;  // Indicador para actualizar valores ADC

// Valores anteriores para la comparación
uint8_t lastAdcValue1 = 255, lastAdcValue2 = 255, lastCounter = 255;

// Prototipos de funciones
void setup(void);
void convertVoltage(char *str, uint8_t value);
void sendUART(char data);
void sendStringUART(char* str);
void updateString(char *str, int value);
void conditionalLCDUpdate(void);
void conditionalUARTSend(void);

// Configuración inicial del sistema
void setup(void) {
    cli();  // Deshabilitar interrupciones globales
    DDRD = 0xFF;  // Configurar Puerto D como salida
    DDRB = 0xFF;  // Configurar Puerto B como salida
    DDRC = 0x00;  // Configurar Puerto C como entrada
    
    // Configuración UART
    UBRR0H = (BRC >> 8);  // Configurar baud rate alto
    UBRR0L = BRC;         // Configurar baud rate bajo
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0); // Habilitar transmisión, recepción e interrupción RX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Configurar formato: 8 bits de datos, 1 bit de parada
    
    Lcd_Init8bits();  // Inicializar LCD
    _delay_ms(50);    // Esperar para asegurar correcta inicialización de la LCD
    Lcd_Clear();      // Limpiar pantalla LCD
    Lcd_Set_Cursor(0, 1);
    Lcd_Write_String("S1");  // Etiqueta Sensor 1
    Lcd_Set_Cursor(0, 7);
    Lcd_Write_String("S2");  // Etiqueta Sensor 2
    Lcd_Set_Cursor(0, 13);
    Lcd_Write_String("S3");  // Etiqueta Sensor 3
    initADC();  // Inicializar ADC
    ADCSRA |= (1 << ADSC);  // Iniciar la primera conversión ADC

    sei();  // Habilitar interrupciones globales
}

// Convertir valor ADC a voltaje y actualizar cadena de caracteres
void convertVoltage(char *str, uint8_t value) {
    float voltage = (value * 5.0) / 255.0;
    uint16_t int_part = (uint16_t)voltage;
    uint16_t dec_part = (uint16_t)((voltage - int_part) * 100);  // Dos decimales

    if (int_part < 10) {
        str[0] = '0' + int_part;
        str[1] = '.';
        str[2] = '0' + (dec_part / 10);
        str[3] = '0' + (dec_part % 10);
        str[4] = 'V';
        str[5] = '\0';
    } else {
        str[0] = '0' + (int_part / 10);
        str[1] = '0' + (int_part % 10);
        str[2] = '.';
        str[3] = '0' + (dec_part / 10);
        str[4] = '0' + (dec_part % 10);
        str[5] = 'V';
        str[6] = '\0';
    }
}

// Actualizar una cadena con un valor de 3 dígitos
void updateString(char *str, int value) {
    str[0] = '0' + (value / 100);
    str[1] = '0' + ((value / 10) % 10);
    str[2] = '0' + (value % 10);
    str[3] = '\0';
}

// Enviar un dato por UART
void sendUART(char data) {
    while (!(UCSR0A & (1 << UDRE0)));  // Esperar a que el buffer esté vacío
    UDR0 = data;  // Enviar dato
}

// Enviar una cadena de caracteres por UART
void sendStringUART(char* str) {
    while(*str) {
        sendUART(*str++);
    }
}

// Actualizar la pantalla LCD si los valores han cambiado
void conditionalLCDUpdate(void) {
    if (adcValue1 != lastAdcValue1) {
        convertVoltage(voltageStr1, adcValue1);
        Lcd_Set_Cursor(1, 1);
        Lcd_Write_String(voltageStr1);
        lastAdcValue1 = adcValue1;
    }
    if (adcValue2 != lastAdcValue2) {
        convertVoltage(voltageStr2, adcValue2);
        Lcd_Set_Cursor(1, 7);
        Lcd_Write_String(voltageStr2);
        lastAdcValue2 = adcValue2;
    }
    if (counter != lastCounter) {
        updateString(counterStr, counter);
        Lcd_Set_Cursor(1, 13);
        Lcd_Write_String(counterStr);
        lastCounter = counter;
    }
    lcdUpdateFlag = 0;
}

// Enviar datos por UART si los valores han cambiado
void conditionalUARTSend(void) {
    if (adcValue1 != lastAdcValue1 || adcValue2 != lastAdcValue2 || counter != lastCounter) {
        sendStringUART("S1 ");
        sendStringUART(voltageStr1);
        sendStringUART(" S2 ");
        sendStringUART(voltageStr2);
        sendStringUART(" S3 ");
        sendStringUART(counterStr);
        sendUART('\n');
        
        lastAdcValue1 = adcValue1;  // Actualizar valores anteriores
        lastAdcValue2 = adcValue2;
        lastCounter = counter;
    }
}

// Función principal
int main(void) {
    setup();  // Configuración inicial del sistema
    
    while (1) {
        if (lcdUpdateFlag) {
            conditionalLCDUpdate();  // Actualizar la pantalla LCD si es necesario
        }
        
        conditionalUARTSend();  // Enviar datos por UART si es necesario

        _delay_ms(100);  // Esperar 100ms
    }
}

// Interrupción del ADC
ISR(ADC_vect) {
    if (adcChannel == 0) {
        ADMUX &= ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0)); // Seleccionar canal ADC0
        adcValue1 = ADCH;  // Leer valor alto del ADC
        adcChannel = 1;  // Cambiar a canal 1
    } else {
        ADMUX = (ADMUX & ~((1 << MUX2) | (1 << MUX1) | (1 << MUX0))) | (1 << MUX0); // Seleccionar canal ADC1
        adcValue2 = ADCH;  // Leer valor alto del ADC
        adcChannel = 0;  // Cambiar a canal 0
    }
    ADCSRA |= (1 << ADSC);  // Iniciar la próxima conversión ADC
    lcdUpdateFlag = 1;  // Indicar que se debe actualizar la LCD
}

// Interrupción UART para recibir datos
ISR(USART_RX_vect) {
    char received = UDR0;  // Leer dato recibido
    if (received == 'a' && counter < 255) {
        counter++;  // Incrementar contador si es menor a 255
    } else if (received == 'b' && counter > 0) {
        counter--;  // Decrementar contador si es mayor a 0
    }
    lcdUpdateFlag = 1;  // Indicar que se debe actualizar la LCD
}

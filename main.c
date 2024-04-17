/*
******************************************************************************
Universidad del Valle de Guatemala
Programación de Microcrontroladores
Proyecto: Lab 4
Archivo: main.asm
Hardware: ATMEGA328p
Author : Moises Rosales
******************************************************************************
 */ 
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
volatile uint8_t CONTADOR = 0;
volatile uint8_t CONTADOR_ADC = 0; //CONTADOR ADC
volatile uint8_t DISPLAY = 0; //VALOR DISPLAY 2
volatile uint8_t DISPLAY1 = 0; //VALOR DISPLAY 1
volatile uint8_t DISPLAY2 = 0; //VALOR DISPLAY 2




#define BOTON1   PB0
#define BOTON2   PB1

uint8_t LISTA[] = {0x7B, 0x09, 0b0111001, 0b0101111, 0x2D, 0x3B, 0b1111110, 0x71, 0b1111111, 0x73, 0x77, 0x1F, 0x2E, 0x5D, 0x6F, 0b11111111};
//uint8_t LISTA[] = {0x7E, 0x7E };


//INDICAMOS LOS PINES DE ENTRADA Y SALIDA
void unit_pines(){
	// Configurar PB0 y PB1 como entradas con pull-up
	DDRB &= ~((1 << DDB0) | (1 << DDB1));
	PORTB |= (1 << PB0) | (1 << PB1);
	
	
	
	// Configurar  salida
	DDRD = 0xFF;
	DDRB = 0b00111100;
	DDRC = 0b00111111;
	
	// Habilitar interrupciones por cambio de estado en PCINT0 y PCINT1
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);
	sei(); // Habilitar interrupciones globales

}

void salida_led() {
	
	PORTD = 0;
	
	// Mostrar el valor del contador en los LEDs
	PORTD = CONTADOR;
}


void incremento() {
	CONTADOR++;
	if (CONTADOR == 256) {
		CONTADOR = 0;
	}
}

void decremento() {
	if (CONTADOR > 0) {
		CONTADOR--;
		} else {
		CONTADOR = 255;
	}
}



// Rutina de interrupción
ISR(PCINT0_vect) {	
	// Comprobar si el botón de incremento (PC4) está presionado
	if ((PINB & (1 << BOTON1))) {
	//	_delay_ms(100);
		decremento();
	}

	// Comprobar si el botón de decremento (PC5) está presionado
	if ((PINB & (1 << BOTON2))) {
	//	_delay_ms(100);
		incremento();
	}
}

// Función para inicializar ADC
void init_ADC() {
	// Habilitar ADC y configurar preescalador a 128
	ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	
	// Configurar referencia de voltaje AVCC con capacitor externo en AREF
	ADMUX = 0;
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << ADLAR);
	
	// Configurar pin A6 como entrada analógica
	ADMUX |= (1 << MUX2) | (1 << MUX1); // A6 en ADMUX[5:0]
	
	// Habilitar interrupción de conversión completa del ADC
	ADCSRA |= (1 << ADIE);

	
	// Iniciar primera conversión
	//ADCSRA |= (1 << ADSC);
}


ISR(ADC_vect){
	
	CONTADOR_ADC = ADCH;
	DISPLAY1 = (CONTADOR_ADC & 0b00001111); // LISTA(display1) && 0b00000011
	DISPLAY2 = (CONTADOR_ADC >> 4);
	
	ADCSRA |= (1<<ADIF);
	
}


void Salida_display(){
	/*PORTC |= ((LISTA[DISPLAY1] & 0b10000000)>>6|(PORTC&0b1100000));
	PORTC |= ((LISTA[DISPLAY1] & 0b01000000)>>4);
	PORTC |= ((LISTA[DISPLAY1] & 0b00000100)<<2);
	PORTC |= ((LISTA[DISPLAY1] & 0b00000001)<<2);
	PORTB |= ((LISTA[DISPLAY1] & 0b00000010)<<3|(PORTB&0b11001111));
	PORTB |= ((LISTA[DISPLAY1] & 0b00001000)<<2);*/
	PORTC |= (LISTA[DISPLAY1] & 0b00011111);
	PORTB |= (LISTA[DISPLAY1] & 0b000110000);
}

void Salida_display1(){
	/*PORTC |= ((LISTA[DISPLAY2] & 0b10000000)>>6|(PORTC&0b1100000));
	PORTC |= ((LISTA[DISPLAY2] & 0b01000000)>>4);
	PORTC |= ((LISTA[DISPLAY2] & 0b00000100)<<2);
	PORTC |= ((LISTA[DISPLAY2] & 0b00000001)<<2);
	PORTB |= ((LISTA[DISPLAY2] & 0b00000010)<<3|(PORTB&0b11001111));
	PORTB |= ((LISTA[DISPLAY2] & 0b00001000)<<2);*/
	PORTC |= (LISTA[DISPLAY2] & 0b00011111);
	PORTB |= (LISTA[DISPLAY2] & 0b000110000);
	
}

void comparador(){
	if (CONTADOR_ADC>=CONTADOR)
	{
		PORTC |= (1 << PC5);
		} else{
		PORTC &= ~(1 << PC5);
	}
}

int main(void){
    cli();
	UCSR0B = 0;
	//cli(); // Deshabilitar interrupciones globales
    unit_pines(); // Inicializar pines
	init_ADC();
	
	
    while (1){
	salida_led();
	PORTB |= (1<<PB2);
	PORTB &= ~(1<<PB3);
	PORTC &= 0b11100000;
	PORTB &= 0b11001111;

	Salida_display1();
	_delay_ms(10);
	PORTB |= (1<<PB3);
	PORTB &= ~(1<<PB2);
	PORTC &= 0b11100000;
	PORTB &= 0b11001111;
	Salida_display();
	_delay_ms(10);
	ADCSRA |= (1<<ADSC);
	comparador();
	}

	return 0;
}	
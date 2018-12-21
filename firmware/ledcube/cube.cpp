#include <stdint.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "cube.h"

#define P_CAT1 PORTD
#define DDR_CAT1 DDRD
#define P_CAT2 PORTD
#define DDR_CAT2 DDRD
#define P_CAT3 PORTD
#define DDR_CAT3 DDRD
#define P_CAT4 PORTB
#define DDR_CAT4 DDRB

#define CAT1 PD5
#define CAT2 PD6
#define CAT3 PD7
#define CAT4 PB0

#define P_LATCH PORTB
#define DDR_LATCH DDRB
#define LATCH PB1

#define P_SPI PORTB
#define DDR_SPI DDRB
#define MOSI PB3
#define MISO PB4
#define SCLK PB5
#define SS PB2

volatile uint8_t cube_4::layer_counter = 0;
volatile uint8_t cube_4::pwm_counter = 0;
volatile uint8_t cube_4::buffer[64] = {0};
volatile uint8_t cube_4::pwm_resolution = 0x1F;

void cube_4::_start(){
    /* setup ports and pins */ 
    DDR_CAT1 |= (1 << CAT1);
    DDR_CAT2 |= (1 << CAT2);
    DDR_CAT3 |= (1 << CAT3);
    DDR_CAT4 |= (1 << CAT4);
    DDR_LATCH |= (1 << LATCH);
    DDR_SPI |= (1 << MOSI) | (1 << SCLK) | (1 << SS);
    DDR_SPI &= ~(1<<MISO);

    /* setup spi */
    SPCR = SPSR = 0;
    SPCR |= (1<<SPE) | (1<<MSTR);
    SPSR |= (1<<SPI2X);

    /* setup timer1 */
    TCCR1A = TCCR1B = TCCR1C = 0;
    OCR1A = 650;
    TIMSK1 |= (1 << OCIE1A);
    TCCR1B |= (1 << WGM12) | (1 << CS10);
    TCNT1 = 0;
    sei();
}

void cube_4::start(){
    cube_4::_start();
}
void cube_4::clear(){
    memset((void*)cube_4::buffer,0,64);
}

void set(unsigned int x, unsigned int y, unsigned int z, uint8_t value){

}

void cube_4::set_all(uint8_t value){
    memset((void*)cube_4::buffer,value,64);
}

void cube_4::set_pwm_resolution(uint8_t resolution){
    pwm_resolution = resolution;
    pwm_counter = 0;
}

uint8_t* cube_4::get_buffer(){
    return (uint8_t*)buffer;
}

void cube_4::set_cathode(uint8_t c){
    switch(c){
        case 0: 
            P_CAT1 |= (1 << CAT1);
            P_CAT2 &= ~(1 << CAT2);
            P_CAT3 &= ~(1 << CAT3);
            P_CAT4 &= ~(1 << CAT4);
        break;
        case 1: 
            P_CAT1 &= ~(1 << CAT1);
            P_CAT2 |= (1 << CAT2);
            P_CAT3 &= ~(1 << CAT3);
            P_CAT4 &= ~(1 << CAT4);
        break;
        case 2:
            P_CAT1 &= ~(1 << CAT1);
            P_CAT2 &= ~(1 << CAT2);
            P_CAT3 |= (1 << CAT3);
            P_CAT4 &= ~(1 << CAT4);
        break;
        case 3:
            P_CAT1 &= ~(1 << CAT1);
            P_CAT2 &= ~(1 << CAT2);
            P_CAT3 &= ~(1 << CAT3);
            P_CAT4 |= (1 << CAT4);
        break;
    }
}

void cube_4::timer_isr(){
    P_LATCH &= ~(1<<LATCH);    
    pwm_counter++;
    pwm_counter %= pwm_resolution;
    if(pwm_counter == 0)
    {
        layer_counter++;
        layer_counter %= 4;
    }

    uint16_t layer_word = 0; 
    for(uint8_t i= 0; i<16; i++){
        layer_word |= ((pwm_counter < buffer[layer_counter*16+i]) << i);
    }
    SPCR &= ~(1<<SPIE);
    SPDR = layer_word & 0xFF;
    while(!(SPSR & (1<<SPIF))){}
    SPCR |= (1<<SPIE);
    SPDR = layer_word >> 8;

    set_cathode(layer_counter);
}

 ISR(TIMER1_COMPA_vect,ISR_NOBLOCK){
     cube_4::timer_isr();
 }

 ISR(SPI_STC_vect){
     P_LATCH |= (1<<LATCH);
}

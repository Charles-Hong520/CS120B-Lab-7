/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "io.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

int main(void) {
    DDRC = 0xFF; PORTC = 0x00; //lcd data lines
    DDRD = 0xFf; PORTD = 0x00; //lcd control lines
    LCD_init();
    LCD_DisplayString(1, "Hello World");
    while (1) {

    }
    return 1;
}

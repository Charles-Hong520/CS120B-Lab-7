/*	Author: Charles Hong
 *  Partner(s) Name: N/A
 *	Lab Section: 022
 *	Assignment: Lab #7  Exercise #1
 *	Exercise Description: LCD Count 0-9
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  
 *  Demo Link: https://drive.google.com/file/d/19sz0ts9JuQiC-ydv_e3NXjhmWNwcTddn/view?usp=sharing
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#define A (PINA&0x03)
volatile unsigned char TimerFlag = 0; //TimerISR sets it to 1, programmer sets it to 0
unsigned long _avr_timer_M = 1; //start count from here, down to 0. Default 1ms
unsigned long _avr_timer_cntcurr = 0; //current internal count of 1ms ticks

void TimerOn() {
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1  = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

enum State {start, init, dec, inc} state;
unsigned char i = 0;
unsigned char disp = 0;

void Tick() {

    switch(state) {
        case start:
        state = init;
        disp = 0;
        break;
        case init:
        if(A==0x00 || 
            (((A==0x01) && disp==9) ||
            ((A==0x02) && disp==0))) {
            state = init;
        } else if(A==0x03) {
            state = init;
            disp = 0;
        } else if(A==0x01 && disp>=0 && disp<=8) {
            state = inc;
            disp = disp+1;
        } else {
            state = dec;
            disp = disp-1;
        }
        break;
        case inc:
        if(A==0x00 || (A==0x02 && disp==0)) {
            state = init;
        } else if(A==0x03) {
            state = init;
            disp = 0;
        } else if(A==0x01) {
            state = inc;
            if(i==10 && disp<9) {
                disp = disp + 1;
                i=0;
            }
        } else {
            state = dec;
            disp = disp-1;
        }
        break;
        case dec:
        if(A==0x00 || (A==0x01 && disp==9)) {
            state = init;
        } else if(A==0x03) {
            state = init;
            disp = 0;
        } else if(A==0x02) {
            state = dec;
            if(i==10 && disp>0) {
                disp = disp - 1;
                i=0;
            }
        } else {
            state = inc;
            disp = disp+1;
        }
        break;
        default:
        state = start;
        printf("something wrong in first switch \n"); break;
    }
    switch(state) {
        case inc:
        i++;
        break;
        case dec:
        i++;
        break;
        case start: break;
        case init:
        i=0;
        break;

    }
}
int main(void) {
    DDRA = 0x00; PORTA = 0xFF; //lcd data lines
    DDRC = 0xFF; PORTC = 0x00; //lcd data lines
    DDRD = 0xFF; PORTD = 0x00; //lcd control lines
    disp = 0;
    // unsigned char* hello= "Hello World";
    // LCD_DisplayString(1, hello);
    unsigned char* display = "0";
    TimerSet(100);
    TimerOn();
    LCD_init();
    LCD_ClearScreen();
    i=0;
    state = start;
    PORTA = PINA;

    while (1) {
        display[0]=disp+'0';
        LCD_DisplayString(1,display);
        Tick();
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}

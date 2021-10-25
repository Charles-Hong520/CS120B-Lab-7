/*	Author: Charles Hong
 *  Partner(s) Name: N/A
 *	Lab Section: 022
 *	Assignment: Lab #7  Exercise #2
 *	Exercise Description: 12321 game with lcd score
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  
 *  Demo Link: https://drive.google.com/file/d/1CaO4EKXb2UNgl9IknCXAvmMTj8xrHdFA/view?usp=sharing
 */
#include <avr/io.h>
#include "io.h"
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif
#define A (PINA&0x01)
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
unsigned char* display = "5";
enum State {start, downS, upS, downG, upG,win} state;
unsigned char a[] = {0x01, 0x02, 0x04, 0x02};
unsigned char i = 0, score = 5;
void Tick() {
    if(state!=win) {
        display[0]=score+'0';
        LCD_DisplayString(1,display);
    } else {
        LCD_DisplayString(1,"YOU WIN!");
    }
    switch(state) {
        case start:
        score = 5;
        i=0;
        state = upG;
        break;
        case upG:
        if(A) {
            state = downS;
            
            if(i%2==1) {
                if(score>=9) {
                    state = win;
                } else {
                    score++;
                }
            } else {
                if(score > 0) {
                    score--;
                }
            }
        }
        else {
            state = upG;
            i = (i+1)%4;
        }
        break;
        case downS:
        if(A) state = downS;
        else state = upS;
        break;
        case upS:
        if(A) {
            state = downG;
            i=0;
        }
        else state = upS;
        break;
        case downG:
        if(A) {
            state = downG;
            i = (i+1)%4;
        }
        else state = upG;
        break;
        case win:
        if(A) {
            state = start;
        }
        break;
    }
    PORTB = a[i];
}
int main(void) {
    DDRA = 0x00; PORTA = 0xFF; //lcd data lines
    DDRB = 0xFF; PORTB = 0x00; //lcd data lines
    DDRC = 0xFF; PORTC = 0x00; //lcd data lines
    DDRD = 0xFF; PORTD = 0x00; //lcd control lines
    TimerSet(300);
    TimerOn();
    LCD_init();
    LCD_ClearScreen();
    PORTB=i=0;
    state = start;
    PORTA = PINA;

    while (1) {

        Tick();
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 1;
}

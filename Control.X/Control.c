/*
 * File:   Control.c
 * Author: swimm
 *
 * Created on 20 de octubre de 2021, 02:50 PM
 */
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF         // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
#define _XTAL_FREQ 4000000
#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))

#include <xc.h>
#include <stdio.h>
#include <pic16f887.h>

#include <xc.h>

char con, buffer[];
int frec, rc, tr;

void USART_CONFIG(int frec, tr, rc){
    RCSTAbits.SPEN = 1;
    TXSTAbits.SYNC = 0;
    TXSTAbits.TX9 = 0;
    RCSTAbits.RX9 = 0;
    switch(frec){
        case 4:
            SPBRG = 25;
            TXSTAbits.BRGH = 1;
            BAUDCTLbits.BRG16 = 0;
            break;
        case 8:
            SPBRG = 12;
            TXSTAbits.BRGH = 0;
            BAUDCTLbits.BRG16 = 0;
            break;     
    }
    if(tr==1){
        TXEN = 1;
    }
    else{
        TXEN = 0;
    }
    if(rc == 1){
        RCSTAbits.CREN = 1;
    }
    else{
        RCSTAbits.CREN = 0;
    }
}

void UART_write(unsigned char* word){   //Función que transmite datos
    while (*word != 0){                 //Verifica que el puntero aumente
        TXREG = (*word);                //Envía el caracter que toca de la cadena
        while(!TXSTAbits.TRMT);         //Espera a que se haya enviado el dato
        word++;                         //Aumenta el apuntador para ir al
    }                                   //siguente caracter
    return;
}

//Para el BOSS, el RB0 es izquierda, RB1 para derecha, 
//RB2 para soltar rocas y RB3 para soltar cohete
//Para el runner, RB0 es patear y RB1 agacharse


void __interrupt()isr(void){
    if(RBIF){
        if((RB0 == 0)&&(RB1 == 1)&&(RB2 == 1)&&(RB3 == 1)){
            TXREG = 0x31;
        }
        else if((RB0 == 1)&&(RB1 == 0)&&(RB2 == 1)&&(RB3 == 1)){
            TXREG = 0x32;
        }
        else if((RB0 == 1)&&(RB1 == 1)&&(RB2 == 0)&&(RB3 == 1)){
            TXREG = 0x33;
        }
        else if((RB0 == 1)&&(RB1 == 1)&&(RB2 == 1)&&(RB3 == 0)){
            TXREG = 0x34;
        }
        else if((RB0 == 1)&&(RB1 == 1)&&(RB2 == 1)&&(RB3 == 1)){
            TXREG = 0x30;
        }
        sprintf(buffer, "%d", con);
        //UART_write(con);
        RBIF = 0;
    }
}

void setup(){
    ANSEL = 0x00;
    ANSELH = 0x00;
    TRISA = 0x01;
    TRISB = 0x0F;
    
    OSCCONbits.IRCF = 0B110;
    OSCCONbits.OSTS = 0;
    OSCCONbits.SCS = 1;
    
    OPTION_REGbits.nRBPU = 0;
    INTCONbits.RBIE = 1;
    
    IOCB = 0x0F;
    WPUB = 0x0F;
    
    USART_CONFIG(4, 1, 1);
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    PIE1bits.RCIE = 1;
    RCIF = 0;
    con = 0x00;
}

void main(void) {
    setup();
    while(1){
        
    }
}

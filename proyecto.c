/*
 * File:   proyecto.c
 * Author: Juan Sebastián
 *
 * Created on 15 de diciembre de 2015, 09:29 PM
 */

#include <xc.h>
#include "configuracion.h"
#include <p18f4553.h>
#include <stdio.h>
#define _XTAL_FREQ 20000000
#define factor_pwm 6.24
#include <pwm_2.h>
#include <lcd8e.h>
#include <adconv.h>

int tecla=255,menu=0,tiempo=0;

int leer_tecla_d(void) 
{
    int tecla_teclado;
    //TRISB=0XF0;
    tecla_teclado=255;
    
    PORTB=0X0E;  
    if (PORTBbits.RB4==0)
        tecla_teclado=1;
    if (PORTBbits.RB5==0)
        tecla_teclado=2;
    if (PORTBbits.RB6==0)
        tecla_teclado=3;
    if (PORTBbits.RB7==0)
        tecla_teclado=10;
    
    PORTB=0X0D;
    if (PORTBbits.RB4==0)
        tecla_teclado=4;
    if (PORTBbits.RB5==0)
        tecla_teclado=5;
    if (PORTBbits.RB6==0)
        tecla_teclado=6;
    if (PORTBbits.RB7==0)
        tecla_teclado=11;
    
    PORTB=0X0B;
    if (PORTBbits.RB4==0)
        tecla_teclado=7;
    if (PORTBbits.RB5==0)
        tecla_teclado=8;
    if (PORTBbits.RB6==0)
        tecla_teclado=9;
    if (PORTBbits.RB7==0)
        tecla_teclado=12;
    
    PORTB=0X07;
    if (PORTBbits.RB4==0)
        tecla_teclado=14;
    if (PORTBbits.RB5==0)
        tecla_teclado=0;
    if (PORTBbits.RB6==0)
        tecla_teclado=15;
    if (PORTBbits.RB7==0)
        tecla_teclado=13;
    
    //ANTIRREBOTE
      if (tecla_teclado != 255)
    {
        for (int t=0;t<300;t++)
            __delay_ms(1);
    } 
    
    return tecla_teclado;
    
}
void interrupt isr(void){
    if(TMR0IF==1){
        menu++;
        if(tecla==16){
            tiempo--;
        }
        TMR0=46005;
        TMR0IF=0;
    }
    if(RBIF==1){
        tecla=leer_tecla_d();
        PORTB=PORTB;
        PORTB=0;
        RBIF=0;
    }
}

void main(void) {
    ADCON1=0X0F;
    Config_ADC('R',20,0);
    TRISC=0;
    TRISB=0XF0;
    INTCONbits.TMR0IE=1;
    INTCONbits.TMR0IF=0;
    TMR0=46005;
    T0CONbits.TMR0ON=1;
    T0CONbits.T08BIT=0;
    T0CONbits.T0CS=0;
    T0CONbits.PSA=0;
    T0CONbits.T0PS2=1;
    T0CONbits.T0PS1=1;
    T0CONbits.T0PS0=1;
    INTCON2bits.RBPU=0;
    INTCONbits.RBIE=1;
    INTCONbits.RBIF=0;
    INTCONbits.GIE=1;
    int luzext,pwm,tecla2,i=50,unidades=0,decenas=0,j=0,tiempo1=0,k=0;
    float led=0;
    char linea[16],linea2[16];
    configurar_pwm(reg_pr2 155,pwm_prescaler 16);
    lcd_inicializar();
    while(1){
        duty_cycle_pwm(0);
        PORTB=0;
        lcd_posicion(0,0);
        sprintf(linea,"CONTROL OFF     ");
        lcd_escribir(linea);
        if(menu<=1){
            lcd_posicion(1,0);
            sprintf(linea2,"A AUTO          ");
            lcd_escribir(linea2);
        }
        if(menu>1&&menu<=3){
            lcd_posicion(1,0);
            sprintf(linea2,"B MANUAL        ");
            lcd_escribir(linea2);
        }
        if(menu>3&&menu<=5){
            lcd_posicion(1,0);
            sprintf(linea2,"C TEMP ON       ");
            lcd_escribir(linea2);
            if(menu==5)
                menu=0;
        }
        if(tecla==12){
            T0CONbits.TMR0ON=0;
            tecla=tecla2=255;
            decenas=unidades=0;
            do{
                lcd_posicion(1,0);
                sprintf(linea2,"TEMP ON:%2ds     ",tiempo1);
                lcd_escribir(linea2);
                lcd_posicion(1,9);
                lcd_cursor(BLINKING_ON,CURSOR_OFF);
                __delay_ms(30);
                lcd_cursor(BLINKING_OFF,CURSOR_OFF);
                if(j==0&&tecla>=0&&tecla<=9){
                    unidades=tecla;
                    tiempo1=unidades;
                    tecla=255;
                    j++;
                }
                if(j==1&&unidades<6&&tecla>=0&&tecla<=9){
                    decenas=unidades;
                    unidades=tecla;
                    tiempo1=decenas*10+unidades;
                    tecla=255;
                    j=0;
                }
                if(j==1&&unidades==6&&tecla==0){
                    decenas=unidades;
                    unidades=tecla;
                    tiempo1=decenas*10+unidades;
                    tecla=255;
                    j=0;
                }
                if(tecla==12){
                    tiempo=tiempo1;
                    tiempo1=0;
                    T0CONbits.TMR0ON=1;
                    tecla2=13;
                    tecla=16;
                }
                if(tecla==15){
                    tiempo1=0;
                    T0CONbits.TMR0ON=1;
                    tecla2=13;
                    tecla=255;
                    menu=0;
                }
            }while(tecla2!=13);
        }
        if(tecla==16){
            do{
                lcd_posicion(1,0);
                sprintf(linea2,"TEMP ON:%2ds     ",tiempo);
                lcd_escribir(linea2);
            }while(tiempo!=0);
                tecla=10;
                menu=0;
        }
        if(tecla==10){
            tecla=tecla2=255;
            lcd_borrar();
            do{
                luzext=Valor_ADC(0);
                if(luzext>=3998)
                    led=100;
                else
                    led=(luzext/4095.00)*100;
                pwm=100-led;
                duty_cycle_pwm(pwm);
                lcd_posicion(0,0);
                sprintf(linea,"CONTROL ON      ");
                lcd_escribir(linea);
                if(menu<=1){
                    lcd_posicion(1,0);
                    sprintf(linea2,"D DESACTIVAR    ");
                    lcd_escribir(linea2);
                }
                if(menu>1&&menu<=3){
                    lcd_posicion(1,0);
                    sprintf(linea2,"B MANUAL        ");
                    lcd_escribir(linea2);
                }
                if(menu>3&&menu<=5){
                    lcd_posicion(1,0);
                    sprintf(linea2,"C TEMP OFF      ");
                    lcd_escribir(linea2);
                    if(menu==5)
                        menu=0;
                }
                if(tecla==12){
                    T0CONbits.TMR0ON=0;
                    tecla=255;
                    decenas=unidades=0;
                    do{
                        luzext=Valor_ADC(0);
                        if(luzext>=3998)
                            led=100;
                        else
                            led=(luzext/4095.00)*100;
                        pwm=100-led;
                        duty_cycle_pwm(pwm);
                        lcd_posicion(1,0);
                        sprintf(linea2,"TEMP OFF:%2ds    ",tiempo1);
                        lcd_escribir(linea2);
                        lcd_posicion(1,10);
                        lcd_cursor(BLINKING_ON,CURSOR_OFF);
                        __delay_ms(30);
                        lcd_cursor(BLINKING_OFF,CURSOR_OFF);
                        if(j==0&&tecla>=0&&tecla<=9){
                            unidades=tecla;
                            tiempo1=unidades;
                            tecla=255;
                            j++;
                        }
                        if(j==1&&unidades<6&&tecla>=0&&tecla<=9){
                            decenas=unidades;
                            unidades=tecla;
                            tiempo1=decenas*10+unidades;
                            tecla=255;
                            j=0;
                        }
                        if(j==1&&unidades==6&&tecla==0){
                            decenas=unidades;
                            unidades=tecla;
                            tiempo1=decenas*10+unidades;
                            tecla=255;
                            j=0;
                        }
                        if(tecla==12){
                            tiempo=tiempo1;
                            tiempo1=0;
                            T0CONbits.TMR0ON=1;
                            tecla2=13;
                            tecla=16;
                        }
                        if(tecla==15){
                            tiempo1=0;
                            T0CONbits.TMR0ON=1;
                            tecla2=13;
                            tecla=255;
                        }
                    }while(tecla2!=13);
                    tecla2=255;
                }
                if(tecla==16){
                    do{
                        lcd_posicion(1,0);
                        sprintf(linea2,"TEMP OFF:%2ds    ",tiempo);
                        lcd_escribir(linea2);
                        luzext=Valor_ADC(0);
                        if(luzext>=3998)
                            led=100;
                        else
                            led=(luzext/4095.00)*100;
                        pwm=100-led;
                        duty_cycle_pwm(pwm);
                    }while(tiempo!=0);
                    tecla=255;
                    tecla2=13;
                    menu=0;
                }
                if(tecla==11||tecla==13)
                    tecla2=13;
            }while(tecla2!=13);
        }
        if(tecla==11){
            tecla=tecla2=255;
            lcd_borrar();
            do{
                duty_cycle_pwm(i);
                lcd_posicion(0,0);
                sprintf(linea,"CONTROL MANUAL  ");
                lcd_escribir(linea);
                if(i%10==0&&(int)i/10>k){  
                    lcd_posicion(1,k);
                    sprintf(linea2,"%c",255);
                    lcd_escribir(linea2);
                    k++;
                }                
                if(i%10==0&&(int)i/10<=k&&k<10){
                    lcd_posicion(1,k);
                    sprintf(linea2,"%c",219);
                    lcd_escribir(linea2);
                    k++;
                }                
                lcd_posicion(1,11);
                sprintf(linea2,"%3d%c ",i,37);
                lcd_escribir(linea2);
                if(tecla==14){
                    i=i-5;
                    if(i<0)
                        i=0;
                    k=0;
                    tecla=255;
                }
                if(tecla==15){
                    i=i+5;
                    if(i>100)
                        i=100;
                    k=0;
                    tecla=255;
                }
                if(tecla==13||tecla==10)
                    tecla2=13;
            }while(tecla2!=13);
            menu=0;
        }
    }
}
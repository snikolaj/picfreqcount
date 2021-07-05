/*
 * File:   main.c
 * Author: Stefan
 *
 * Created on 20, ??? 2021, 17:19
 */


#include "mcc_generated_files/mcc.h"

#define _XTAL_FREQ 16000000

#define divNumSize 3

#define DAT PORTAbits.RA0 
#define CLK PORTAbits.RA1
#define EN PORTAbits.RA2
#define RS PORTAbits.RA4

uint8_t callbacks = 0;
unsigned long frequency = 0;

char divNum[divNumSize]; 
uint32_t divisor = 1;

const char sizeHz[] = "Hz";
const char sizeKHz[] = "KHz";
const char sizeMHz[] = "MHz";
uint8_t choice = 1;
inline void customInterrupt(){
    callbacks = callbacks + 1;
}

inline void LCD_command(char command){
  // 4 bit mode

  /*digitalWrite(D7, (command >> 7) & 1);
  digitalWrite(D6, (command >> 6) & 1);
  digitalWrite(D5, (command >> 5) & 1);
  digitalWrite(D4, (command >> 4) & 1);
  digitalWrite(EN, HIGH);
  digitalWrite(EN, LOW);
  delay(2);

  digitalWrite(D7, (command >> 3) & 1);
  digitalWrite(D6, (command >> 2) & 1);
  digitalWrite(D5, (command >> 1) & 1);
  digitalWrite(D4, command & 1);
  digitalWrite(EN, HIGH);
  digitalWrite(EN, LOW);
  delay(2); */

  // 8 bit mode with 74hc595

  for(uint8_t i = 7; i != 0; i--){
    DAT = (command >> i) & 1;
    CLK = HIGH;
    CLK = LOW;
  }
  DAT = (command & 1);
  CLK = HIGH;
  CLK = LOW;

  CLK = HIGH;
  CLK = LOW; // for the RCLK

  EN = HIGH;
  EN = LOW;
  __delay_ms(2);
}

inline void LCD_write(char command){
  RS = HIGH;
  LCD_command(command);
  RS = LOW;
}

inline void LCD_write_string(const char* str){
  int j = 0;
  while(str[j] != '\0'){
    LCD_write(str[j]);
    j++;
  }
}

void main(void) {
    // initialize the device
    SYSTEM_Initialize();
    TMR1_SetInterruptHandler(customInterrupt);
    TMR1_StartTimer();
    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();

    // MCC already sets ANSELA and TRISA to make all pins digital inputs
    TRISAbits.TRISA0 = 0; // set RA0 as output
    TRISAbits.TRISA1 = 0; // set RA1 as output
    TRISAbits.TRISA2 = 0; // set RA2 as output
    TRISAbits.TRISA4 = 0; // set RA4 as output
    
    DAT = LOW;
    CLK = LOW;

    EN = LOW;
    RS = LOW;
    __delay_ms(20);		/* LCD Power ON Initialization time >15ms */
	LCD_command(0x38);	/* Initialization of 16X2 LCD in 8bit mode */
	LCD_command(0x0C);	/* Display ON Cursor OFF */
	LCD_command(0x06);	/* Auto Increment cursor */
	LCD_command(0x01);	/* Clear display */
	LCD_command(0x80);	/* Cursor at home position */
    
    
    
    while (1)
    {
        __delay_ms(1000);
        TMR1_StopTimer();
        frequency = 0xFFFF * callbacks * 8; // callbacks * 16 bit integer max
        frequency += TMR1_ReadTimer() * 8;  // add running timer 
        
        LCD_command(0x01);
        
        // if it's less than 100 we only have 2 digits - an exception
        if(frequency == 0){
            LCD_write(0x30);
            goto end;
        }
        
        if(frequency < 10){
            LCD_write(frequency + 0x30);
            goto end;
        }
        
        if(frequency < 100){
            divNum[1] = frequency % 10;
            frequency = frequency / 10;
            divNum[0] = frequency;
            
            LCD_write(divNum[0] + 0x30);
            LCD_write(divNum[1] + 0x30);
            goto end;
        }
        
        if(frequency > 9999) {
            divisor = 10;
            choice = 2;
        }
        
        if(frequency > 99999) {
            divisor = 100;
            choice = 2;
        }
        
        if(frequency > 999999) {
            divisor = 1000;
            choice = 3;
        }
        
        if(frequency > 9999999) {
            divisor = 10000;
            choice = 3;
        }
        

        frequency = frequency / divisor;
        for(uint8_t i = 0; i < divNumSize; i++){
            divNum[divNumSize - i - 1] = frequency % 10 + 0x30;
            frequency = frequency / 10;
        }
        LCD_write_string(divNum);
        
        
        end:
        
        if(choice == 1){
            LCD_write_string(sizeHz);
        }
        if(choice == 2){
            LCD_write_string(sizeKHz);
        }
        if(choice == 3){
            LCD_write_string(sizeMHz);
        }
        
        TMR1_Reload();
        TMR1_StartTimer();
    }
}



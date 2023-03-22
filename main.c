/* Demo project pingpongtoren + hoogtesensor 
 * 
 * 
 * pinout:  RC2 = receiver input
 *          RC7 = transmitter output
 *          RB6 = pulse lengte output
 *          RB4 = pwm output
 *          RC1 = motor output
 */

#include "mcc_generated_files/mcc.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include "PI.h"
#include "UART.h"
#include "spi1.h"

uint8_t blue = 0x00, green = 0x00, red = 0xFF;

enum states {
    GREEN_UP, RED_DOWN, BLUE_UP, GREEN_DOWN, RED_UP, BLUE_DOWN
};
enum states change_color = GREEN_UP;
uint8_t step = 1;

enum count {
    UP, DOWN
};
enum count direction = UP;
char led_run = 0;
const int NumberOfLEDs = 60;

void Send_LED_Frame(uint8_t intensity, uint8_t blue, uint8_t green, uint8_t red) {
    SPI1_ExchangeByte(0xE0 | intensity);
    SPI1_ExchangeByte(blue);
    SPI1_ExchangeByte(green);
    SPI1_ExchangeByte(red);
}

void Send_LED_StartFrame() {
    for (int i = 0; i < 4; i++) {
        //printf("sendled ok \r\n");
        SPI1_ExchangeByte(0x00);
        //printf("exchange byte ok \r\n");
    }
}

void Send_LED_EndFrame() {
    for (int i = 0; i < 4; i++) {
        SPI1_ExchangeByte(0xFF);
    }
}

void Led_Follow(uint8_t inputhoogte){
    Send_LED_StartFrame();
    float inputhoogtereal = inputhoogte * 60.0 / 185.0;
    if (inputhoogtereal > 20 && inputhoogtereal < 40){
        inputhoogtereal = (inputhoogtereal - 5);
    }
    uint8_t intensity = 0x1F;
    uint8_t last_led_on = (uint8_t)inputhoogtereal;
    if (last_led_on < 1) last_led_on = 1;
    for (char led = 0; led < NumberOfLEDs; led++) {
        if (led <= last_led_on) {
            Send_LED_Frame(intensity, blue, green, red);
        } else {
            Send_LED_Frame(0x00, 0x00, 0x00, 0x00);
        }
        if (led == last_led_on) {
            intensity = 0x0F;
        }
    }
    Send_LED_EndFrame();
}

void Led_program(){
    switch (direction) {
            case UP: if (led_run < NumberOfLEDs - 1) {
                    led_run++;
                } else {
                    direction = DOWN;
                }
                break;
            case DOWN: if (led_run > 0) {
                    led_run--;
                } else {
                    direction = UP;
                }
                break;
        }

        switch (change_color) {
            case GREEN_UP: if (green < 0xFF) {
                    green += step;
                } else {
                    change_color = RED_DOWN;
                }
                break;
            case RED_DOWN: if (red > 0x00) {
                    red -= step;
                } else {
                    change_color = BLUE_UP;
                }
                break;
            case BLUE_UP: if (blue < 0xFF) {
                    blue += step;
                } else {
                    change_color = GREEN_DOWN;
                }
                break;
            case GREEN_DOWN: if (green > 0x00) {
                    green -= step;
                } else {
                    change_color = RED_UP;
                }
                break;
            case RED_UP: if (red < 0xFF) {
                    red += step;
                } else {
                    change_color = BLUE_DOWN;
                }
                break;
            case BLUE_DOWN: if (blue > 0x00) {
                    blue -= step;
                } else {
                    change_color = GREEN_UP;
                }
                break;
        }
}
/*
                         Main application
 */
void main(void) {
    // initialize the device
    SYSTEM_Initialize();

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

    // Initialiseer de hoogtemeting

    TMR0_StartTimer();
    TMR2_StartTimer();
    
    ADC_SelectChannel(Hoogtesensor);
    ADC_StartConversion();
            SPI1_Open(0);

    printf("start");
    while (1) {

        uartHandler();
        // PI moet op een vaste frequentie (elke 33ms) lopen voor de integrator
        if (TMR0_HasOverflowOccured()) {
            PI();
            static uint8_t printCycle = 0; //door static toe te voegen wordt "printCycle" niet elke keer her geinitialiseerd maar behoudt het zijn vorige waarde
            if (printCycle++ > 30) {
                printLogs();
                printCycle = 0;
          }
          Led_Follow(PI_GetSensorHeight());
          Led_program();
        }
         /*     
        Led_Follow(PI_GetSensorHeight());
        Led_program();*/
/*
        switch (direction) {
            case UP: if (led_run < NumberOfLEDs - 1) {
                    led_run++;
                } else {
                    direction = DOWN;
                }
                break;
            case DOWN: if (led_run > 0) {
                    led_run--;
                } else {
                    direction = UP;
                }
                break;
        }

        switch (change_color) {
            case GREEN_UP: if (green < 0xFF) {
                    green += step;
                } else {
                    change_color = RED_DOWN;
                }
                break;
            case RED_DOWN: if (red > 0x00) {
                    red -= step;
                } else {
                    change_color = BLUE_UP;
                }
                break;
            case BLUE_UP: if (blue < 0xFF) {
                    blue += step;
                } else {
                    change_color = GREEN_DOWN;
                }
                break;
            case GREEN_DOWN: if (green > 0x00) {
                    green -= step;
                } else {
                    change_color = RED_UP;
                }
                break;
            case RED_UP: if (red < 0xFF) {
                    red += step;
                } else {
                    change_color = BLUE_DOWN;
                }
                break;
            case BLUE_DOWN: if (blue > 0x00) {
                    blue -= step;
                } else {
                    change_color = GREEN_UP;
                }
                break;
        }*/

        //start frame

        //Send_LED_StartFrame();
 /*       for (char led = 0; led < NumberOfLEDs; led++) {
            if (led <= void PI_GetSensorHeight()) {
                Send_LED_Frame(0x1F, blue, green, red);
            } else {
                Send_LED_Frame(0x00, 0x00, 0x00, 0x00);
            }
        }*/
         //stop frame

        //__delay_ms(50);

        
    }
    
    SPI1_Close();

}

/**
 End of File
 */
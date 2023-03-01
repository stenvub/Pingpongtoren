/*
 * File:   Leds.c
 * Author: Woensdag
 *
 * Created on March 1, 2023, 3:26 PM
 */


#include "mcc_generated_files/mcc.h"
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
    SPI1_Exchange8bit(0xE0 | intensity);
    SPI1_Exchange8bit(blue);
    SPI1_Exchange8bit(green);
    SPI1_Exchange8bit(red);
}

void Send_LED_StartFrame() {
    for (int i = 0; i < 4; i++) {
        SPI1_Exchange8bit(0x00);
    }
}

void Send_LED_EndFrame() {
    for (int i = 0; i < 4; i++) {
        SPI1_Exchange8bit(0xFF);
    }
}

int main(void) {
    
    while (1) {
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

        //start frame
        Send_LED_StartFrame();
        for (char led = 0; led < NumberOfLEDs; led++) {
            if (led <= led_run) {
                Send_LED_Frame(0x1F, blue, green, red);
            } else {
                Send_LED_Frame(0x00, 0x00, 0x00, 0x00);
            }
        }
        //stop frame
        Send_LED_EndFrame();
        __delay_ms(50);
        
    }
}

/**
 End of File
 */

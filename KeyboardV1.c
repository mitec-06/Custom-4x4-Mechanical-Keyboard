#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "tusb_config.h"
#include "tusb.h"
#include "ws2812.pio.h"
#include "hardware/pio.h"

void pin_setup(void);
void scan_matrix_and_report(void);
void led_turn();
void put_pixel(PIO pio, uint sm, uint32_t pixel_grb);
void led_ALL_ON();
void led_ONE_ON();
void LED_BRIGHTNESS();
void without_touch();
uint32_t hsv_to_rgb(int hue);
bool isLedsOff();

bool key_tracking[4][4] = {false};
uint8_t modified = 0;

// define keymap here, declare const uint8_t keymap[4][4] = {...} HID_KEY_A, HID_KEY_B, 
// also define a local 6 byte storage array to track which keys are being sent to the computer 
uint8_t active_keys[6] = {0};

static volatile int led_brightness = 50;

static volatile int adjusted_brightness = 0;

static volatile int hue = 0;


const uint ROW_PIN1 = 4;
const uint ROW_PIN2 = 5;
const uint ROW_PIN3 = 6;
const uint ROW_PIN4 = 7;
const uint COL_PIN1 = 8;
const uint COL_PIN2 = 9;
const uint COL_PIN3 = 10;
const uint COL_PIN4 = 11;

uint row_pins[4] = {ROW_PIN1, ROW_PIN2, ROW_PIN3, ROW_PIN4};

uint col_pins[4] = {COL_PIN1, COL_PIN2, COL_PIN3, COL_PIN4};

const uint8_t key_map[4][4] = {{HID_KEY_A, HID_KEY_B, HID_KEY_C, HID_KEY_D}, {HID_KEY_E, HID_KEY_F, HID_KEY_G, HID_KEY_H}, {HID_KEY_I, HID_KEY_J
, HID_KEY_K, HID_KEY_L}, {HID_KEY_M, HID_KEY_N, HID_KEY_O, HID_KEY_P}};

bool led_map[4][4] = {0};
static volatile bool matrix_changed = false;

const uint ENC_SW = 1;
const uint ENC_A = 2;
const uint ENC_B = 3;

#define LED_COUNT 16

uint32_t leds[LED_COUNT];

// add two helper functions, look through active keys, find the first slot that equals 0, set it to the scancode, and break
// the other helper function is looking through active keys, and finding the slot that equals the scancode, removing it and breaking


PIO pio = pio0;
uint sm = 0;


int main()
{
    stdio_init_all();
    tusb_init();
    uint offset = pio_add_program(pio, &ws2812_program);

    // set the pins as GPIO_PWM
    gpio_init(ENC_A);
    gpio_set_dir(ENC_A, GPIO_IN);
    gpio_pull_up(ENC_A);

    gpio_init(ENC_B);
    gpio_set_dir(ENC_B, GPIO_IN);
    gpio_pull_up(ENC_B);

    // redundant since two slices are already shared, but just to get the concept down
    
    // determines whether the matrix changed or not.
    pin_setup();

    

    ws2812_program_init(pio, sm, offset, 0, 800000, false);

    adjusted_brightness = (int)(led_brightness * 255.0/ 100.0);
    // initial brightness setup

    while (true) {

        // scanning is analyzed first, if the keyboard is in an idle state, without_touch() is called
        // else, led_turn is called, and tries to detect the rotary encoder turn...

        // a hardware delay of 5 seconds is also implemented to make sure that the rotary encoder and LEDs work without any bugs..
        // hue is also incremented here, and matrix_changed is reset to false after running all the functions

        tud_task();
        scan_matrix_and_report(); // where matrix_changed can be assigned false;
        LED_BRIGHTNESS();
        if (!isLedsOff()) without_touch();
        if (isLedsOff()) led_turn();
        sleep_ms(5);

        hue = (hue + 1) % 360;
        matrix_changed = false;
     
    }
}

// The following four functions are debugging functions used earlier to test whether
// the hardware was capable of turning on the LEDs in the first place.
bool isLedsOff(){
    for(int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            if (led_map[i][j]) return true;
        }
    }
    return false;
}

void led_ONE_ON(){
    leds[0] = (255 << 16);
    for (int i = 1; i < LED_COUNT; i++){
        leds[i] = 0x0;
    }

    for (int i = 0; i < LED_COUNT; i++){
        put_pixel(pio, sm, leds[i]);
    }
    sleep_us(100);
}

void led_ALL_ON(){

    for (int i = 0; i < LED_COUNT; i++){
        leds[i] = (adjusted_brightness << 16) | (adjusted_brightness << 8) | (adjusted_brightness << 0);
    }
    for (int i = 0; i < LED_COUNT; i++){
        put_pixel(pio, sm, leds[i]);
    }
    sleep_us(100);
}
void led_turn(){
    for (int i = 0; i < LED_COUNT; i++){
        leds[i] = 0x0;
    }

    for (int r = 0; r < 4; r++){
        for (int c = 0; c < 4; c++){
            int index = (c % 2 == 0) ? 4 * c + r : 4 * c + (3 - r);
            if (led_map[r][c]){
               leds[index] = hsv_to_rgb(hue % 360);
            } 
        }
    }
    
    for (int i = 0; i < LED_COUNT; i++){
        put_pixel(pio, sm, leds[i]);
    }
    sleep_us(100);
}

void without_touch(){
    int offset = 15;

    // Ring system

    // ring1, innermost 4 leds

    leds[5] = hsv_to_rgb(hue % 360);
    leds[6] = hsv_to_rgb(hue % 360);
    leds[9] = hsv_to_rgb(hue % 360);
    leds[10] = hsv_to_rgb(hue % 360);

    // ring 2, next outward 8 leds

    leds[1] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[2] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[4] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[8] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[7] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[11] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[13] = hsv_to_rgb((hue-offset + 360) % 360);
    leds[14] = hsv_to_rgb((hue-offset + 360) % 360);

    // ring 3, the corners of the keyboard

    leds[0] = hsv_to_rgb((hue - (2 * offset) + 360) % 360);
    leds[3] = hsv_to_rgb((hue - (2 * offset) + 360) % 360);
    leds[12] = hsv_to_rgb((hue - (2 * offset) + 360) % 360);
    leds[15] = hsv_to_rgb((hue - (2 * offset) + 360) % 360);

    for (int i = 0; i < LED_COUNT; i++){
        put_pixel(pio, sm, leds[i]);
    }
}

uint32_t hsv_to_rgb(int hue){

// if hue is less than zero, pure red is sent to the leds
if (hue < 0) return (adjusted_brightness << 16) | (0 << 8) | (0 << 0);;
int segment = hue / 60;
int remainder = hue % 60;

int rising = remainder * 255/60;
int falling = (255 - remainder * 255/60);


// based on what segment the hue calculates to be, changes which color byte is losing/gaining value, creating a cycle.
if (segment == 0) return (adjusted_brightness << 16) | ((adjusted_brightness * rising / 255) << 8) | (0 << 0);
else if (segment == 1) return ((adjusted_brightness * falling / 255) << 16) | (adjusted_brightness << 8) | (0 << 0);
else if (segment == 2) return (0 << 16) | (adjusted_brightness << 8) | ((adjusted_brightness * rising / 255) << 0);
else if (segment == 3) return (0 << 16) | ((adjusted_brightness * falling / 255) << 8) | (adjusted_brightness << 0);
else if (segment == 4) return ((adjusted_brightness * rising / 255) << 16) | (0 << 8) | (adjusted_brightness << 0);
else return (adjusted_brightness << 16) | (0 << 8) | ((adjusted_brightness * falling / 255) << 0);
}

void LED_BRIGHTNESS(){
    // will prolly add code for ENC_SW
    // add code..

    static bool prev_enc_a = true;

    bool first_state = gpio_get(ENC_A);

    // make sure that ENC_A isint turned on
    if (prev_enc_a && !first_state){
        // 5 ms sleep for debouncing VERY IMPORTANT, WILL NOT RUN PROPERLY OTHERWISE
        sleep_ms(1);
        // checks one last time to make sure that ENC_A isint on
        if (!gpio_get(ENC_A)){
        // gpio_xor_mask(1 << 25); used for testing but pretty useless tbh
        bool other_state = gpio_get(ENC_B);
        // if A is off but B is ON, then its going CW
        if (other_state){
            if (led_brightness == 0){
            led_brightness = 0;
            }
            else led_brightness--; // CCW
        } else { // if B is OFF as well, then that means that A was the last to go, and its going CW, so led_brightness goes up.
            if (led_brightness == 100) {
            led_brightness = 100;
            }
            else led_brightness++; // CW
        }
        // calculate for adjusted brightness, it is cast into an int and then sent for the led to change its brightness
        adjusted_brightness = (int)(led_brightness * 255.0/ 100.0);
        }
    }

    prev_enc_a = first_state;
    
}

void pin_setup(){
// configure the output/input of each, ex is col output or row input, gpio_init(..)

    for (int i = 0; i < 4; i++){
        gpio_init(row_pins[i]);
        gpio_init(col_pins[i]);
    }
    // then set all the columns to pull up in order make all the signals equal to 1, if there is a 0, then a key is pressed.
    // gpio_pull_up(..)

    for (int i = 0; i < 4; i++){
        gpio_pull_up(col_pins[i]);
    }

    for (int i = 0; i < 4; i++){
        gpio_set_dir(row_pins[i], GPIO_OUT);
        gpio_set_dir(col_pins[i], GPIO_IN);
    }
}



void scan_matrix_and_report(){
    // keep this bool to check whether or not the matrix has been changed or not (add or remove a key)
       
        // Matrix Scanning Algorithm
        for (int r = 0; r < 4; r++){
            gpio_put(row_pins[r], 0);

            sleep_us(3);

            for (int c = 0; c < 4; c++){
                // if columns are pulled down, and its not already in key_tracking, that means its been chosen...
                if (!gpio_get(col_pins[c]) && !key_tracking[r][c]){
                    // Set key tracking to true. 
                    key_tracking[r][c] = true;
                    led_map[r][c] = true;
                    
                    // send the currently active map as scancode
                    uint8_t scancode = key_map[r][c];

                    matrix_changed = true;

                    // keeps a inventory of which keys are currently active, if a spot has been removed, then it inserts the newest code there..
                    for (int i = 0; i < 6; i++){
                        if (active_keys[i] == 0){
                            active_keys[i] = scancode;
                            break;
                        }
                    }

                    
                }
                else if (gpio_get(col_pins[c]) && key_tracking[r][c]) // if column pin has been released AND the key was already in the list of active keys..
                {
                    uint8_t scancode = key_map[r][c];
                    modified = 0;
                    for (int i = 0; i < 6; i++){
                        // find the specific key, and then set it as 0 so that another key can take it...
                        if (active_keys[i] == scancode){
                            active_keys[i] = 0;
                            break;
                        }
                    }

                    key_tracking[r][c] = false;
                    led_map[r][c] = false;
                    matrix_changed = true;
                }
            }

        gpio_put(row_pins[r], 1);
        sleep_us(3);
        }

    // if matrix has been changed, the key bundle is sent to the keyboard and displays yours truly
     if (matrix_changed)
    if (tud_hid_ready()){
        tud_hid_keyboard_report(0, modified, active_keys);
     } 
}


uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen){
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, const uint8_t* buffer, uint16_t reqlen){
    (void) instance;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;
}

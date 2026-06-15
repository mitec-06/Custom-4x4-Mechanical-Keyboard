#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb_config.h"
#include "tusb.h"
// #include "usb_hid_keys.h"


bool key_tracking[4][4] = {false};
uint8_t modified = 0;
// debouncing algo

// define keymap here, declare const uint8_t keymap[4][4] = {...} HID_KEY_A, HID_KEY_B, 
// also define a local 6 byte storage array to track which keys are being sent to the computer 
uint8_t active_keys[6] = {0};


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

// add two helper functions, look through active keys, find the first slot that equals 0, set it to the scancode, and break
// the other helper function is looking through active keys, and finding the slot that equals the scancode, removing it and breaking

// something along the lines of uint8_code code = keymap[r][c]
// add_key_to_report(code)
// then push it to pc with tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, active_keys);

int main()
{
    stdio_init_all();
    tusb_init();
    
    // determines whether the matrix changed or not.
    

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
    
    // put an interupt to save power  gpio_set_irq_enabled_with_callback()
    // set later: gpio_set_irq_enabled_with_callback();

    while (true) {
        // In here, use check conditions and a for loop to check for each column, if a row is 0, then a key is pressed.
        // set it as gpio_put(..) and gpio_set(..) to put everything back.
    
        tud_task();
        bool matrix_changed = false;
        // Matrix Scanning Algorithm
        for (int r = 0; r < 4; r++){
            gpio_put(row_pins[r], 0);

            sleep_us(3);

            for (int c = 0; c < 4; c++){
                if (!gpio_get(col_pins[c]) && !key_tracking[r][c]){
                    // Set key tracking to true. 
                    key_tracking[r][c] = true;
                    

                    // find the key and set modified, and active_keys
                    uint8_t scancode = key_map[r][c];

                    if (scancode == HID_KEY_ALT_LEFT){
                        modified |= KEYBOARD_MODIFIER_LEFTSHIFT;
                    }
                    matrix_changed = true;
                    for (int i = 0; i < 6; i++){
                        if (active_keys[i] == 0){
                            active_keys[i] = scancode;
                            break;
                        }
                    }

                    
                }
                else if (gpio_get(col_pins[c]) && key_tracking[r][c])
                {
                    uint8_t scancode = key_map[r][c];
                    modified = 0;
                    for (int i = 0; i < 6; i++){
                        if (active_keys[i] == scancode){
                            active_keys[i] = 0;
                            break;
                        }
                    }
                    key_tracking[r][c] = false;
                    matrix_changed = true;
                }
        }
        gpio_put(row_pins[r], 1);
        sleep_us(3);
    }

    
     if (matrix_changed)
    if (tud_hid_ready()){
        tud_hid_keyboard_report(0, modified, active_keys);
        matrix_changed = false;} 
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

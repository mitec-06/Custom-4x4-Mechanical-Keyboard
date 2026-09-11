# Custom 4x4 Keyboard PCB Design

This project is a custom 4x4 mechanical keyboard PCB featuring USB HID firmware on RP2040, addressable RGB LEDs (SK6812) driven via PIO state machine, rotary encoder brightness control, and HSV color animation functions.

## Functions

### Core Functions

- 16 keys arranged in a 4 x 4 matrix pattern.

- 6 key rollover - up to 6 simultaneous keypresses registered.

- USB HID keyboard output - It can work as a plug-and-play keyboard on any computer.

- Debouncing key detection - Prevents false or double triggers from a mechanical switch bounce.

- Diode per key - Diodes were included for every key in order to prevent ghosting.

### LED Functions
- Idle RGB ripple animation - Outward ripple effect starting from center rings cycling through the entire HSV color spectrum.

- Keypress-reactive LEDS - Animation switches to keypress mode when any key is pressed, returns to idle afterwards.

- Full HSV color transition - Smooth transition between red, orange, yellow, green, cyan, blue, magenta and back.

- Ring based animation - Three concentric rings with slight phase offset creating a ripple appearance.

- Brightness control - Using a rotary encoder, smooth brightness scaling from 0-100% applied uniformly to all LEDs.

- Brightness preserved across all animation modes, whether its idle or a key is being pressed.

### Encoder Functions 

- Clockwise rotation increases brightness, counterclockwise rotation decreases brightness.

- Hardware debouncing edge detection - Prevents any skips when rotating the encoder.

### Hardware Features 

- 3.3V to 5V level shifting for LED data signal via SN74LV1T34

- Daisy-chained SK6812 MINI-E LEDs driven via RP2040 PIO state machine.

- RP2040 PIO state machine - Dedicated programmable Input/Output handles precise WS2812 timing independent of CPU timing.

- WS2812 protocol followed to ensure correct latch reset timing.

- USB powered - single cable for both HID and power communication.


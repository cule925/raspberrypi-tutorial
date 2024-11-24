#include <stdio.h>
#include <pigpio.h>

#define BUTTON_PIN			27			// Use GPIO 27
#define BUTTON_PRESS_DELAY_US		200000
#define BUTTON_PRESS_MAX_COUNT		5
#define BUTTON_PRESSED			0
#define BUTTON_INTERRUPT_TIMEOUT_MS	200

int pressedCount = 0;

// ISR function on button interrupt (GPIO falling edge)
void button_isr(int gpio, int level, uint32_t tick) {

	printf("Button pressed!\n");
	pressedCount++;


}

// Entrypoint
int main() {

	// Initialize the pigpio library
	if (gpioInitialise() < 0) {
		printf("Failed to initialize pigpio library\n");
		return -1;
	}
	printf("Library pigpio initialized\n");

	// Set GPIO pin as input
	gpioSetMode(BUTTON_PIN, PI_INPUT);

	// Set internal pull-up resistor
	gpioSetPullUpDown(BUTTON_PIN, PI_PUD_UP);

	// Register interrupt function on falling edge
	if (gpioSetISRFunc(BUTTON_PIN, FALLING_EDGE, BUTTON_INTERRUPT_TIMEOUT_MS, button_isr)) {
		printf("Failed to set ISR function!\n");
		gpioSetMode(BUTTON_PIN, PI_INPUT);
		gpioSetPullUpDown(BUTTON_PIN, PI_PUD_OFF);
		gpioTerminate();
		return -1;
	}

	printf("Press the button for this number of times: %d\n", BUTTON_PRESS_MAX_COUNT);

	// Read input from button and set LED on if button was pressed
	while (1) {

		if (pressedCount == BUTTON_PRESS_MAX_COUNT) break;	// If button press was detected
		gpioDelay(BUTTON_PRESS_DELAY_US);			// Delay

	}

	// Unregister interrupt function,
	// set GPIO pin as input and remove any pull-up or pull-down resistors,
	// return the pin to its default state
	gpioSetISRFunc(BUTTON_PIN, FALLING_EDGE, BUTTON_INTERRUPT_TIMEOUT_MS, NULL);
	gpioSetMode(BUTTON_PIN, PI_INPUT);
	gpioSetPullUpDown(BUTTON_PIN, PI_PUD_OFF);

	// Deinitialize the pigpio library
	gpioTerminate();

	printf("Library pigpio deinitialized\n");

	return 0;

}

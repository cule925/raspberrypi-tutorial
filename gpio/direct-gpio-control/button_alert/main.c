#include <stdio.h>
#include <pigpio.h>

#define BUTTON_PIN			27			// Use GPIO 27
#define BUTTON_PRESS_DELAY_US		200000
#define BUTTON_PRESS_MAX_COUNT		5
#define BUTTON_PRESSED			0

int wasPressed = 0;

// Callback function on button alert (GPIO read changed level)
void button_callback(int gpio, int level, uint32_t tick) {

	if (level == BUTTON_PRESSED && wasPressed == 0) {
		printf("Button pressed!\n");
		wasPressed = 1;
	}

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

	// Register callback on read changed level
	if (gpioSetAlertFunc(BUTTON_PIN, button_callback)) {
		printf("Failed to set alert callback function!\n");
		gpioSetMode(BUTTON_PIN, PI_INPUT);
		gpioSetPullUpDown(BUTTON_PIN, PI_PUD_OFF);
		gpioTerminate();
		return -1;
	}

	printf("Press the button for this number of times: %d\n", BUTTON_PRESS_MAX_COUNT);

	// Read input from button and set LED on if button was pressed
	for (int i = 0; i < BUTTON_PRESS_MAX_COUNT; i++) {

		while (1) {

			if (wasPressed == 1) break;				// If button press was detected
			gpioDelay(BUTTON_PRESS_DELAY_US);			// Delay

		}

		gpioDelay(BUTTON_PRESS_DELAY_US);
		wasPressed = 0;

	}

	// Unregister callback,
	// set GPIO pin as input and remove any pull-up or pull-down resistors,
	// return the pin to its default state
	gpioSetAlertFunc(BUTTON_PIN, NULL);
	gpioSetMode(BUTTON_PIN, PI_INPUT);
	gpioSetPullUpDown(BUTTON_PIN, PI_PUD_OFF);

	// Deinitialize the pigpio library
	gpioTerminate();

	printf("Library pigpio deinitialized\n");

	return 0;

}

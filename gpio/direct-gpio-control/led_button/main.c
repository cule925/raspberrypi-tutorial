#include <stdio.h>
#include <pigpio.h>

#define LED_PIN			17			// Use GPIO 17
#define LED_ON			1
#define LED_OFF			0
#define LED_DELAY_US		500000

#define BUTTON_PIN		27			// Use GPIO 27
#define BUTTON_PRESS_DELAY_US	200000
#define BUTTON_PRESS_MAX_COUNT	5
#define BUTTON_PRESSED		0

// Entrypoint
int main() {

	// Initialize the pigpio library
	if (gpioInitialise() < 0) {
		printf("Failed to initialize pigpio library\n");
		return -1;
	}
	printf("Library pigpio initialized\n");

	// Set GPIO pin as output
	gpioSetMode(LED_PIN, PI_OUTPUT);

	// Set GPIO pin as input with internal pull-up
	gpioSetMode(BUTTON_PIN, PI_INPUT);
	gpioSetPullUpDown(BUTTON_PIN, PI_PUD_UP);

        printf("Press the button for this number of times: %d\n", BUTTON_PRESS_MAX_COUNT);

	// Read input from button and set LED ON for a while if button was pressed
	for (int i = 0; i < BUTTON_PRESS_MAX_COUNT; i++) {

		gpioWrite(LED_PIN, LED_OFF);					// Turn the LED off

		while (1) {

			if (gpioRead(BUTTON_PIN) == BUTTON_PRESSED) break;	// Check if button is being pressed
			gpioDelay(BUTTON_PRESS_DELAY_US);			// Delay

		}

		if (i != 0) printf("Button pressed %d times\n", i + 1);
		else printf("Button pressed 1 time\n");
		gpioWrite(LED_PIN, LED_ON);					// Turn the LED on
		gpioDelay(LED_DELAY_US);					// Delay

	}

	// Set GPIO pins as input and remove any pull-up or pull-down resistors,
	// return the pins to their default state
	gpioSetMode(BUTTON_PIN, PI_INPUT);
	gpioSetMode(LED_PIN, PI_INPUT);
	gpioSetPullUpDown(BUTTON_PIN, PI_PUD_OFF);
	gpioSetPullUpDown(LED_PIN, PI_PUD_OFF);

	// Deinitialize the pigpio library
	gpioTerminate();

	printf("Library pigpio deinitialized\n");

	return 0;

}

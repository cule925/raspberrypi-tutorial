#include <stdio.h>
#include <pigpio.h>

#define LED_PIN		17			// Use GPIO 17
#define LED_BLINK_RPT	20
#define LED_ON		1
#define LED_OFF		0
#define LED_DELAY_US	500000

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

	// Blink the LED
	for (int i = 0; i < LED_BLINK_RPT; i++) {

		gpioWrite(LED_PIN, LED_ON);			// Turn the LED on
		gpioDelay(LED_DELAY_US);			// Delay
		gpioWrite(LED_PIN, LED_OFF);			// Turn the LED off
		gpioDelay(LED_DELAY_US);			// Delay

		if ((i + 1) % 5 == 0) printf("Blinked %d times\n", i + 1);

	}

	// Set GPIO pin as input and remove any pull-up or pull-down resistors,
	// return the pin to its default state
	gpioSetMode(LED_PIN, PI_INPUT);
	gpioSetPullUpDown(LED_PIN, PI_PUD_OFF);

	// Deinitialize the pigpio library
	gpioTerminate();

	printf("Library pigpio deinitialized\n");

	return 0;

}

#include <stdio.h>
#include <pigpio.h>

#define LED_PIN		17			// Use GPIO 17
#define LED_DELAY_US	20000

#define PWM_FREQ	1000			// In Hz

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

	// Print default software PWM frequency
	printf("Default software PWM frequency: %d\n", gpioGetPWMfrequency(LED_PIN));

	// Set new software PWM frequency
	gpioSetPWMfrequency(LED_PIN, PWM_FREQ);

	// Print new software PWM frequency
	printf("New software PWM frequency: %d\n", gpioGetPWMfrequency(LED_PIN));

	// Brighten up the LED with software PWM
	for (int i = 0; i < 256; i++) {

		gpioPWM(LED_PIN, i);		// Duty cycle in range from 0 to 255, when 0 then it is off
		gpioDelay(LED_DELAY_US);

	}

	// Dimm the LED with software PWM
	for (int i = 0; i < 256; i++) {

		gpioPWM(LED_PIN, 255 - i);	// Duty cycle in range from 0 to 255, when 0 then it is off
		gpioDelay(LED_DELAY_US);

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

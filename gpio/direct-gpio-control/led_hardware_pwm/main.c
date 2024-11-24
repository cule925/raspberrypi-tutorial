#include <stdio.h>
#include <pigpio.h>

#define LED_PIN		18			// Use GPIO 18 that supports hardware PWM
#define LED_DELAY_US	20000

#define PWM_FREQ	1000			// In Hz
#define DUTY_CYCLE_DIV	200

// Entrypoint
int main() {

	// Duty cycle
	unsigned int dutyCycle;
	unsigned int dutyCycleStep = PI_HW_PWM_RANGE / DUTY_CYCLE_DIV;

	// Initialize the pigpio library
	if (gpioInitialise() < 0) {
		printf("Failed to initialize pigpio library\n");
		return -1;
	}
	printf("Library pigpio initialized\n");

	// Set GPIO pin alternate mode 5 - hardware PWM
	printf("Mode set: %d\n", gpioSetMode(LED_PIN, PI_ALT5));

	// Print new hardware PWM frequency
	printf("Hardware PWM frequency: %d\n", PWM_FREQ);

	// Brighten up the LED with hardware PWM
	dutyCycle = 0;
	while (dutyCycle < PI_HW_PWM_RANGE) {

		gpioHardwarePWM(LED_PIN, PWM_FREQ, dutyCycle);
		dutyCycle += dutyCycleStep;
		gpioDelay(LED_DELAY_US);

	}

	// Dimm the LED with hardware PWM
	do {

		gpioHardwarePWM(LED_PIN, PWM_FREQ, dutyCycle);
		dutyCycle -= dutyCycleStep;
		gpioDelay(LED_DELAY_US);

	} while (dutyCycle != 0);

	// Set GPIO pin as input and remove any pull-up or pull-down resistors,
	// return the pin to its default state
	gpioSetMode(LED_PIN, PI_INPUT);
	gpioSetPullUpDown(LED_PIN, PI_PUD_OFF);

	// Deinitialize the pigpio library
	gpioTerminate();

	printf("Library pigpio deinitialized\n");

	return 0;

}

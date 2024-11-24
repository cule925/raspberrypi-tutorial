#include <stdio.h>
#include <pigpio.h>

#define I2C_BUS			1		// Use bus 1 that uses GPIO 2 (alt mode 0) and GPIO 3 (alt mode 0), they have fixed pull-up resistors
#define I2C_TX_SIZE		2		// Transmit message size
#define SLAVE_ADDRESS		0x22		// The slave device
#define I2C_WRITE_REPEAT	20		// Repeat transactions
#define I2C_WRITE_DELAY_US	1000000		// Delay between each transaction

// I2C handle
int i2c_handle = -1;

// Message that will be transmitted over I2C
uint8_t command_tx[I2C_TX_SIZE] = {0};

// Transmitting data
void start_transactions() {

	uint8_t select[4] = {0, 0, 1, 1}, set[4] = {1, 0, 1, 0};

	// Start the transactions
	for (int i = 0; i < I2C_WRITE_REPEAT; i++) {

		command_tx[0] = select[i % 4];			// Set first byte to be sent
		command_tx[1] = set[i % 4];			// Set second byte to be sent

		printf("Sending data: select [%x], set [%x]\n", command_tx[0], command_tx[1]);

		// Start write transaction
		if (i2cWriteDevice(i2c_handle, (char *) command_tx, I2C_TX_SIZE) < 0) printf("Error in transmitting data\n");

		gpioDelay(I2C_WRITE_DELAY_US);			// Delay

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

	// I2C initialize
	i2c_handle = i2cOpen(I2C_BUS, SLAVE_ADDRESS, 0);
	if (i2c_handle < 0) {
		if (i2c_handle == PI_BAD_I2C_BUS) printf("PI_BAD_I2C_BUS\n");
		else if (i2c_handle == PI_BAD_I2C_ADDR) printf("PI_BAD_I2C_ADDR\n");
		else if (i2c_handle == PI_BAD_FLAGS) printf("PI_BAD_FLAGS\n");
		else if (i2c_handle == PI_NO_HANDLE) printf("PI_NO_HANDLE\n");
		else if (i2c_handle == PI_I2C_OPEN_FAILED) printf("PI_I2C_OPEN_FAILED\n");
		else printf("Undefined error\n");
		printf("Failed to return a handle to the device\n");
		gpioTerminate();
		return -1;
	}

	// Perform I2C transactions
	start_transactions();

	// I2C deinitialize
	i2cClose(i2c_handle);

	// Deinitialize the pigpio library
	gpioTerminate();

	printf("Library pigpio deinitialized\n");

	return 0;

}

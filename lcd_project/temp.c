#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <byteswap.h>

float read_temperature(int file, int TA_REG) {
    int32_t reg32 = i2c_smbus_read_word_data(file, TA_REG);
    if (reg32 < 0) {
        fprintf(stderr, "ERROR: Read failed on i2c bus register %d - %s\n", TA_REG, strerror(errno));
        exit(1);
    }

    // Convert to Celsius
    reg32 = be16toh(reg32);  // Convert from big-endian to host byte order
    float temperature = (float)((reg32 & 0x0FFF) / 16.0);
    if (reg32 & 0x1000) {
        temperature -= 256.0;  // For negative temperatures
    }
    return temperature;
}

#ifndef I2C_FUNCTIONS_H
#define I2C_FUNCTIONS_H7

int init_temp(char *bus, unsigned int address) {
	int file = open(bus, O_RDWR);
    	if (file < 0) {
        	fprintf(stderr, "ERROR: opening %s - %s\n", bus, strerror(errno));
        	exit(1);
    	}

    	if (ioctl(file, I2C_SLAVE, address) == -1) {
        	fprintf(stderr, "ERROR: setting address %d on i2c bus %s with ioctl() - %s", address, bus, strerror(errno));
        	exit(1);
    	}
    	return file;
}

#endif


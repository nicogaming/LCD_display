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

// Function to initialize the buzzer
int init_buzzer(char* device) {
    int file = open(device, O_RDWR);
    if (file < 0) {
        perror("Failed to open the I2C device");
        exit(1);
    }
    return file;
}

// Function to turn the buzzer on
void buzzer_on(int file) {
    unsigned char buf[2] = {0x00, 0x01}; // Assuming register 0x00 controls buzzer state
    if (write(file, buf, sizeof(buf)) != sizeof(buf)) {
        perror("Failed to write to the I2C device buzzer");
        exit(1);
    }
}

// Function to turn the buzzer off
void buzzer_off(int file) {
    unsigned char buf[2] = {0x00, 0x00}; // Assuming register 0x00 controls buzzer state
    if (write(file, buf, sizeof(buf)) != sizeof(buf)) {
        perror("Failed to write to the I2C device buzzer");
        exit(1);
    }
}

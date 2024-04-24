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

// Function to send a command to the LCD
void lcdCommand(int fd, unsigned char command, int cb) {
	unsigned char buf[2] = {cb, command};
	buf[2] = command;
    if(write(fd, buf, sizeof(buf)) != sizeof(buf)) {
        perror("Failed to write to the I2C bus");
    }
    usleep(1000);
}

// Function to send data (character) to the LCD
void lcdData(int fd, unsigned char data) {
    unsigned char buf[2] = {0x40, data};
    if(write(fd, buf, sizeof(buf)) != sizeof(buf)) {
        perror("Failed to write to the I2C bus");
    }
    usleep(1000);
}

int init_lcd_fd(char* device, int lcd_fd, int lcd_address) {
	if((lcd_fd = open(device, O_RDWR)) < 0) {
        	perror("Failed to open the I2C device");
        	return 1;
   	 }

	// Set the I2C slave address
    	if(ioctl(lcd_fd, I2C_SLAVE, lcd_address) < 0) {
        	perror("Failed to set I2C slave address");
        	close(lcd_fd);
        	return 1;
    	}
	return lcd_fd;
}

int init_lcd_bl_fd (char* device, int lcd_bl_fd, int lcd_rgb_address) {
	if ((lcd_bl_fd = open(device, O_RDWR)) < 0) {
    		perror("Failed to open the I2C device");
	    	return 1;
    	}

	if (ioctl(lcd_bl_fd, I2C_SLAVE, lcd_rgb_address) < 0) {
    		perror("Failed to set I2C slave address");
	    	close(lcd_bl_fd);
	    	return 1;
    	}
	return lcd_bl_fd;
}

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

#define MPC9808_BUS     "/dev/i2c-2"
#define MPC9808_ADR     0x18
#define TA_REG          0x05
#define MANID_REG       0x06
#define DEVID_REG       0x07

//defines the adress for the display itself and the rgb for the display
#define LCD_ADDRESS 0x3E
#define LCD_RGB_ADDRESS 0x62
#define I2C_DEV "/dev/i2c-2"

#ifndef I2C_FUNCTIONS_H
#define I2C_FUNCTIONS_H7

//creates a connection for the I2C display and backlight
int lcd_fd;
int lcd_bl_fd;

//initializes communication with a I2C device on a specified I2C bus
int i2c_init(char *bus, unsigned int address) {
    int file = open(bus, O_RDWR);
    if (file < 0) {
        fprintf(stderr, "ERROR: opening %s - %s\n", bus, strerror(errno));
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, address) == -1) {
        fprintf(stderr, "ERROR: setting address %d on i2c bus %s with ioctl() - %s", address, bus, strerror(errno));
        exit(1);
    }
    printf("i2c init\n");
    return file;
}

#endif

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

//reading the temperature from the tempreader
float readTemperature(int file) {
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


int main() {
    int tempFile = i2c_init(MPC9808_BUS, MPC9808_ADR);
    // Open the I2C device
    if((lcd_fd = open(I2C_DEV, O_RDWR)) < 0) {
        perror("Failed to open the I2C device");
        return 1;
    }

    if ((lcd_bl_fd = open(I2C_DEV, O_RDWR)) < 0) {
    	perror("Failed to open the I2C device");
	    return 1;
    }

    // Set the I2C slave address
    if(ioctl(lcd_fd, I2C_SLAVE, LCD_ADDRESS) < 0) {
        perror("Failed to set I2C slave address");
        close(lcd_fd);
        return 1;
    }

    if (ioctl(lcd_bl_fd, I2C_SLAVE, LCD_RGB_ADDRESS) < 0) {
    	perror("Failed to set I2C slave address");
	    close(lcd_bl_fd);
	    return 1;
    }

    lcdCommand(lcd_fd, 0x01, 0x00);

    // Initialize the LCD
    lcdCommand(lcd_fd, 0x28, 0x00); // 2-line mode, 5x8 character font
    lcdCommand(lcd_fd, 0x0d, 0x00);
    lcdCommand(lcd_fd, 0x01, 0x00);
    lcdCommand(lcd_fd, 0x06, 0x00);
    lcdCommand(lcd_fd, 0x02, 0x00);

    //Add backlight
    lcdCommand(lcd_bl_fd, 0x00, 0x0);
    //lcdCommand(lcd_bl_fd, 0x15, 0x8);

    while (1) {
        lcdCommand(lcd_fd, 0x01, 0x00); // Clear display
        float temperature = readTemperature(tempFile);

        // Print a message to the LCD
        char tempStr[20];
        snprintf(tempStr, sizeof(tempStr), "Temp: %.2f", temperature);
        if (temperature < 20) {
            lcdCommand(lcd_bl_fd, 0x01, 0x8);
        } else if (temperature > 24) {
            //changes the backlight to red when the temp is over 24
            lcdCommand(lcd_bl_fd, 0x10, 0x8);
        } else {
            //changes the temp to green when the temp is between 20 and 24
            lcdCommand(lcd_bl_fd, 0x04, 0x8);
        }

        for (int i = 0; tempStr[i] != '\0'; i++) {
            lcdData(lcd_fd, tempStr[i]);
        }
        usleep(500000);
    }

    // Close the I2C connection
    close(lcd_fd);
    close(lcd_bl_fd);

    return 0;
}


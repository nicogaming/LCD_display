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

// CUSTOM INCLUDES
#include "buzzer.h"
#include "lcd.h"
#include "temp.h"

#define MPC9808_BUS     "/dev/i2c-2"
#define MPC9808_ADR     0x18
#define TA_REG          0x05
#define MANID_REG       0x06
#define DEVID_REG       0x07

#define LCD_ADDRESS 0x3E
#define LCD_RGB_ADDRESS 0x62
#define I2C_DEV "/dev/i2c-2"

int lcd_fd;
int lcd_bl_fd;
int buzzer_fd;
int temp_fd;

int main() {

    //int tempFile = i2c_init(MPC9808_BUS, MPC9808_ADR);
    // Open the I2C device
    temp_fd = init_temp(MPC9808_BUS, MPC9808_ADR);

    lcd_fd = init_lcd_fd(I2C_DEV, lcd_fd, LCD_ADDRESS);

    lcd_bl_fd = init_lcd_bl_fd(I2C_DEV, lcd_bl_fd, LCD_RGB_ADDRESS);

    /*if ((buzzer_fd = init_buzzer(I2C_DEV)) < 0) {
        perror("Failed to initialize the buzzer");
        return 1;
    }*/

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
        float temperature = read_temperature(temp_fd, TA_REG);

        // Print a message to the LCD
        char tempStr[20];
        snprintf(tempStr, sizeof(tempStr), "Temp: %.2f", temperature);
        if (temperature < 20) {
            lcdCommand(lcd_bl_fd, 0x01, 0x8);
        } else if (temperature > 24) {
            lcdCommand(lcd_bl_fd, 0x10, 0x8);
            // Turn the buzzer on
            //buzzer_on(buzzer_fd);
        } else {
            lcdCommand(lcd_bl_fd, 0x04, 0x8);
            // Turn the buzzer off
            //buzzer_off(buzzer_fd);
        }

        for (int i = 0; tempStr[i] != '\0'; i++) {
            lcdData(lcd_fd, tempStr[i]);
        }
        usleep(500000);
    }

    // Close the I2C connection
    close(lcd_fd);
    close(lcd_bl_fd);
    close(buzzer_fd);

    return 0;
}

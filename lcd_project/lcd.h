extern void lcdCommand(int fd, unsigned char command, int cb);

extern void lcdData(int fd, unsigned char data);

extern int init_lcd_fd(char* device, int lcd_fd, int lcd_address);

extern int init_lcd_bl_fd(char* device, int lcd_bl_fd, int lcd_rgb_address);

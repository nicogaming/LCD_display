# I2C Project In C
This project is made for a school project by Kevin and Nicolai.

### Compiling
Compile with `gcc main.c display.c temp.c buzzer.c -o program -li2c`

### LCD Service
This service is for running the LCD and temperature reader. 

As you can see we have create the `lcd.service` file. All you need to do is either: 
A. Create your own service file in `/etc/systemd/system/` and copy the code into that file.
B. Just import it in to the folder.

Remember to adjust the `ExecStart` in the service to the path where your compiled C program is located.

Remember to also run `systemctl enable lcd.service` for make it running on startup ;)



#include <lpc213x.h>
#define RS (1 << 0)
#define RW (1 << 1)
#define EN (1 << 2)
class LCD {
public:
LCD() {
initialize();
}
void initialize() {
// Set the pin directions for LCD
IODIR0 = 0x00003FC7;
IOCLR0 = 0x00003FC7;
// Initialize LCD in 8-bit mode
sendCommand(0x38); // Function Set: 8-bit mode, 2 lines
sendCommand(0x0E); // Display ON, Cursor ON
sendCommand(0x80); // Set cursor at the beginning
}
void sendCommand(const char command) {
IOCLR0 = 0x00003Fc7; // Clear control lines
IOCLR0 = RS; // RS = 0 for command
IOSET0 = command << 6; // Set the command on the data lines
pulse(); // Send the pulse to latch data
}
void sendData(const char data) {
IOCLR0 = 0x00003Fc7; // Clear control lines
IOSET0 = RS; // RS = 1 for data
IOSET0 = data << 6; // Set the data on the data lines
pulse(); // Send the pulse to latch data
}
void pulse() {
IOSET0 = EN;
delay(1000);
IOCLR0 = EN;
delay(1000);
}
void delay(int n) {
while (n--) {
// Empty loop for delay
}
}
void displayTemperature(int temperature) {
int f1, s1, f2, f3, f4;
s1 = (temperature * 500) / 1023;
f1 = s1 / 100;
f2 = s1 % 100;
f3 = f2 / 10;
f4 = f2 % 10;

sendCommand(0x80); // Move cursor to beginning of screen
sendData(f1 + 0x30); // Convert to ASCII and send
sendData(f3 + 0x30);
sendData(f4 + 0x30);
sendData(223); // '°' symbol
sendData('C'); // 'C' character for Celsius
}
private:
int a, b, c, d, e, f, g, h;
int f1, s1, f2, f3, f4;
};
class ADC {
public:
ADC() {
initialize();
}
void initialize() {
// Set the ADC pins (for channels 0 and 1)
PINSEL0 = 0x000000C0;
PINSEL1 = 0x00080000;
// Set the ADC control register: ADC operating at 60 MHz PLL clock
AD0CR = 0x01200280; // Enable ADC0, start conversion on channel 0
// Set ADC clock divider to ensure ADC clock is = 15 MHz
// System clock = 60 MHz, so divide by 4 to get 15 MHz for ADC
AD0CR |= (4 << 8); // CLKDIV set to 4
}
int readTemperature() {
// Wait for the ADC conversion to complete
while (!(AD0DR & 0x80000000)) {
continue;
}
// Read ADC result
int result = (AD0DR & 0x0000FFC0) >> 6;
return result; // Return ADC value (temperature value)
}
};
int main() {
// Initialize the LCD and ADC
LCD lcd;
ADC adc;
// Main loop
while (true) {
// Read the ADC value for temperature
int adcValue = adc.readTemperature();
// Display the temperature on the LCD
lcd.displayTemperature(adcValue);
}
return 0;
}
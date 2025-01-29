#include <lpc213x.h>
#define RS (1 << 0)
#define RW (1 << 1)
#define EN (1 << 2)
class LCD {
public:
    LCD() {
        IODIR0 |= 0x00003FC7;
        IOCLR0 = 0x00003FC7;
        initLCD();
    }
    void initLCD() {
        address(0x38);  
        address(0x0E);  
        address(0x80);  
    }
    void address(const char a) {
        IOCLR0 = 0x00003Fc7;  
        IOCLR0 = RS;          
        IOSET0 = a << 6;      
        pulse();
    }
    void data(const char d) {
        IOCLR0 = 0x00003Fc7;  
        IOSET0 = RS;           
        IOSET0 = d << 6;       
        pulse();
    }
    void pulse() {
        IOSET0 = EN;
        delay(1000);
        IOCLR0 = EN;
        delay(1000);
    }
    void delay(int n) {
        while (n--);
    }
    void displayTemperature(int temp) {
        int f1, f2, f3, f4;
        f1 = temp / 100;  
        f2 = temp % 100;  
        f3 = f2 / 10;
        f4 = f2 % 10;
        address(0x80); 
        data(f1 + 0x30);  
        data(f3 + 0x30);  
        data(f4 + 0x30);  
        data(223);         
        data('C');         
    }
};
class ADC {
public:
    ADC() {
        PINSEL0 |= 0x000000C0;  
        PINSEL1 |= 0x00080000; 
    }
    int readADC() {
        AD0CR = 0x01200280;
        while (!(AD0DR & 0x80000000)) continue;  
        AD0CR = 0x00200200;  
        int result = (AD0DR & 0x0000FFC0) >> 6;
        return result;
    }
    int calculateTemperature(int adcValue) {
        return (adcValue * 500) / 1023; 
    }
};
class System {
public:
    System() : lcd(), adc() {}
    void run() {
       while (1) {
            int adcValue = adc.readADC(); 
            int temperature = adc.calculateTemperature(adcValue);  
            lcd.displayTemperature(temperature); 
        }
    }
private:
    LCD lcd;  
    ADC adc;  
};
int main() {
    System sys;  
    sys.run();   
    return 0;
}
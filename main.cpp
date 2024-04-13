#include "mbed.h"
#include "C12832.h"                     // LCD display library


AnalogOut   aout(p18);                  // Application Board 3.5mm jack
AnalogIn    pot1(p17);                  // Potentiometer connected to p17
C12832      lcd(p5, p7, p6, p8, p11);   // LCD display
BusIn       up(p15);                    // Application Board Up
BusIn       down(p12);                  // .. Down
DigitalIn   press(p14);                 // Application Board joystick click

class FnGenerator {
public:
    static const float PI;
    static AnalogOut aout;
    static const int TSIZE = 4096;
    static float waveTable[TSIZE];
    static volatile int index;
    static volatile float frequency;   
    static float phaseIncrement;       // To control the rate of index increment

    static void initSineTable() {
        for (int i = 0; i < TSIZE; ++i) {
            waveTable[i] = 0.5f + 0.5f * sin(2.0 * PI * i / TSIZE);
        }
    }
    static void initSquareTable() {
    for (int i = 0; i < TSIZE; ++i) {
        if (i < TSIZE / 2) 
            waveTable[i] = 1.0f; 
        else 
            waveTable[i] = 0.0f;
        }
    }

    static void updateOutput() {
        aout = waveTable[index];
        index = (index + int(phaseIncrement)) % TSIZE;
    }

    static void updateFrequency(float newFrequency, float sampleRate) {
        frequency = newFrequency;
        phaseIncrement = frequency / sampleRate * TSIZE;  
    }
};

// Initialize static members
AnalogOut FnGenerator::aout(p18);
static const int sampleRate = 20000;
const float FnGenerator::PI = 3.14159265358979323846;
float FnGenerator::waveTable[FnGenerator::TSIZE];
volatile int FnGenerator::index = 0;
volatile float FnGenerator::frequency = 0;
float FnGenerator::phaseIncrement = FnGenerator::frequency / 44100 * FnGenerator::TSIZE;

int main() {
    int fnLabelPos = 0;                 //0 sine, 1 square, 3 triangle, 4 noise
    FnGenerator::initSineTable();
    //FnGenerator::initSquareTable();
    Ticker ticker;
    ticker.attach(&FnGenerator::updateOutput, 1.0 / sampleRate);
    lcd.printf("Sine");


    while (true) {
        if(up && fnLabelPos != 0){
            ticker.detach();
            lcd.cls();
            lcd.locate(0, 0);
            lcd.printf("Sine");
            FnGenerator::initSineTable();
            ticker.attach(&FnGenerator::updateOutput, 1.0 / sampleRate);
            fnLabelPos -= 1;
        }
        else if(down && fnLabelPos != 1){
            ticker.detach();
            lcd.cls();
            lcd.locate(0, 0);
            lcd.printf("Square");
            FnGenerator::initSquareTable();
            ticker.attach(&FnGenerator::updateOutput, 1.0 / sampleRate);
            fnLabelPos += 1;
        }
        float readValue = pot1.read();  
        float newFrequency = float(readValue * 999);  
        FnGenerator::updateFrequency(newFrequency, sampleRate);
        
        lcd.locate(0, 15);
        lcd.printf("Freq: %.2f Hz", FnGenerator::frequency);
        
        
    }
}

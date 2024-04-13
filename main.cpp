#include "mbed.h"
#include "C12832.h"                     // LCD display library


AnalogOut   aout(p18);                    // Application Board 3.5mm jack
AnalogIn    pot1(p17);                     // Potentiometer connected to p17
C12832      lcd(p5, p7, p6, p8, p11);        // LCD display
BusIn       joy(p15, p12, p13, p16);
DigitalIn   press(p14);

class FnGenerator {
public:
    static const double PI;
    static AnalogOut aout;
    static const int TSIZE = 4096;
    static float waveTable[TSIZE];
    static volatile int index;
    static volatile double frequency;   // Add frequency control
    static double phaseIncrement;       // To control the rate of index increment

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

    static void updateFrequency(double newFrequency, float sampleRate) {
        frequency = newFrequency;
        phaseIncrement = frequency / sampleRate * TSIZE;  
    }
};

// Initialize static members
AnalogOut FnGenerator::aout(p18);
static const int sampleRate = 44100;
const double FnGenerator::PI = 3.14159265358979323846;
float FnGenerator::waveTable[FnGenerator::TSIZE];
volatile int FnGenerator::index = 0;
volatile double FnGenerator::frequency = 28.0;
double FnGenerator::phaseIncrement = FnGenerator::frequency / 44100 * FnGenerator::TSIZE;

int main() {
    int fnLabelPos = 0;                     //0 sine, 1 square, 3 triangle, 4 noise
    FnGenerator::initSineTable();
    //FnGenerator::initSquareTable();
    Ticker ticker;
    ticker.attach(&FnGenerator::updateOutput, 1.0 / sampleRate);
    lcd.printf("Sine");


    while (true) {

        float readValue = pot1.read();  
        double newFrequency = 28.0 + double(readValue * 1000);  
        FnGenerator::updateFrequency(newFrequency, sampleRate);
        
        lcd.locate(0, 15);
        lcd.printf("Freq: %.2f Hz", FnGenerator::frequency);
        
        
    }
}

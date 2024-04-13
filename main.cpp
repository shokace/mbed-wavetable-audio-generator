#include "mbed.h"
#include "C12832.h"  // LCD display library


AnalogOut aout(p18);
AnalogIn pot1(p17); // Potentiometer connected to p17
C12832 lcd(p5, p7, p6, p8, p11); // LCD display

class SineGenerator {
public:
    static const double PI;
    static AnalogOut aout;
    static const int TSIZE = 1024;
    static float sineTable[TSIZE];
    static volatile int index;
    static volatile double frequency;  // Add frequency control
    static double phaseIncrement;  // To control the rate of index increment

    static void initSineTable() {
        for (int i = 0; i < TSIZE; ++i) {
            sineTable[i] = 0.5f + 0.5f * sin(2.0 * PI * i / TSIZE);
        }
    }

    static void updateOutput() {
        aout = sineTable[index];
        index = (index + int(phaseIncrement)) % TSIZE;
    }

    static void updateFrequency(double newFrequency, float sampleRate) {
        frequency = newFrequency;
        phaseIncrement = frequency / sampleRate * TSIZE;  
    }
};

// Initialize static members
static const float sampleRate = 10000;
const double SineGenerator::PI = 3.14159265358979323846;
AnalogOut SineGenerator::aout(p18);
float SineGenerator::sineTable[SineGenerator::TSIZE];
volatile int SineGenerator::index = 0;
volatile double SineGenerator::frequency = 440.0;
double SineGenerator::phaseIncrement = SineGenerator::frequency / 10000.0 * SineGenerator::TSIZE;

int main() {
    SineGenerator::initSineTable();
    Ticker ticker;
    ticker.attach(&SineGenerator::updateOutput, 1.0 / sampleRate);

    lcd.cls();
    lcd.printf("Sine Wave Generator");

    while (true) {
        float readValue = pot1.read();  
        double newFrequency = 440.0 + double(readValue * 1000);  
        SineGenerator::updateFrequency(newFrequency, sampleRate);

        //lcd.locate(0, 15);
        //lcd.printf("Freq: %.2f Hz", SineGenerator::frequency);
        //wait(0.1);  // Update frequency readout at a reasonable rate
    }
}

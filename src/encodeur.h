/* 
Read optical incremental rotary encoder output.
Serial output columns:
    
    1. Absolute encoder position (pulses, integer)
    2. Change in encoder position (pulses, integer)
    3. Time since Arduino start in (microseconds, integer)
    4. Change in time (microseconds, integer)
    5. Speed (pulses per second, float)
Working well.
volatile directive is used for variables that are modified in the
interrupt service routines.
*/

#include <Arduino.h>

// Pin definitions.
// - enc_a is ENC Signal A line (Arduino digital pin 2)
// - enc_b is ENC Signal B line (Arduino digital pin 3)
#define ENC_A  2
#define ENC_B  3

// Main loop refresh period.
#define REFRESH_MS  50

// Main serial data connection to computer.
#define BAUD_RATE   115200

class encodeur
{
private:
    // Encoder signal line states
    volatile boolean state_a = 0;
    volatile boolean state_b = 0;

    // Encoder position
    volatile int enc_pos = 0;
    int enc_pos_prev = 0;
    int enc_pos_change = 0;
    float enc_pos_deg = 0;

    // Timing
    unsigned long micros_current = 0;
    unsigned long micros_prev = 0;
    long micros_change = 0;
public:
    encodeur(/* args */);
    ~encodeur();
    void interrupt_enc_a();
    void interrupt_enc_b();
    void display();
};



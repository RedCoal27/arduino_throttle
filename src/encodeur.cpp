#include "encodeur.h"

encodeur::encodeur(/* args */)
{
    pinMode(ENC_A, INPUT);
    pinMode(ENC_B, INPUT); 

    state_a = (boolean) digitalRead(ENC_A);
    state_b = (boolean) digitalRead(ENC_B);


  
    micros_prev = micros();

    Serial.begin(BAUD_RATE);
}


encodeur::~encodeur()
{
}


void encodeur::display()
{
    // Calculate elapsed time  
    micros_current = micros();
    if (micros_current < micros_prev) {
        micros_change = micros_current + (4294967295 - micros_prev);
    } else {
        micros_change = micros_current - micros_prev;
    }

    // Calculate change in encoder position.
    enc_pos_change = enc_pos - enc_pos_prev;
    enc_pos_change = abs(enc_pos_change);

    enc_pos_deg = enc_pos / 500.0 * 360.0; // 500 pulses par revolution 

    // Emit data
    Serial.print("Position en pulse : ");
    Serial.print(enc_pos);
    Serial.print("\t");
    Serial.print("Position en deg : ");
    Serial.print(enc_pos_deg);
    Serial.print("\t");
    Serial.print("Vitesse en pulse par seconde");
    Serial.print(enc_pos_change / (micros_change / 1e6));
    Serial.print("\n");

    enc_pos_prev = enc_pos;
    micros_prev = micros_current;
}

// Detect pulses from depth encoder.
void encodeur::interrupt_enc_a()
{
    if (!state_a) {
        state_b ? enc_pos++: enc_pos--;         
    }
    state_a = !state_a;
}


void encodeur::interrupt_enc_b()  
{
    state_b = !state_b;
}

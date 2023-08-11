#pragma once
#include <Arduino.h>
// Include the Arduino Stepper Library
#include <Stepper.h>
#include "encodeur.h"
#include "string.h"
// Number of steps per output rotation
// 200 pour 1.8 degrees
#define stepsPerRevolution 200
#define Motor1 12
#define Motor2 11
#define Motor3 10
#define Motor4 9

#define Tension1 A1
#define Tension2 A2
#define Tension3 A3
#define Tension4 A4

#define OpenSignal 6
#define ClosedSignal 7

#define enabled_motor 4

#define Speed 87 


class Motor:private Stepper,public encodeur
{
private:
    float* Tension;
    bool Open;
    bool Closed;
    int steps_left;
    uint8_t delta_home;
    bool available;
public:
    int position;
    Motor();
    ~Motor();
    void send_state();
    void read_tension();
    bool has_to_stop();
    void send_data(int);
    uint16_t read_state();
    int step(int steps_to_move, bool bypass);
    void move_at_position(int new_position);
    void home();
    void open();
    void close();
    void is_available();
};


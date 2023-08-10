#include "motor.h"

Motor::Motor(/* args */) : Stepper(stepsPerRevolution, Motor1, Motor2, Motor3, Motor4),encodeur()
{
    pinMode(enabled_motor,OUTPUT);
    digitalWrite(enabled_motor,0);
    pinMode (OpenSignal, INPUT_PULLUP);
    pinMode (ClosedSignal, INPUT_PULLUP);
    Tension = new float[4];
    for(int i= 0; i<4;i++)
            Tension[i] = 0;
    // 87 rpm pour la vitesse normale
    setSpeed(Speed);
}

Motor::~Motor()
{
    delete[] Tension;
}

bool Motor::read()
{
    float* Temp = new float[4];
    Temp[0] = float((analogRead(Tension1))) / 1023 * 5;
    Temp[1] = float((analogRead(Tension2))) / 1023 * 5;
    Temp[2] = float((analogRead(Tension3))) / 1023 * 5;
    Temp[3] = float((analogRead(Tension4))) / 1023 * 5;
    for(int i= 0; i<4;i++)
        if(Temp[i]>Tension[i])
            Tension[i] = Temp[i];
    delete[] Temp;
    Open = digitalRead(OpenSignal);
    Closed = digitalRead(ClosedSignal);
    if(Open==1 && direction==1){
        //Serial.println("Open");
        return true;
    }
    if(Closed==1 && direction==0){
        //Serial.println("Closed");
        return true;
    }
    return false;
}

void Motor::send_data(int extra_step){
    Serial.println(String(Tension[0])+"|"+String(Tension[1])+"|"+String(Tension[2])+"|"+String(Tension[3])+"|"+String(Open)+"|"+String(Closed)+"|"+String(extra_step)+"\n");
}

void Motor::reset_tension(){
    for(int i= 0; i<4;i++)
            Tension[i] = 0;
}



/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
void Motor::step(int steps_to_move)
{
    digitalWrite(enabled_motor,1);
    steps_left = abs(steps_to_move); // how many steps to take
    int extra_step = 0;
    // determine direction based on whether steps_to_mode is + or -:
    if (steps_to_move > 0)
    {
        direction = 1;
    }
    if (steps_to_move < 0)
    {
        direction = 0;
    }
    reset_tension();
    bool stop = read();
    if(stop==1 && direction==0){
        steps_left = 0;
    }

    // decrement the number of steps, moving one step each time:
    while (steps_left > 0)
    {
        unsigned long now = micros();
        // move only if the appropriate delay has passed:
        if (now - last_step_time >= step_delay)
        {
            // get the timeStamp of when you stepped:
            last_step_time = now;
            // increment or decrement the step number,
            // depending on direction:
            if (direction == 1)
            {
                step_number++;
                if (step_number == number_of_steps)
                {
                    step_number = 0;
                }
            }
            else
            {
                if (step_number == 0)
                {
                    step_number = number_of_steps;
                }
                step_number--;
            }
            // decrement the steps left:
            steps_left--;
            // step the motor to step number 0, 1, ..., {3 or 10}
            stepMotor(step_number % 4);

            stop = read();
            if(stop == 1 && extra_step==0){
                extra_step=steps_left;
            if(steps_left>30 && direction==1)
                steps_left=30;
            else
                steps_left=0;
            }
        }
    };
    delay(20);
    send_data(extra_step);
    digitalWrite(motor_pin_1, LOW);
    digitalWrite(motor_pin_2, LOW);
    digitalWrite(motor_pin_3, LOW);
    digitalWrite(motor_pin_4, LOW);
    digitalWrite(enabled_motor,0);
}
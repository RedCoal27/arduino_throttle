#include "motor.h"

Motor::Motor() : Stepper(stepsPerRevolution, Motor1, Motor2, Motor3, Motor4),encodeur()
{
    pinMode(enabled_motor,OUTPUT);
    digitalWrite(enabled_motor,1);
    pinMode (OpenSignal, INPUT_PULLUP);
    pinMode (ClosedSignal, INPUT_PULLUP);
    Tension = new float[4];
    for(uint8_t i= 0; i<4;i++)
            Tension[i] = 0;
    // 87 rpm pour la vitesse normale
    setSpeed(Speed);
}

Motor::~Motor()
{
    delete[] Tension;
}

void Motor::read_tension(){
    float* Temp = new float[4];
    Temp[0] = float((analogRead(Tension1))) / 1023 * 5;
    Temp[1] = float((analogRead(Tension2))) / 1023 * 5;
    Temp[2] = float((analogRead(Tension3))) / 1023 * 5;
    Temp[3] = float((analogRead(Tension4))) / 1023 * 5;
    for(uint8_t i= 0; i<4;i++)
        if(Temp[i]>Tension[i])
            Tension[i] = Temp[i];
    delete[] Temp;
}

uint8_t Motor::read_state(){
    Open = digitalRead(OpenSignal);
    Closed = digitalRead(ClosedSignal);
    return (Open<<1 | Closed);
}

bool Motor::has_to_stop()
{
    read_state();
    if(Open == 1 && direction==1){
        //Serial.println("Open");
        return true;
    }
    if(Closed == 1 && direction==0){
        //Serial.println("Closed");
        return true;
    }
    return false;
}

void Motor::send_data(int extra_step){
    Open = digitalRead(OpenSignal);
    Closed = digitalRead(ClosedSignal);
    Serial.println(String(Tension[0])+"|"+String(Tension[1])+"|"+String(Tension[2])+"|"+String(Tension[3])+"|"+String(Open)+"|"+String(Closed)+"|"+String(extra_step)+"\n");
}

void Motor::send_state(){
    Serial.write(read_state());
}


/*
 * Moves the motor steps_to_move steps.  If the number is negative,
 * the motor moves in the reverse direction.
 */
uint16_t Motor::step(uint16_t steps_to_move, bool bypass)
{
    steps_left = abs(steps_to_move); // how many steps to take
    // determine direction based on whether steps_to_mode is + or -:
    if (steps_to_move > 0)
    {
        direction = 1;
    }
    if (steps_to_move < 0)
    {
        direction = 0;
    }
    bool stop = has_to_stop();
    if(stop==1){
        if(bypass==0)
            steps_left = 0;
        else{
            if(steps_left > 100)
                steps_left = 100;
        }
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
            // increment or decrement the position,
            // depending on direction:
            if (direction == 1)
                position++;
            else
                position--;
            // decrement the steps left:
            steps_left--;
            // step the motor to step number
            stepMotor(position % 4);
            stop = has_to_stop();
            //si bypass est à 0 et qu'un capteur est détécté, arreter la et retourner le nombre de pas restant non effectué
            if(stop == 1){
                if(bypass == 0){
                    return steps_left;
                }
                else{ 
                    if(steps_left > 100)
                        steps_left = 100;
                }
            }
        }
    };
    return 0;
}

void Motor::move_at_position(uint16_t new_position){
    step(new_position-position,false);
}

void Motor::home(){
    position = 1000;
    step(100, true);
    step(-1000, true);
    step(-(position%4), true);//pour bien placer les pas sur un vrai 0
    delay(100);
    position = 0;
    Serial.println(position);
    step(1, false);

    while(Closed == 1){
        step(1, false);
        read_state();
        Serial.println(Closed);
    };
    step(-1,true);
    delta_home = position;
}

void Motor::open(){
    move_at_position(1000);
    Serial.println(position);
}

void Motor::close(){
    move_at_position(0);
    Serial.println(position);
}

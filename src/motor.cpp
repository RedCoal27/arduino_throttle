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

int Motor::read_tension(uint8_t input){
    int tension = 0;
    switch(input){
        case 0:
            tension = analogRead(Tension1);
            break;
        case 1:
            tension = analogRead(Tension2);
            break;
        case 2:
            tension = analogRead(Tension3);
            break;
        case 3:
            tension = analogRead(Tension4);
            break;
        default:
            break;
    }
    return tension;
}


uint16_t Motor::read_state(){
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
int Motor::step(int steps_to_move)
{
    digitalWrite(enabled_motor,1);
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
                    // digitalWrite(enabled_motor,0);//coupe le moteur
                    return steps_left;
            }
        }
    };
    // digitalWrite(enabled_motor,0);//coupe le moteur
    return 0;
}

void Motor::move_at_position(int new_position){
    step(new_position-position);
}

void Motor::home(){
    position = 1000;
    step(200);
    step(-1000);
    delay(100);
    position = 0;
    step(1);
    setSpeed(Speed/4);
    while(Closed == 1){
        step(1);
        delay(5);
        read_state();
    };
    delta_home = position;
    step(-1);
    delta_home = delta_home - position; //différence activation désactivation capteur dans un sens puis l'autre
    setSpeed(Speed);
    position = 0;
}

void Motor::open(){
    move_at_position(800);
    // Serial.println(position);
}

void Motor::close(){
    move_at_position(0);
    // Serial.println(position);
}

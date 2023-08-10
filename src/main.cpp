#include <Arduino.h>
#include "motor.h"

Motor _Motor;

int test = 0;


//fonction a attacher parce que arduino chiant
void interrupt_a(){
  _Motor.interrupt_enc_a();
}

//fonction a attacher parce que arduino chiant
void interrupt_b(){
  _Motor.interrupt_enc_b();
}


void setup()
{
	// initialize the serial port:
	Serial.begin(115200);
  attachInterrupt(0, interrupt_a, CHANGE);
  attachInterrupt(1,interrupt_b, CHANGE); 
  delay(500);
}

const int steps = 880;

void loop() 
{
  Serial.flush();
  while(!Serial.available());
  _Motor.step(Serial.readStringUntil('\n').toInt());
}
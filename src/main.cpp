#include <Arduino.h>
#include "motor.h"
#include <Wire.h>

Motor _Motor;
uint8_t _register = 0;

uint16_t last_steps_value = 0;
uint16_t steps_to_move = 0;

// Adresse I2C de l'esclave
#define I2C_ADDRESS 0x01

// step_count
void interrupt_a(){
  _Motor.interrupt_enc_a();
}

// step_count
void interrupt_b(){
  _Motor.interrupt_enc_b();
}



// Cette fonction est appelée lorsque des données sont reçues d'un maître I2C
void receiveData(int numBytes) {
  if(numBytes == 1){
    _register = Wire.read(); // Lisez la valeur envoyée par le maître I2C
    return;
  }
  if(numBytes == 2){
    steps_to_move = Wire.read()<<8 | Wire.read();
  };
}

// Cette fonction est appelée lorsque le maître I2C demande des données
void sendData() {
  int data = 0;
  if(_register==0x01){
    data = _Motor.read_state();
  }
  if(_register==0x02){
    data = _Motor.position;
  }
  Wire.write(data>>8 & 0xFF); // Envoyez les données au maître I2C
  Wire.write(data & 0xFF);
  _register = 0;
}


void setup() {
  Serial.begin(115200);
  Wire.setTimeout(10);
  // Configurer les interruptions
  attachInterrupt(0, interrupt_a, CHANGE);
  attachInterrupt(1, interrupt_b, CHANGE); 

  // Configurer I2C en mode esclave
  Wire.begin(I2C_ADDRESS); // Démarrez I2C en tant qu'esclave avec l'adresse définie
  Wire.onReceive(receiveData); // Fonction à appeler lorsque des données sont reçues
  Wire.onRequest(sendData); // Fonction à appeler lorsque des données sont demandées
  delay(500);
  _Motor.home();
  delay(1000);
}

void loop() {
  if(last_steps_value != steps_to_move){
    last_steps_value = steps_to_move;
    _Motor.move_at_position(steps_to_move);
  }
}
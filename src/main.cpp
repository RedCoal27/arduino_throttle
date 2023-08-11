#include <Arduino.h>
#include "motor.h"
#include <Wire.h>

Motor _Motor;
uint8_t _register = 0;

// Adresse I2C de l'esclave
#define I2C_ADDRESS 0x04

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
  int steps = Wire.read()<<8 | Wire.read();
  _Motor.move_at_position(steps);
}

// Cette fonction est appelée lorsque le maître I2C demande des données
void sendData() {
  int sensorData = 0;
  if(_register==0x01)
    sensorData = _Motor.read_state();
  if(_register==0x02)
    sensorData = _Motor.position;
  Wire.write(sensorData>>8 & 0xFF); // Envoyez les données au maître I2C
  Wire.write(sensorData & 0xFF);
  _register = 0;
}


void setup() {
  Serial.begin(115200);
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
  // _Motor.open();
  // delay(2000);
  // _Motor.close();
  // delay(2000);
}
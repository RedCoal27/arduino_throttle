#include <Arduino.h>
#include "motor.h"
#include <Wire.h>

Motor _Motor;

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
  if(Wire.available()) {
    int steps = Wire.read(); // Lisez la valeur envoyée par le maître I2C
    _Motor.step(steps);
  }
}

// Cette fonction est appelée lorsque le maître I2C demande des données
void sendData() {
  // // Supposons que vous ayez une méthode `_Motor.getSensorData()` qui renvoie la valeur du capteur.
  // // Si ce n'est pas le cas, remplacez par votre propre logique pour obtenir des données du capteur.
  // int sensorData = _Motor.getSensorData();
  // Wire.write(sensorData); // Envoyez les données au maître I2C
}


void setup() {
  // Configurer les interruptions
  attachInterrupt(0, interrupt_a, CHANGE);
  attachInterrupt(1, interrupt_b, CHANGE); 

  // Configurer I2C en mode esclave
  Wire.begin(I2C_ADDRESS); // Démarrez I2C en tant qu'esclave avec l'adresse définie
  Wire.onReceive(receiveData); // Fonction à appeler lorsque des données sont reçues
  Wire.onRequest(sendData); // Fonction à appeler lorsque des données sont demandées
  delay(500);
}

void loop() {
  // Le traitement principal peut être ici. Les fonctions I2C seront appelées
  // automatiquement à mesure que les événements I2C se produisent.
}
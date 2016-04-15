/****************************************************************
*                Ecriture série des valeurs servos              *
****************************************************************/
        
        
        //SERVO 
#include <Servo.h>
int i=-1;
int y=0;
const int nbrServos=6;                              //Nombre de servomoteurs
const int brocheServo[nbrServos]={3,5,6,9,13,11};   //Choix des broches pour les servomoteurs 3 5 6 9 10 11
//const int brocheServo[nbrServos]={8,9};
int angle[nbrServos];
const int angle0[nbrServos]={90,180,0,90,90,100}; //90 0 180 180 90
//int angle0[nbrServos]={90,90};
Servo servo[nbrServos];

//Servo 2 : angle+ = montée : OK
//Servo 3 : angle+ = montée : ANGLE A INVERSER !!
//          Départ:180 degrés
//Servo 4 : angle+ = montée


        //VOLTMETRE
int batteryLevel;
float voltage;
int mesure_voltage;
  //R1=97.9 kOhms 
  //R2=98.8 kOhms
float periode=0; //permet de relentir la cadence d'envoi de la tension

int pinLED=12;



void setup() {
  Serial.begin(9600);
  voltmetre();
  Serial.print("Voltage = ");Serial.print(voltage);Serial.println(" V ");Serial.println();
  
  
  if (voltage<6.5) 
  {
    Serial.println("CRITICAL ERROR : BATTERY TOO LOW !");
    while(1);
  }
  
  for (int j=0 ; j<2 ; j++){
    servo[j].attach(brocheServo[j], 800, 2100);     //Necessité de reduire la largeur de la PWM pour la restreindre (course des premiers servos~140)
  }
  for (int j=2 ; j<nbrServos ; j++){
    servo[j].attach(brocheServo[j], 550, 2450);  //Pour servo poignet1 : 650,2400?
    //Par défaut : 500,2500, par sécurité j'enlève 50us pour pas arriver en butée mécanique
  }

  for (int k=0; k<nbrServos; k++){
    servo[k].write(angle0[k]);
  }
  
  y=0;
  i=-1;
}

void loop() {
  if (periode>16000000){
    voltmetre();
    periode=0;
    //Serial.println("loop");
  }
  
  while (Serial.available()){

      int c=Serial.read();
      if(c==250) {
         Serial.println(" ");
         i=0; 
      }
      /*if(c==65) { //MODE 1
        glowingLED(30);
      }
      if(c==202) { //MODE 2
        stillLED();
      }
      if(c==203) { //MODE 3
        glowingLED(30);
      }*/
      else {
         //Serial.print(c);
         //Serial.print(" ");
         if(i>=0 && i<nbrServos && voltage>6.5) {
            angle[i]=c;
            //Serial.print("Servo numero ");
            Serial.print(i+1);
            Serial.print("=");
            Serial.print(angle[i]);
            Serial.print(" / ");
            servo[i].write(angle[i]);
            i++;
         } 
         if(i==nbrServos) {
            y=0;
            i=-1; 
         }
      }
  
  }
  
  periode++;
 }
 
 
void voltmetre(){
    mesure_voltage = analogRead(0);
    voltage = mesure_voltage / (255/5);
    voltage = voltage * 0.50229;
    batteryLevel=(voltage-6.6)*100/2.4;
    if(batteryLevel > 0) Serial.println(batteryLevel);
    //Serial.print(voltage);
    //Serial.println(" V ");
    //else Serial.println("error");
}
 
 
void glowingLED(int periodeLED){
 for(int fadeValue = 0 ; fadeValue <= 255; fadeValue +=5) { 
    analogWrite(pinLED, fadeValue);         
    // wait for periodeLED milliseconds to see the dimming effect    
    delay(periodeLED);                            
  } 

  for(int fadeValue = 255 ; fadeValue >= 0; fadeValue -=5) { 
    analogWrite(pinLED, fadeValue);         
    // wait for periodeLED milliseconds to see the dimming effect    
    delay(periodeLED);                            
  } 
}
 
void flashingLED(int periodeLED){
  analogWrite(pinLED,250);
  delay(periodeLED);
  analogWrite(pinLED,0);
  delay(periodeLED);
}
 
void stillLED(){
  analogWrite(pinLED,250);
}

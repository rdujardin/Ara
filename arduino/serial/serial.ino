/****************************************************************
*                Ecriture série des valeurs servos              *
****************************************************************/
#define MODE_BRAS 0
#define MODE_TANK 1
int mode = MODE_BRAS;

        //SERVOS 
#include <Servo.h>
int i=-1;
int y=0;
const int nbrServos=6;                              //Nombre de servomoteurs
const int brocheServo[nbrServos]={3,5,6,9,13,11};   //Choix des broches pour les servomoteurs 3 5 6 9 10 11
int angle[nbrServos];
const int angle0[nbrServos]={90,180,0,90,180,90}; //90 0 180 180 90
Servo servo[nbrServos];

//Servo 2 : angle+ = montée : OK
//Servo 3 : angle+ = montée : ANGLE A INVERSER !!
//          Départ:180 degrés
//Servo 4 : angle+ = montée


        //VOLTMETRE
int batteryLevel;
char batteryLevel_char;
float voltage;
int mesure_voltage;
  //R1=97.9 kOhms 
  //R2=98.8 kOhms
float periode=0; //permet de relentir la cadence d'envoi de la tension


        //softPWM
unsigned long currentMicros = micros();
unsigned long previousMicros = 0;

float PWM_frequency = 200;
unsigned long PWM_period = 1e6 / PWM_frequency; //Period = 1 / F(Hz) *  

int pinRightMotor = 12;
int pinLeftMotor = 13;


void setup() {
  Serial.begin(9600);
  voltmetre();
  //Serial.print("Voltage = ");Serial.print(mesure_voltage);Serial.println(" V ");Serial.println();
  
  while (voltage<6.5) 
  {
    Serial.print((char)150); //valeur arbitraire
    delay(100);
  }
  
  for (int j=0 ; j<2 ; j++){
    servo[j].attach(brocheServo[j], 800, 2100);     //Necessité de reduire la largeur de la PWM pour la restreindre (course des premiers servos~140)
  }
  for (int j=2 ; j<nbrServos ; j++){
    servo[j].attach(brocheServo[j], 600, 2400);  //Pour servo poignet1 : 650,2400?
    //Par défaut : 500,2500, par sécurité j'enlève 50us voire 100us pour pas arriver en butée mécanique
  }

  for (int k=0; k<nbrServos; k++){
    servo[k].write(angle0[k]);
  }
  
  y=0;
  i=-1;
  
  pinMode(pinLeftMotor,OUTPUT);
  pinMode(pinRightMotor,OUTPUT);
}

void loop() {
  if (periode>1600000){
    voltmetre();
    periode=0;
    //Serial.println("loop");
  }
  
  if (mode == MODE_TANK) {
    softPWM(pinLeftMotor,puissance[0]);
    softPWM(pinRightMotor,puissance[1]);
  } 
  while (Serial.available()){

      int c=Serial.read();
      if(c==250) {
         Serial.println(" ");
         i=0; 
         mode = MODE_BRAS
      }
      if(c==254) {
         Serial.println(" ");
         i=0; 
         MODE = MODE_TANK:
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
         if(mode == MODE_BRAS && i>=0 && i<nbrServos && voltage>6.5) {
            angle[i]=c;
            //Serial.print("Servo numero ");
            Serial.print(i+1);
            Serial.print("=");
            Serial.print(angle[i]);
            Serial.print(" / ");
            servo[i].write(angle[i]);
            i++;
            if(i==nbrServos) {
              y=0;
              i=-1; 
            }
         } 
         if(mode == MODE_TANK && i>=0 && i<2 && voltage>6.5) {
            puissance[i]=c;
            //Serial.print("Servo numero ");
            Serial.print(i+1);
            Serial.print("=");
            Serial.print(puissance[i]);
            Serial.print(" / ");
            i++;
            if(i==2) {
              y=0;
              i=-1; 
            }
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
    batteryLevel_char=batteryLevel;
    if(batteryLevel > 0) Serial.print(batteryLevel_char);
    //Serial.print(voltage);
    //Serial.println(" V ");
    //Serial.println(batteryLevel);
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


/*************** softPWM  ***************/ 
void softPWM(int pin, int dutyCycle/* Percentage */) {
  currentMicros = micros(); //Time elapsed since the start of the start of the programm
  if (currentMicros - previousMicros <= dutyCycle*period/100) {
    digitalWrite(pin,HIGH);
    Serial.println("ON");
  }
  else {
    if ( currentMicros - previousMicros <= PWM_period) {
      digitalWrite(pin, LOW);
      Serial.println("OFF");    
    }
    else {
      digitalWrite(pin, HIGH);
      Serial.println("ON");
      previousMicros = currentMicros;
    }
  }  
}


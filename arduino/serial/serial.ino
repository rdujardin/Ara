/****************************************************************
*                Ecriture série des valeurs servos              *
****************************************************************/
 
#include <Servo.h>

int i=-1;
int y=0;
const int nbrServos=5;                              //Nombre de servomoteurs
const int brocheServo[nbrServos]={3,5,7,8,12};   //Choix des broches pour les servomoteurs 3 5 6 9 10 11
//const int brocheServo[nbrServos]={8,9};
int angle[nbrServos];
const int angle0[nbrServos]={90,90,95,180,90}; //90 0 180 180 90
//int angle0[nbrServos]={90,90};
Servo servo[nbrServos];

//SERVO 2 : angle+ = montée : OK
//SERVO 3 : angle+ = montée : ANGLE A INVERSER !!
//          Départ:180 degrés
//SERVO 4 : angle+ = montée



void setup() {
  Serial.begin(9600);
  
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

  while (Serial.available()){

      int c=Serial.read();
      if(c==250) {
         Serial.println(" ");
         i=0; 
      }
      else {
         //Serial.print(c);
         //Serial.print(" ");
         if(i>=0 && i<nbrServos) {
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
  
  
    //if (Serial.read()==65) {
      //servo[2].write(180);
      //Serial.println("caca");
      //angle[i%nbrServos] = Serial.read();
      //if (angle[nbrServos-1]>0) {   
        /*for (int k=0; k<nbrServos ; k++) {
          Serial.print("Servo numero ");
          Serial.println(k+1);
          Serial.println(angle[k]);
          servo[k].write(angle[k]);
          angle[k]=0;
        }
        delay(3);
      }*/
    //}
    //i++;
 //}
 }
